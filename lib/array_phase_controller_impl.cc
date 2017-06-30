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
#include "array_phase_controller_impl.h"
#include <math.h>
#include <string>
#include <iostream>
#include <cmath>
#include <complex>

namespace gr {
  namespace inets {

    array_phase_controller::sptr
    array_phase_controller::make(int develop_mode, int block_id, int noutput, double phase_shift)
    {
      return gnuradio::get_initial_sptr
        (new array_phase_controller_impl(develop_mode, block_id, noutput, phase_shift));
    }

    /*
     * The private constructor
     */
    array_phase_controller_impl::array_phase_controller_impl(int develop_mode, int block_id, int noutput, double phase_shift)
      : gr::block("array_phase_controller",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(noutput, noutput, sizeof(gr_complex))),
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
    array_phase_controller_impl::~array_phase_controller_impl()
    {
    }

    void
    array_phase_controller_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items/4;
    }


    void array_phase_controller_impl::shift_the_phase(gr_complex &temp){
      double magn = abs(temp);
      double shifted_arg = arg(temp) + _phase_shift;
      temp = std::polar(magn, shifted_arg);
    }
    std::vector<tag_t> tags_in;
    //std::vector<tag_t> tags_out0;
    //std::vector<tag_t> tags_out1;
    //std::vector<tag_t> tags_out2;
    //std::vector<tag_t> tags_out3;

    int
    array_phase_controller_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out0 = (gr_complex*) output_items[0];
      gr_complex *out1 = (gr_complex*) output_items[1];
      gr_complex *out2 = (gr_complex*) output_items[2];
      gr_complex *out3 = (gr_complex*) output_items[3];
      set_relative_rate(1);

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

      get_tags_in_range(tags_in, 0, nitems_read(0), nitems_read(0) + noutput_items);

      if(_develop_mode){
        if(!tags_in.empty()){
          //std::cout << "There are tags in the stream." << std::endl;
          for (int i = 0; i < tags_in.size(); i++){
            std::cout << "The tags on the Input Port" << std::endl;
            std::cout << "Index of tags: " << i << std::endl;
            std::cout << "Offset: " << tags_in[i].offset << std::endl;
            std::cout << "Key: " << tags_in[i].key << std::endl;
            std::cout << "Value: " << tags_in[i].value << std::endl;
            std::cout << "Srcid: " << tags_in[i].srcid << std::endl;
            std::cout << std::endl;
          }
        }
        //else
          //std::cout << "This stream has no tags!!" << std::endl;
      }

      //get_tags_in_range(tags_out0, 0, nitems_written(0), nitems_written(0) + noutput_items);
      //get_tags_in_range(tags_out1, 1, nitems_written(1), nitems_written(1) + noutput_items);
      //get_tags_in_range(tags_out2, 2, nitems_written(2), nitems_written(2) + noutput_items);
      //get_tags_in_range(tags_out3, 3, nitems_written(3), nitems_written(3) + noutput_items);

      /*
          std::cout << "The tags on the Output Port 0" << std::endl;
          std::cout << "Index of tags: " << i << std::endl;
          std::cout << "Offset: " << tags_out0[i].offset << std::endl;
          std::cout << "Key: " << tags_out0[i].key << std::endl;
          std::cout << "Value: " << tags_out0[i].value << std::endl;
          std::cout << "Srcid: " << tags_out0[i].srcid << std::endl;
          std::cout << std::endl;
          std::cout << "The tags on the Output Port 1" << std::endl;
          std::cout << "Index of tags: " << i << std::endl;
          std::cout << "Offset: " << tags_out1[i].offset << std::endl;
          std::cout << "Key: " << tags_out1[i].key << std::endl;
          std::cout << "Value: " << tags_out1[i].value << std::endl;
          std::cout << "Srcid: " << tags_out1[i].srcid << std::endl;
          std::cout << std::endl;
          std::cout << "The tags on the Output Port 2" << std::endl;
          std::cout << "Index of tags: " << i << std::endl;
          std::cout << "Offset: " << tags_out2[i].offset << std::endl;
          std::cout << "Key: " << tags_out2[i].key << std::endl;
          std::cout << "Value: " << tags_out2[i].value << std::endl;
          std::cout << "Srcid: " << tags_out2[i].srcid << std::endl;
          std::cout << std::endl;
          std::cout << "The tags on the Output Port 3" << std::endl;
          std::cout << "Index of tags: " << i << std::endl;
          std::cout << "Offset: " << tags_out3[i].offset << std::endl;
          std::cout << "Key: " << tags_out3[i].key << std::endl;
          std::cout << "Value: " << tags_out3[i].value << std::endl;
          std::cout << "Srcid: " << tags_out3[i].srcid << std::endl;
          std::cout << std::endl;*/

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items/4);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }// end of general_work
  } //namespace inets
} /* namespace gr */
