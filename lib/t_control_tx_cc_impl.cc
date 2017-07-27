/* -*- c++ -*- */
/*
 * Copyright 2016 <+YOU OR YOUR COMPANY+>.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <gnuradio/io_signature.h>
#include "t_control_tx_cc_impl.h"
#include <uhd/types/time_spec.hpp>
#include <sys/time.h>
#include <stdlib.h>
#include <uhd/usrp/multi_usrp.hpp>
#define _PI 3.14159265359

namespace gr {
  namespace inets {

    t_control_tx_cc::sptr
    t_control_tx_cc::make(int develop_mode, int block_id, double bps, double t_pretx_interval_s, int record_on, std::string file_name_extension, int name_with_timestamp, int antenna_number)
    {
      return gnuradio::get_initial_sptr
        (new t_control_tx_cc_impl(develop_mode, block_id, bps, t_pretx_interval_s, record_on, file_name_extension, name_with_timestamp, antenna_number));
    }

    /*
     * The private constructor
     */
    t_control_tx_cc_impl::t_control_tx_cc_impl(int develop_mode, int block_id, double bps, double t_pretx_interval_s, int record_on, std::string file_name_extension, int name_with_timestamp, int antenna_number)
      : gr::block("t_control_tx_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
        _develop_mode(develop_mode),
        _block_id(block_id),
        _last_tx_time(0),
        _t_pretx_interval_s(t_pretx_interval_s),
        _file_name_extension(file_name_extension),
        _name_with_timestamp(name_with_timestamp),
        _record_on(record_on),
        _phase(0),
        _bps(bps),
        _antenna_number(antenna_number)
    {
      if(_develop_mode)
        std::cout << "develop_mode of t_control_tx ID: " << _block_id << " is activated." << "and t_re is " << _t_pretx_interval_s << std::endl;
      if(_record_on)
      {
        time_t tt = time(0);   // get time now
        struct tm * now = localtime( & tt );
        std::ostringstream file_name;
        if(_name_with_timestamp)
          file_name << "/home/inets/source/gr-inets/results/" << (now->tm_year + 1900) << "_" << (now->tm_mon + 1) << "_" << now->tm_mday << "_" << now->tm_hour << "_" << now->tm_min << "_" << now->tm_sec << "_block" << _block_id << "_" << _file_name_extension << ".txt";
        else
          file_name << "/home/inets/source/gr-inets/results/" << _file_name_extension << ".txt";
        _file_name_str = file_name.str();
      }
      message_port_register_in(pmt::mp("phase_in"));
      set_msg_handler(pmt::mp("phase_in"), boost::bind(&t_control_tx_cc_impl::set_phase, this, _1)
      );
      //message_port_sub(pmt::mp("phase_out"), pmt::mp("direction_mapper"));
      struct timeval ti;
      gettimeofday(&ti, NULL);
      double pc_clock = ti.tv_sec + ti.tv_usec/1000000.0;
      if (antenna_number == 1) {
        uhd::device_addr_t dev_addr;
        dev_addr["addr0"] = "192.168.10.2";
        dev_addr["addr1"] = "192.168.10.3";
        _dev = uhd::usrp::multi_usrp::make(dev_addr);
        _dev->set_time_now(uhd::time_spec_t(pc_clock));

      // This command will be processed fairly soon after the last PPS edge:
      //_dev->set_time_next_pps(uhd::time_spec_t(0.0));
      uhd::time_spec_t usrp_time = _dev->get_time_now();
      int usrp_time_full = usrp_time.get_full_secs();
      double usrp_time_frac = usrp_time.get_frac_secs();
      double _time_sum = usrp_time_full + usrp_time_frac;
      std::cout << "USRP time: " << _time_sum << '\n';
      std::cout << "PC time: " << pc_clock  << '\n';
      }
    }

    /*
     * Our virtual destructor.
     */
    t_control_tx_cc_impl::~t_control_tx_cc_impl()
    {
    }

    void
    t_control_tx_cc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
      ninput_items_required[0] = noutput_items;
    }

    int
    t_control_tx_cc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      /*
       * signal is not changed through carrier sensing block, i.e., it simply output its input.
       */
      for(int i = 0; i < noutput_items; i++)
      {
        gr_complex temp = in[i];
        shift_the_phase(temp);
        out[i] = temp;
      }

      std::vector <tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + noutput_items);// + packet_length);

      if(process_tags_info(tags))
      {
        if(_develop_mode)
        {
          //std::cout << "++++++++++   t_control_tx_cc ID: " << _block_id << "  ++++++++++" << std::endl;
        }
        /*
          JUA parketizer code starts
        */
        // Add key to the tx_time tag
        static const pmt::pmt_t time_key = pmt::string_to_symbol("tx_time");
        // Get the time
        struct timeval t;
        gettimeofday(&t, NULL);
        double tx_time = t.tv_sec + t.tv_usec / 1000000.0;
        double min_time_diff = pmt::to_double(_packet_len_tag.value) / _bps; //Max packet len [bit] / bit rate
        // double min_time_diff = (1000 * 8.0) / _bps; //Max packet len [bit] / bit rate
        // Ensure that frames are not overlap each other
//        if((tx_time - _last_tx_time) < (min_time_diff + _t_pretx_interval_s)) {
//          tx_time = _last_tx_time + min_time_diff;
//          if(_develop_mode)
//            std::cout << "t_control ID " << _block_id << " in time packet" << std::endl;
//        }
        //std::cout << "tx time = " << std::fixed << tx_time << std::endl;
        // update the tx_time to the current packet
        _last_tx_time = tx_time;
        if (_antenna_number == 1) {
        uhd::time_spec_t usrp_time = _dev->get_time_now();
        int usrp_time_full = usrp_time.get_full_secs();
        double usrp_time_frac = usrp_time.get_frac_secs();
        double time_sum = usrp_time_full + usrp_time_frac;
        std::cout << "USRP Time: "<< time_sum << '\n';
        std::cout << "Time difference: " << tx_time - time_sum << '\n';
        }
        // question 1: why add 0.05?
        //std::cout << "elapsed time: " << elapsed_time() << '\n';
        uhd::time_spec_t now = uhd::time_spec_t(tx_time-2.77) + uhd::time_spec_t(_t_pretx_interval_s);
        // the value of the tag is a tuple
        const pmt::pmt_t time_value = pmt::make_tuple(
          pmt::from_uint64(now.get_full_secs()),
          pmt::from_double(now.get_frac_secs())
        );
        //pmt::pmt_t sob_key = pmt::string_to_symbol("tx_sob");
        //pmt::pmt_t sob_value;

        add_item_tag(0, _packet_len_tag.offset, time_key, time_value);
        //add_item_tag(0,_packet_len_tag.offset, sob_key, sob_value);
        if(_develop_mode)
        {
          std::cout << "offset: " << _packet_len_tag.offset << std::endl;
        }

        if(_record_on)
        {
          std::ofstream ofs (_file_name_str.c_str(), std::ofstream::app);
          ofs << t.tv_sec << " " << t.tv_usec << "\n";
          ofs.close();
        }
       /*
         JUA parketizer code starts
       */
      }
      // Do <+signal processing+>
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    int
    t_control_tx_cc_impl::process_tags_info(std::vector <tag_t> tags)
    {
      int tag_detected = 0;
      for(int i = 0; i < tags.size(); i++)
      {
        /*
        if(_develop_mode)
        {
          std::cout << "Index of tags: " << i << std::endl;
          std::cout << "Offset: " << tags[i].offset << std::endl;
          std::cout << "Key: " << tags[i].key << std::endl;
          std::cout << "Value: " << tags[i].value << std::endl;
          std::cout << "Srcid: " << tags[i].srcid << std::endl;
        }
        */

          // std::cout << "string comapre: " << pmt::symbol_to_string(tags[i].key) << "packet_len" <<  (pmt::symbol_to_string(tags[i].key) == "packet_len") << std::endl;
        if(pmt::symbol_to_string(tags[i].key) == "packet_len")
        {
          _packet_len_tag = tags[i];
          tag_detected = 1;
          if(_develop_mode)
          {
            std::cout << "++++ t_control ID: " << _block_id << " gets a packet (packet_len tag) to send ";
            if(_develop_mode == 2)
            {
              struct timeval t;
              gettimeofday(&t, NULL);
              double current_time_show = t.tv_sec - double(int(t.tv_sec/10)*10) + t.tv_usec / 1000000.0;
              std::cout << " at time " << current_time_show << " s" << std::endl;
            }
            else
              std::cout << "." << std::endl;
            }
          break;
        }
      }
      return tag_detected;
    }

    void t_control_tx_cc_impl::set_phase(pmt::pmt_t phase_in) {
      if(pmt::is_dict(phase_in)) {
        if (_develop_mode) {
          std::cout << "Phase is is a dict" << '\n';
        }
        //pmt::print(pmt::dict_values(phase_in));
        pmt::pmt_t not_found = pmt::string_to_symbol("mistake");
        if (_antenna_number == 1) {
          if (pmt::dict_has_key(phase_in, pmt::string_to_symbol("phase_key1"))) {
            _phase = pmt::to_double(pmt::dict_ref(phase_in, pmt::string_to_symbol("phase_key1"), not_found));
            if (_develop_mode) {
              std::cout << "dict has the key" << '\n';
            }
          }
          else if (_develop_mode)
            std::cout << "The phase_key1 doesnt exist in the dict." << '\n';
        } else if (_antenna_number == 2) {
          _phase = pmt::to_double(pmt::dict_ref(phase_in, pmt::string_to_symbol("phase_key2"), not_found));
        } else if (_antenna_number == 3) {
          _phase = pmt::to_double(pmt::dict_ref(phase_in, pmt::string_to_symbol("phase_key3"), not_found));
        } else {
          _phase = pmt::to_double(pmt::dict_ref(phase_in, pmt::string_to_symbol("phase_key4"), not_found));
        }
      }
      else {
        std::cout << "phase_in is not a dictionary" << '\n';
      }
    }

    void t_control_tx_cc_impl::shift_the_phase(gr_complex &temp){
      double magn = abs(temp);
      double shifted_arg = _phase + arg(temp);
      int v = rand() % 100000; // to avoid gnuradio crashing
      if (_develop_mode) {
        if (v < 1) {
          std::cout << "input: " << temp << '\n';
          std::cout << "argument in: " << arg(temp) << '\n';
          std::cout << "argument out: " << shifted_arg << '\n';
          std::cout << "cosine in: " << cos(arg(temp)) << '\n';
          std::cout << "cosine out: " << cos(shifted_arg) << '\n';
        }
      }
      temp = std::polar(magn, shifted_arg);
      if (_develop_mode) {
        if (v < 1) {
          std::cout << "shifted output: " << temp << '\n';
        }
      }
    }
  } /* namespace inets */
} /* namespace gr */
