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

#ifndef INCLUDED_INETS_DIRECTION_MAPPER_IMPL_H
#define INCLUDED_INETS_DIRECTION_MAPPER_IMPL_H

#include <inets/direction_mapper.h>

namespace gr {
  namespace inets {

    class direction_mapper_impl : public direction_mapper
    {
     private:
       int _develop_mode;
       int _block_id;

     public:
      direction_mapper_impl(int develop_mode, int block_id, double phase_1, double phase_2, double phase_3, double phase_4);
      ~direction_mapper_impl();
      double _phase_1;
      double _phase_2;
      double _phase_3;
      double _phase_4;
      int _indicator;
      int _count;
      std::vector<int> _nodes;
      std::vector<double> _angles;
      pmt::pmt_t _phase_values;
      pmt::pmt_t _phase_key1;
      pmt::pmt_t _phase_key2;
      pmt::pmt_t _phase_key3;
      pmt::pmt_t _phase_key4;
      void accept_frame(pmt::pmt_t trigger);
      void check_destination(pmt::pmt_t frame_in);
    };

  } // namespace inets
} // namespace gr

#endif /* INCLUDED_INETS_DIRECTION_MAPPER_IMPL_H */
