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
#include "phase_shifter_impl.h"

namespace gr {
  namespace inets {

    phase_shifter::sptr
    phase_shifter::make(int develop_mode, int block_id, int noutput, double phase_shift)
    {
      return gnuradio::get_initial_sptr
        (new phase_shifter_impl(int develop_mode, int block_id int noutput,double phase_shift);
    }

    /*
     * The private constructor
     */
    phase_shifter_impl::phase_shifter_impl(utput, int block_id double phase_shift)
      : gr::sync_interpolator("phase_shifter",
              gr::io_signature::make(1, 1, sizeof(gr_complex),
              gr::io_signature::make(noutput, noutput, sizeof(gr_complex), 4)
              _develop_mode(develop_mode),
              _block_id(block_id),
              _noutput(noutput),
              _phase_shift(phase_shift)
    {}

    /*
     * Our virtual destructor.
     */
    phase_shifter_impl::~phase_shifter_impl()
    {
    }

    int
    phase_shifter_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const <+ITYPE+> *in = (const <+ITYPE+> *) input_items[0];
      <+OTYPE+> *out = (<+OTYPE+> *) output_items[0];

      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace inets */
} /* namespace gr */
