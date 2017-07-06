/* -*- c++ -*- */
/*
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
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

#include <gnuradio/io_signature.h>
#include "antenna_array_controller_impl.h"
#include <math.h>
#include <string>
#include <iostream>
#include <cmath>
#include <complex>
#include <uhd/types/time_spec.hpp>
#include <sys/time.h>
#include <ctime>
#include <fstream>


namespace gr {
  namespace inets {

    antenna_array_controller::sptr
    antenna_array_controller::make(int develop_mode, int block_id, int noutput, double phase_shift, double bps, double t_pretx_interval_s, int record_on, std::string file_name_extension, int name_with_timestamp)
    {
      return gnuradio::get_initial_sptr
        (new antenna_array_controller_impl(develop_mode, block_id, noutput, phase_shift, bps, t_pretx_interval_s, record_on, file_name_extension, name_with_timestamp));
    }

    /*
     * The private constructor
     */
    antenna_array_controller_impl::antenna_array_controller_impl(int develop_mode, int block_id, int noutput, double phase_shift, double bps, double t_pretx_interval_s, int record_on, std::string file_name_extension, int name_with_timestamp)
      : gr::sync_interpolator("antenna_array_controller",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(noutput, noutput, sizeof(gr_complex)), 4),
              _develop_mode(develop_mode),
              _block_id(block_id),
              _noutput(noutput),
              _phase_shift(phase_shift),
              _last_tx_time(0),
              _t_pretx_interval_s(t_pretx_interval_s),
              _file_name_extension(file_name_extension),
              _name_with_timestamp(name_with_timestamp),
              _record_on(record_on),
              _bps(bps)

    {
      if(_develop_mode)
      std::cout << "Develop mode is activated" << '\n';
    }

    /*
     * Our virtual destructor.
     */
    antenna_array_controller_impl::~antenna_array_controller_impl()
    {
    }

    void antenna_array_controller_impl::shift_the_phase(gr_complex &temp){
      double magn = abs(temp);
      double shifted_arg = arg(temp) + _phase_shift;
      temp = std::polar(magn, shifted_arg);
    }

    int
    antenna_array_controller_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out0 = (gr_complex*) output_items[0];
      gr_complex *out1 = (gr_complex*) output_items[1];
      gr_complex *out2 = (gr_complex*) output_items[2];
      gr_complex *out3 = (gr_complex*) output_items[3];

      // Do <+signal processing+>
      //Shift the phases of the signal for each port seperately
      for (int i=0 ; i < noutput_items ; i++){
        out0[i] = in[i];
        if(_develop_mode){
          //std::cout << "out0 = " << *out0 << std::endl;
        }
      }
      for (int i=0 ; i < noutput_items ; i++){
        if(_noutput < 2)
        break;
        gr_complex temp = in[i];
        shift_the_phase(temp);
        out1[i] = temp;
        if(_develop_mode){
          //std::cout << "out1 = " << *out1 << std::endl;
        }
      }
      for (int i=0 ; i < noutput_items ; i++){
        if(_noutput < 3)
        break;
        gr_complex temp = out1[i];
        shift_the_phase(temp);
        out2[i] = temp;
        if(_develop_mode){
          //std::cout << "out2 = " << *out2 << std::endl;
        }
      }
      for (int i=0 ; i < noutput_items ; i++){
        if(_noutput < 4)
        break;
        gr_complex temp = out2[i];
        shift_the_phase(temp);
        out3[i] = temp;
        if(_develop_mode){
          //std::cout << "out3 = " << *out3 << std::endl;
        }
      }
      //set_tag_propagation_policy(TPP_DONT);
      std::vector<tag_t> tags_in;
      get_tags_in_range(tags_in, 0, nitems_read(0), nitems_read(0) + noutput_items, pmt::string_to_symbol("packet_len"));
      for (int i = 0; i < tags_in.size(); i++) {
        add_item_tag(0, tags_in[i].offset, tags_in[i].key, tags_in[i].value);
        add_item_tag(1, tags_in[i].offset, tags_in[i].key, tags_in[i].value);
        add_item_tag(2, tags_in[i].offset, tags_in[i].key, tags_in[i].value);
        add_item_tag(3, tags_in[i].offset, tags_in[i].key, tags_in[i].value);
      }
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
      // question 1: why add 0.05?
      uhd::time_spec_t now = uhd::time_spec_t(tx_time) + uhd::time_spec_t(_t_pretx_interval_s);
      // the value of the tag is a tuple
      const pmt::pmt_t time_value = pmt::make_tuple(
        pmt::from_uint64(now.get_full_secs()),
        pmt::from_double(now.get_frac_secs())
      );

      add_item_tag(0, _packet_len_tag.offset, time_key, time_value);

      if (_develop_mode) {
        if (!tags_in.empty()) {
          std::cout << "There are " << tags_in.size() << " Tags on the stream !" << '\n';
          std::cout << "The tags on the Input Port" << '\n';
          std::cout << "Offset: " << tags_in[0].offset << std::endl;
          std::cout << "Key: " << tags_in[0].key << std::endl;
          std::cout << "Value: " << tags_in[0].value << std::endl;
          std::cout << "Srcid: " << tags_in[0].srcid << std::endl;
          std::cout << '\n';
        } else {
          std::cout << "There are NO Tags on the stream !" << '\n';
        }
      }
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace inets */
} /* namespace gr */
