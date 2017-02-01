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

#ifndef INCLUDED_INETS_FRAME_PROBE_IMPL_H
#define INCLUDED_INETS_FRAME_PROBE_IMPL_H

#include <inets/frame_probe.h>

namespace gr {
  namespace inets {

    class frame_probe_impl : public frame_probe
    {
     private:
      int _develop_mode;
      int _block_id;
      int _print_frame;
      void read_info(pmt::pmt_t frame_info);
      void show_detail(pmt::pmt_t frame_info);
      void disp_vec(std::vector<unsigned char> vec);

     public:
      frame_probe_impl(int develop_mode, int block_id, int print_frame);
      ~frame_probe_impl();

    };

  } // namespace inets
} // namespace gr

#endif /* INCLUDED_INETS_FRAME_PROBE_IMPL_H */
