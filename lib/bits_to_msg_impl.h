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

#ifndef INCLUDED_INETS_BITS_TO_MSG_IMPL_H
#define INCLUDED_INETS_BITS_TO_MSG_IMPL_H

#include <inets/bits_to_msg.h>

namespace gr {
  namespace inets {

    class bits_to_msg_impl : public bits_to_msg
    {
     private:
      int _develop_mode;
      std::vector<unsigned char> _payload;
      int _payload_length;
      void trigger(pmt::pmt_t trigger); 

     public:
      bits_to_msg_impl(int develop_mode, int type, double placeholder0, double placeholder1);
      ~bits_to_msg_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace inets
} // namespace gr

#endif /* INCLUDED_INETS_BITS_TO_MSG_IMPL_H */
