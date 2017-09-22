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
    {
      if(develop_mode)
        std::cout << "develop_mode of Direction mapper is activated." << '\n';
      message_port_register_out(pmt::mp("best_direction_out"));
      message_port_register_in(pmt::mp("beacon_reply_in"));
      set_msg_handler(pmt::mp("beacon_reply_in"), boost::bind(&direction_finder_impl::generate_node_table, this, _1));
    }

    /*
     * Our virtual destructor.
     */
    direction_finder_impl::~direction_finder_impl()
    {
    }

    void direction_finder_impl::generate_node_table(pmt::pmt_t beacon_reply_in) {
      pmt::pmt_t not_found = pmt::from_long(7);
      int received_frame_address = pmt::to_long(pmt::dict_ref(beacon_reply_in, pmt::string_to_symbol("source_address"), not_found));
      int snr = pmt::to_long(pmt::dict_ref(beacon_reply_in, pmt::string_to_symbol("reserved_field_I"), not_found));
      int angle = pmt::to_long(pmt::dict_ref(beacon_reply_in, pmt::string_to_symbol("reserved_field_II"), not_found));
      std::cout << "Incoming node address: " << received_frame_address <<'\n';
      std::cout << "SNR: " << snr <<'\n';
      std::cout << "Direction of the destined node : " << angle <<'\n';
    }

  } /* namespace inets */
} /* namespace gr */
