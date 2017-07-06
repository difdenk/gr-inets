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
#include "mux_impl.h"

namespace gr {
  namespace inets {

    mux::sptr
    mux::make()
    {
      return gnuradio::get_initial_sptr
        (new mux_impl());
    }

    /*
     * The private constructor
     */
    mux_impl::mux_impl()
      : gr::sync_interpolator("mux",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(4, 4, sizeof(gr_complex)), 4)
    {}

    /*
     * Our virtual destructor.
     */
    mux_impl::~mux_impl()
    {
    }

    int
    mux_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out0 = (gr_complex *) output_items[0];
      gr_complex *out1 = (gr_complex *) output_items[1];
      gr_complex *out2 = (gr_complex *) output_items[2];
      gr_complex *out3 = (gr_complex *) output_items[3];

      // Do <+signal processing+>
      for (int i = 0; i < noutput_items; i++) {
        out0[i] = in[i];
        out1[i] = in[i];
        out2[i] = in[i];
        out3[i] = in[i];
      }
      return noutput_items;
    }

  } /* namespace inets */
} /* namespace gr */
