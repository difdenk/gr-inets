/* -*- c++ -*- */
/* 
 * Copyright 2016 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_INETS_TX_BUFFER_IMPL_H
#define INCLUDED_INETS_TX_BUFFER_IMPL_H

#include <inets/tx_buffer.h>

namespace gr {
  namespace inets {

    class tx_buffer_impl : public tx_buffer
    {
     private:
      // Nothing to declare in this block.
      int _buffer_size;
      int _policy;
      std::queue<pmt::pmt_t> tx_buff;

     public:
      tx_buffer_impl(int max_buffer_size, int policy);
      ~tx_buffer_impl();

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace inets
} // namespace gr

#endif /* INCLUDED_INETS_TX_BUFFER_IMPL_H */

