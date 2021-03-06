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

#ifndef INCLUDED_INETS_antenna_array_controller_IMPL_H
#define INCLUDED_INETS_antenna_array_controller_IMPL_H

#include <inets/antenna_array_controller.h>

namespace gr {
  namespace inets {

    class antenna_array_controller_impl : public antenna_array_controller
    {
     private:
      int _develop_mode;
      int _block_id;
      int _noutput;
      double _phase_shift;
      tag_t _packet_len_tag;
      int _count;


     public:
      antenna_array_controller_impl(int develop_mode, int block_id, int noutput, double phase_shift);
      ~antenna_array_controller_impl();
      void shift_the_phase(gr_complex &temp);
      int prepare_output_tag(std::vector<tag_t> &tags_in);

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace inets
} // namespace gr

#endif /* INCLUDED_INETS_antenna_array_controller_IMPL_H */
