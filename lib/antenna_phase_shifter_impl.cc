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
#include "antenna_phase_shifter_impl.h"
#include <math.h>
#include <string>
#include <iostream>
#include <cmath>
#include <complex>

namespace gr {
  namespace inets {

    antenna_phase_shifter::sptr
    antenna_phase_shifter::make(int develop_mode, int block_id, int noutput, double phase_shift)
    {
      return gnuradio::get_initial_sptr
        (new antenna_phase_shifter_impl(develop_mode, block_id, noutput, phase_shift));
    }

    /*
     * The private constructor
     */
    antenna_phase_shifter_impl::antenna_phase_shifter_impl(int develop_mode, int block_id, int noutput, double phase_shift)
      : gr::sync_interpolator("antenna_phase_shifter",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(noutput, noutput, sizeof(gr_complex)), 4),
              _develop_mode(develop_mode),
              _block_id(block_id),
              _noutput(noutput),
              _phase_shift(phase_shift)
    {
      if(_develop_mode)
      std::cout << "develop mode of array_phase_controller ID: " << _block_id << "is activated" << std::endl;
    }

    /*
     * Our virtual destructor.
     */
    antenna_phase_shifter_impl::~antenna_phase_shifter_impl()
    {
    }

    void antenna_phase_shifter_impl::shift_the_phase(gr_complex &temp){
      double magn = abs(temp);
      double shifted_arg = arg(temp) + _phase_shift;
      temp = std::polar(magn, shifted_arg);
    }
    int antenna_phase_shifter_impl::work(int noutput_items,
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
      std::vector<tag_t> tags_in;
      get_tags_in_range(tags_in, 0, nitems_read(0), nitems_read(0) + noutput_items);
      if (_develop_mode) {
        if(!tags_in.empty()){
          std::cout << "There are tags on the stream !" << '\n';
        }
        else
        std::cout << "There are NO tags on this stream !" << '\n';
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace inets */
} /* namespace gr */
