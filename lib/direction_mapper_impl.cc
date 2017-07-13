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
#include "direction_mapper_impl.h"

namespace gr {
  namespace inets {

    direction_mapper::sptr
    direction_mapper::make(int develop_mode, int block_id, double phase_1, double phase_2, double phase_3, double phase_4)
    {
      return gnuradio::get_initial_sptr
        (new direction_mapper_impl(develop_mode, block_id, phase_1, phase_2, phase_3, phase_4));
    }

    /*
     * The private constructor
     */
    direction_mapper_impl::direction_mapper_impl(int develop_mode, int block_id, double phase_1, double phase_2, double phase_3, double phase_4)
      : gr::block("direction_mapper",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
              _develop_mode(develop_mode),
              _block_id(block_id),
              _phase_1(phase_1),
              _phase_2(phase_2),
              _phase_3(phase_3),
              _phase_4(phase_4)
    {
      if(develop_mode)
      std::cout << "develop_mode of Direction mapper is activated." << '\n';
      message_port_register_out(pmt::mp("phase_out"));
      message_port_register_in(pmt::mp("frame_in"));
      set_msg_handler(pmt::mp("frame_in"), boost::bind(&direction_mapper_impl::accept_frame, this, _1));
      pmt::pmt_t phase_key1 = pmt::string_to_symbol("phase_key1");
      pmt::pmt_t phase_value1 = pmt::from_double(_phase_1);
      pmt::pmt_t phase_key2 = pmt::string_to_symbol("phase_key2");
      pmt::pmt_t phase_value2 = pmt::from_double(_phase_2);
      pmt::pmt_t phase_key3 = pmt::string_to_symbol("phase_key3");
      pmt::pmt_t phase_value3 = pmt::from_double(_phase_3);
      pmt::pmt_t phase_key4 = pmt::string_to_symbol("phase_key4");
      pmt::pmt_t phase_value4 = pmt::from_double(_phase_4);
      _phase_values = pmt::make_dict();
      _phase_values = pmt::dict_add(_phase_values, phase_key1, phase_value1);
      _phase_values = pmt::dict_add(_phase_values, phase_key2, phase_value2);
      _phase_values = pmt::dict_add(_phase_values, phase_key3, phase_value3);
      _phase_values = pmt::dict_add(_phase_values, phase_key4, phase_value4);
    }

    /*
     * Our virtual destructor.
     */
    direction_mapper_impl::~direction_mapper_impl()
    {
    }

    void direction_mapper_impl::accept_frame(pmt::pmt_t trigger){
      message_port_pub(pmt::mp("phase_out"), _phase_values);
    }


  } /* namespace inets */
} /* namespace gr */
