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


#ifndef INCLUDED_INETS_FRAME_COMPARER_H
#define INCLUDED_INETS_FRAME_COMPARER_H

#include <inets/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace inets {

    /*!
     * \brief <+description of block+>
     * \ingroup inets
     *
     */
    class INETS_API frame_comparer : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<frame_comparer> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of inets::frame_comparer.
       *
       * To avoid accidental use of raw pointers, inets::frame_comparer's
       * constructor is in a private implementation
       * class. inets::frame_comparer::make is the public interface for
       * creating new instances.
       */
      static sptr make(int develop_mode, int block_id, int what_to_compare, int single_input);
    };

  } // namespace inets
} // namespace gr

#endif /* INCLUDED_INETS_FRAME_COMPARER_H */
