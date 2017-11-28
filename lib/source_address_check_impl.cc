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
#include "source_address_check_impl.h"

namespace gr {
  namespace inets {

    source_address_check::sptr
    source_address_check::make(int develop_mode, int source_address, double reserved_argument)
    {
      return gnuradio::get_initial_sptr
        (new source_address_check_impl(develop_mode, source_address, reserved_argument));
    }

    /*
     * The private constructor
     */
    source_address_check_impl::source_address_check_impl(int develop_mode, int source_address, double reserved_argument)
      : gr::block("source_address_check",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
              _develop_mode(develop_mode),
              _source_address(source_address),
              _reserved_argument(reserved_argument)
    {
      message_port_register_in(pmt::mp("frame_info_in"));
      message_port_register_out(pmt::mp("frame_info_out"));
      set_msg_handler(pmt::mp("frame_info_in"), boost::bind(&source_address_check_impl::selector, this, _1 ));
    }

    /*
     * Our virtual destructor.
     */
    source_address_check_impl::~source_address_check_impl()
    {
    }

    void source_address_check_impl::selector(pmt::pmt_t info) {
      pmt::pmt_t not_found = pmt::from_long(7);
      int source_address = pmt::to_long(pmt::dict_ref(info, pmt::string_to_symbol("source_address"), not_found));
      if (_source_address == source_address) {
        message_port_pub(pmt::mp("frame_info_out"), info);
        if (_develop_mode) {
          std::cout << "Correct source address !" << '\n';
        }
      }
      else
        if (_develop_mode) {
          std::cout << "Wrong source address !" << '\n';
          std::cout << "Frame is dropped." << '\n';
        }

    }

  } /* namespace inets */
} /* namespace gr */
