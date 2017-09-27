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
#include "direction_finder_impl.h"
#include <vector>
#include <algorithm>
#include <iterator>

namespace gr {
  namespace inets {

    direction_finder::sptr
    direction_finder::make(int develop_mode, double update_interval, double timeout_value, int destination_address)
    {
      return gnuradio::get_initial_sptr
        (new direction_finder_impl(develop_mode, update_interval, timeout_value, destination_address));
    }

    /*
     * The private constructor
     */
    direction_finder_impl::direction_finder_impl(int develop_mode, double update_interval, double timeout_value, int destination_address)
      : gr::block("direction_finder",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
              _develop_mode(develop_mode),
              _update_interval(update_interval),
              _timeout_value(timeout_value),
              _destination_address(destination_address)
              //_virgin(true)
    {
      if(develop_mode)
        std::cout << "develop_mode of Direction mapper is activated." << '\n';
      message_port_register_out(pmt::mp("best_direction_out"));
      message_port_register_in(pmt::mp("beacon_reply_in"));
      message_port_register_in(pmt::mp("sweep_done_in"));
      set_msg_handler(pmt::mp("beacon_reply_in"), boost::bind(&direction_finder_impl::generate_node_table, this, _1));
      set_msg_handler(pmt::mp("sweep_done_in"), boost::bind(&direction_finder_impl::sweep_done, this, _1));
    }

    /*
     * Our virtual destructor.
     */
    direction_finder_impl::~direction_finder_impl()
    {
    }

    void direction_finder_impl::generate_node_table(pmt::pmt_t beacon_reply_in) {
      _virgin = 7;
      int update_interval = _update_interval;
      int timeout_value = _timeout_value;
      pmt::pmt_t not_found = pmt::from_long(7);
      int received_frame_address = pmt::to_long(pmt::dict_ref(beacon_reply_in, pmt::string_to_symbol("source_address"), not_found));
      int snr = pmt::to_long(pmt::dict_ref(beacon_reply_in, pmt::string_to_symbol("reserved_field_I"), not_found));
      int angle = pmt::to_long(pmt::dict_ref(beacon_reply_in, pmt::string_to_symbol("reserved_field_II"), not_found));
      if (_develop_mode) {
        std::cout << "Incoming node address: " << received_frame_address <<'\n';
        std::cout << "SNR: " << snr <<'\n';
        std::cout << "Direction of the destined node : " << angle <<'\n';
      }
      if (_destination_address = received_frame_address) {
        snr_values.push_back(snr);
        angle_values.push_back(angle);
        if (snr_values.size()%update_interval == 0) {
          int best_direction = find_best_direction();
          _best_direction = pmt::from_long(best_direction);
          if (_develop_mode) {
            std::cout << "best_direction:" << best_direction << '\n';
          }
        }
        if (snr_values.size() >= timeout_value) {
          snr_values.pop_back();
          angle_values.pop_back();
        }
      }
    }

    int direction_finder_impl::find_best_direction(){
      _biggest = std::max_element(snr_values.begin(), snr_values.end());
      int snr_max = *_biggest;
      if (_develop_mode) {
        std::cout << "SNR Max: "<< snr_max << '\n';
      }
      int index = std::distance(snr_values.begin(), _biggest);
      int corresponding_angle = angle_values[index];
      return corresponding_angle;
    }

    void direction_finder_impl::sweep_done(pmt::pmt_t sweep_done){
      if (_virgin = 7) {
        message_port_pub(pmt::mp("best_direction_out"), _best_direction);
        std::cout << "THE DESTINATION NODE IS AT ANGLE "<< _best_direction << '\n';
        std::cout << "TRANSMITTER IS LOCKED TO ANGLE " << _best_direction << '\n';
      }
      else
      {
        std::cout << "SWEEP DONE BUT NO BEACON REPLY RECEIVED!" << '\n';
        std::cout << "DIRECTING THE ANTTENNA TO BROADSIDE DIRECTION (0 DEGREES)" << '\n';
        _best_direction = pmt::from_long(0);
      }  
    }

  } /* namespace inets */
} /* namespace gr */
