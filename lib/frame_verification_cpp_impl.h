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

#ifndef INCLUDED_INETS_FRAME_VERIFICATION_CPP_IMPL_H
#define INCLUDED_INETS_FRAME_VERIFICATION_CPP_IMPL_H

#include <inets/frame_verification_cpp.h>
#include <gnuradio/digital/crc32.h>
#include <boost/crc.hpp>

namespace gr {
  namespace inets {

    class frame_verification_cpp_impl : public frame_verification_cpp
    {
     private:
      int _develop_mode;
      int _block_id;
      int _frame_type; 
      boost::crc_optimal<32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, true> _crc_impl; 
      pmt::pmt_t crc32_bb_calc(pmt::pmt_t msg);
      void check_frame(pmt::pmt_t frame_info);
      void check_frame_v0(pmt::pmt_t rx_frame);
     public:
      frame_verification_cpp_impl(int develop_mode, int block_id);
      ~frame_verification_cpp_impl();

    };

  } // namespace inets
} // namespace gr

#endif /* INCLUDED_INETS_FRAME_VERIFICATION_CPP_IMPL_H */

