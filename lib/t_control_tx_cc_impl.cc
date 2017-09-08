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
#include <complex>
#include <fstream>
#include <string>
#include <ctime>
#include <gnuradio/io_signature.h>
#include <math.h>
#include "t_control_tx_cc_impl.h"
#include <uhd/types/time_spec.hpp>
#include <sys/time.h>
#include <stdlib.h>
#include <uhd/usrp/multi_usrp.hpp>
#define _PI 3.14159265359
#define Imag std::complex<double>(0, 1)
#define Speed_of_Light 299792458

namespace gr {
  namespace inets {

    t_control_tx_cc::sptr
    t_control_tx_cc::make(int develop_mode, int block_id, double bps, double t_pretx_interval_s, int record_on, std::string file_name_extension, int name_with_timestamp, int antenna_number, double frequency, double sweep_mode, bool directional_mode)
    {
      return gnuradio::get_initial_sptr
        (new t_control_tx_cc_impl(develop_mode, block_id, bps, t_pretx_interval_s, record_on, file_name_extension, name_with_timestamp, antenna_number, frequency, sweep_mode, directional_mode));
    }

    /*
     * The private constructor
     */
    t_control_tx_cc_impl::t_control_tx_cc_impl(int develop_mode, int block_id, double bps, double t_pretx_interval_s, int record_on, std::string file_name_extension, int name_with_timestamp, int antenna_number, double frequency, double sweep_mode, bool directional_mode)
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
        _phase((0*_PI)/180),
        _bps(bps),
        _antenna_number(antenna_number),
        _frequency(frequency),
        _sweep_mode(sweep_mode),
        _start(0),
        _initial_message(1),
        _first(true),
        _directional_mode(directional_mode)
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
      if (_sweep_mode) {
        std::cout << "Sweep Mode is activated !" << '\n';
      }
      message_port_register_in(pmt::mp("phase_in"));
      set_msg_handler(pmt::mp("phase_in"), boost::bind(&t_control_tx_cc_impl::set_phase, this, _1));
      struct timeval ti;
      gettimeofday(&ti, NULL);
      double pc_clock = ti.tv_sec + ti.tv_usec/1000000.0;
      if (antenna_number == 1 && _directional_mode) {
        uhd::device_addr_t dev_addr;
        dev_addr["addr0"] = "192.168.10.2";
        dev_addr["addr1"] = "192.168.10.3";
        _dev = uhd::usrp::multi_usrp::make(dev_addr);
        const uhd::time_spec_t last_pps_time = _dev->get_time_last_pps();
        while (last_pps_time == _dev->get_time_last_pps()){
        //sleep 100 milliseconds (give or take)
        }
        // This command will be processed fairly soon after the last PPS edge:
        _dev->set_time_next_pps(uhd::time_spec_t(pc_clock));
        //_dev->set_time_unknown_pps(uhd::time_spec_t(pc_clock));
        //we will tune the frontends in 100ms from now
        uhd::time_spec_t cmd_time = _dev->get_time_now() + uhd::time_spec_t(0.1);
        //sets command time on all devices
        //the next commands are all timed
        _dev->set_command_time(cmd_time);
        //tune channel 0 and channel 1
        _dev->set_tx_freq(_frequency, 0); // Channel 0
        _dev->set_tx_freq(_frequency, 1); // Channel 1
        _dev->set_tx_freq(_frequency, 2); // Channel 2
        _dev->set_tx_freq(_frequency, 3); // Channel 3
        //end timed commands
        _dev->clear_command_time();
        uhd::time_spec_t usrp_time = _dev->get_time_now();
        int usrp_time_full = usrp_time.get_full_secs();
        double usrp_time_frac = usrp_time.get_frac_secs();
        double _time_sum = usrp_time_full + usrp_time_frac;
        if (_develop_mode) {
          std::cout << "USRP time: " << _time_sum << '\n';
          std::cout << "PC time: " << pc_clock  << '\n';
        }
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

      for(int i = 0; i < noutput_items; i++)
      {
        if (_directional_mode) {
          gr_complex temp = in[i];
          shift_the_phase(temp);
          out[i] = temp;
        } else {
          out[i] = in[i];
        }
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
//       if((tx_time - _last_tx_time) < (min_time_diff + _t_pretx_interval_s)) {
//          tx_time = _last_tx_time + min_time_diff;
//          if(_develop_mode)
//            std::cout << "t_control ID " << _block_id << " in time packet" << std::endl;
//        }
        //std::cout << "tx time = " << std::fixed << tx_time << std::endl;
        // update the tx_time to the current packet
        _last_tx_time = tx_time;
        if (_antenna_number == 1 && _directional_mode) {
          uhd::time_spec_t usrp_time = _dev->get_time_now();
          int usrp_time_full = usrp_time.get_full_secs();
          double usrp_time_frac = usrp_time.get_frac_secs();
          double time_sum = usrp_time_full + usrp_time_frac;
          if (_develop_mode) {
            std::cout << "USRP Time: "<< time_sum << '\n';
            std::cout << "Time difference: " << tx_time - time_sum << '\n';
          }
      }

        // question 1: why add 0.05?
        //std::cout << "elapsed time: " << elapsed_time() << '\n';


        uhd::time_spec_t now = uhd::time_spec_t(tx_time-3.9);

        // the value of the tag is a tuple
        const pmt::pmt_t time_value = pmt::make_tuple(
          pmt::from_uint64(now.get_full_secs()),
          pmt::from_double(now.get_frac_secs())
        );
        //pmt::print(time_value);
        if (_directional_mode) {
          add_item_tag(0, _packet_len_tag.offset, time_key, time_value);
        }
        if(_record_on)
        {
          std::ofstream ofs (_file_name_str.c_str(), std::ofstream::app);
          ofs << t.tv_sec << " " << t.tv_usec << "\n";
          ofs.close();
        }
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
        if (_initial_message) {
          _initial_message = 0;
          _start = clock();
        }
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
      int v = rand() % 100000; // to avoid gnuradio crashing from develop_mode
      if (_develop_mode) {
        if (v < 1) {
          std::cout << "input in rectangular form: " << temp << '\n';
          std::cout << "input in polar form: " << sqrt(real(temp)*real(temp)+imag(temp)*imag(temp)) << ", ";
          std::cout << std::fmod(atan2(imag(temp),real(temp)), 2*_PI) * (180/_PI) << '\n';
        }
      }
      double magn = real(temp);
      double arg = imag(temp);
      std::complex<double> temp1(magn, arg);
      std::complex<double> weight;
      double sweep_speed = _sweep_mode;
      double calibration1 = 0.257; // 81 degrees phase offset
      double calibration2 = 0.0523; // 20 degrees offset
      double calibration3 = 0.107; // 42 degrees phase offset
      if (_phase >= 0) { // check if the desired direction is to the left or right
        if(_sweep_mode && !_initial_message) { // to enter sweeping mode wait until the first message arrives
          double sweep = (((clock() - _start)/CLOCKS_PER_SEC)*_PI/180)*sweep_speed;
          if (sweep < _phase) { // sweep until the the phase value provided by the user
            if (_antenna_number == 1) {
              weight = std::exp(0.085 * (0) * sin(sweep) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            }
            else if (_antenna_number == 2) {
              weight = std::exp(0.085 * (1) * sin(sweep) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration1) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            }
            else if (_antenna_number == 3) {
              weight = std::exp(0.085 * (2) * sin(sweep) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration2) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            }
            else
              weight = std::exp(0.085 * (3) * sin(sweep) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration3) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            //weight = std::exp(0.085 * (_antenna_number - 1) * sin(sweep) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            if (v < 1)
            std::cout << "Scanning Angle: " << sweep*180/_PI << '\n';
          }
          else { // stop sweeping at the limit
            //weight = std::exp(0.085 * (_antenna_number - 1) * sin(_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            if (_antenna_number == 1) {
              weight = std::exp(0.085 * (0) * sin(_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            }
            else if (_antenna_number == 2) {
              weight = std::exp(0.085 * (1) * sin(_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration1) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            }
            else if (_antenna_number == 3) {
              weight = std::exp(0.085 * (2) * sin(_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration2) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            }
            else
              weight = std::exp(0.085 * (3) * sin(_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration3) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            if (_first == true && _antenna_number == 1) {
              std::cout << "Scanning Angle: " << _phase*180/_PI << '\n';
              _first = false;
            }
          }
        }
        else { // sweeping mode is disabled
          //weight = std::exp(0.085 * (_antenna_number - 1) * sin(_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag);
          if (_antenna_number == 1) {
            weight = std::exp(0.085 * (0) * sin(_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag);
          }
          else if (_antenna_number == 2) {
            weight = std::exp(0.085 * (1) * sin(_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration1) * (2*_PI*_frequency/Speed_of_Light) * Imag);
          }
          else if (_antenna_number == 3) {
            weight = std::exp(0.085 * (2) * sin(_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration2) * (2*_PI*_frequency/Speed_of_Light) * Imag);
          }
          else
            weight = std::exp(0.085 * (3) * sin(_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration3) * (2*_PI*_frequency/Speed_of_Light) * Imag);
        }
      }
      else { // it is right
        if(_sweep_mode && !_initial_message) {
          double sweep = ((clock() - _start)/CLOCKS_PER_SEC)*_PI/180*sweep_speed;
          if (-sweep > _phase ) {
            //weight = std::exp(0.085 * (4 - _antenna_number) * sin(-sweep) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            if (_antenna_number == 1) {
              weight = std::exp(0.085 * (3) * sin(-sweep) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            }
            else if (_antenna_number == 2) {
              weight = std::exp(0.085 * (2) * sin(-sweep) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration1) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            }
            else if (_antenna_number == 3) {
              weight = std::exp(0.085 * (1) * sin(-sweep) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration2) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            }
            else
              weight = std::exp(0.085 * (0) * sin(-sweep) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration3) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            if (v < 1)
            std::cout << "Scanning Angle: " << -sweep*180/_PI << '\n';
          }
          else {
            //weight = std::exp(0.085 * (4 - _antenna_number) * sin(-_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            if (_antenna_number == 1) {
              weight = std::exp(0.085 * (3) * sin(-_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            }
            else if (_antenna_number == 2) {
              weight = std::exp(0.085 * (2) * sin(-_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration1) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            }
            else if (_antenna_number == 3) {
              weight = std::exp(0.085 * (1) * sin(-_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration2) * (2*_PI*_frequency/Speed_of_Light) * Imag);
            }
            else
              weight = std::exp(0.085 * (0) * sin(-_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration3) * (2*_PI*_frequency/Speed_of_Light) * Imag);
          }
          if (_first == true && _antenna_number == 1) {
              std::cout << "Scanning Angle: " << _phase*180/_PI << '\n';
              _first = false;
          }
        }
        else { //sweeping is disabled
          if (_antenna_number == 1) {
            weight = std::exp(0.085 * (3) * sin(-_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag);
          }
          else if (_antenna_number == 2) {
            weight = std::exp(0.085 * (2) * sin(-_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration1) * (2*_PI*_frequency/Speed_of_Light) * Imag);
          }
          else if (_antenna_number == 3) {
            weight = std::exp(0.085 * (1) * sin(-_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration2) * (2*_PI*_frequency/Speed_of_Light) * Imag);
          }
          else
            weight = std::exp(0.085 * (0) * sin(-_phase) * (2*_PI*_frequency/Speed_of_Light) * Imag) * std::exp(0.085 * sin(calibration3) * (2*_PI*_frequency/Speed_of_Light) * Imag);
        }
      }
      temp = temp1 * weight; // change the block output according to the given weights
      if (_develop_mode) {
        if (v < 1) {
          std::cout << "the weight of antenna " << weight << '\n';
          std::cout << "weight in polar form: " << sqrt(std::real(weight)*std::real(weight)+std::imag(weight)*std::imag(weight)) << ", ";
          std::cout << std::fmod(atan2(imag(weight),real(weight)), 2*_PI) * (180/_PI) << '\n';
          std::cout << "output in rectangular form: " << temp << '\n';
          std::cout << "output in polar form: " << sqrt(real(temp)*real(temp)+imag(temp)*imag(temp)) << ", ";
          std::cout << std::fmod(atan2(imag(temp),real(temp)), 2*_PI) * (180/_PI) << '\n';
        }
      }
    }
  } /* namespace inets */
} /* namespace gr */
