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
#include "PRBS_Gen_impl.h"

namespace gr {
  namespace inets {

    PRBS_Gen::sptr
    PRBS_Gen::make(int develop_mode)
    {
      return gnuradio::get_initial_sptr
        (new PRBS_Gen_impl(develop_mode));
    }

    /*
     * The private constructor
     */
    PRBS_Gen_impl::PRBS_Gen_impl(int develop_mode)
      : gr::sync_block("PRBS_Gen",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(uint8_t)))
    {
    }

    /*
     * Our virtual destructor.
     */
    PRBS_Gen_impl::~PRBS_Gen_impl()
    {
    }

    int
    PRBS_Gen_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      uint8_t *out = (uint8_t *) output_items[0];
      _reference = generate();

      // Do <+signal processing+>
      for (size_t i = 0; i < _reference.size(); i++) {
        out[i] = _reference[i];
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
    std::vector<uint8_t> PRBS_Gen_impl::generate() {
      uint8_t start_state = 0x01;  /* Any nonzero start state will work. */
      uint8_t lfsr = start_state;
      uint8_t bit;
      std::vector<uint8_t> payload;
      payload.push_back(start_state);
      do
      {
        bit  = ((lfsr >> 0) ^ (lfsr >> 1)) & 1; // Polynomial is x^7 + x^6 + 1
        lfsr =  (lfsr >> 1) | (bit << 6);
        payload.push_back(lfsr);
      } while (lfsr != start_state);
      return payload;
      }

  } /* namespace inets */
} /* namespace gr */
