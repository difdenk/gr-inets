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

#ifndef INCLUDED_INETS_WAIT_CPP_IMPL_H
#define INCLUDED_INETS_WAIT_CPP_IMPL_H

#include <inets/wait_cpp.h>

namespace gr {
  namespace inets {

    class wait_cpp_impl : public wait_cpp
    {
     private:
       int _develop_mode;
       int _my_develop_mode;
       std::vector<int> _develop_mode_list;
       int _system_time_granularity_us;
       float _wait_time; 
       void start_waiting(pmt::pmt_t msg);
       void countdown_sensing();
 
     public:
      wait_cpp_impl(std::vector<int> _develop_mode_list, int system_time_granularity_us);
      ~wait_cpp_impl();
    };

  } // namespace inets
} // namespace gr

#endif /* INCLUDED_INETS_WAIT_CPP_IMPL_H */

