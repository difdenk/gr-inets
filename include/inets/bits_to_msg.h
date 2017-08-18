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


#ifndef INCLUDED_INETS_BITS_TO_MSG_H
#define INCLUDED_INETS_BITS_TO_MSG_H

#include <inets/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace inets {

    /*!
     * \brief <+description of block+>
     * \ingroup inets
     *
     */
    class INETS_API bits_to_msg : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<bits_to_msg> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of inets::bits_to_msg.
       *
       * To avoid accidental use of raw pointers, inets::bits_to_msg's
       * constructor is in a private implementation
       * class. inets::bits_to_msg::make is the public interface for
       * creating new instances.
       */
      static sptr make(int develop_mode, int type, double placeholder0, double placeholder1);
    };

  } // namespace inets
} // namespace gr

#endif /* INCLUDED_INETS_BITS_TO_MSG_H */

