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


#ifndef INCLUDED_INETS_ANTENNA_PHASE_SHIFTER_H
#define INCLUDED_INETS_ANTENNA_PHASE_SHIFTER_H

#include <inets/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
  namespace inets {

    /*!
     * \brief <+description of block+>
     * \ingroup inets
     *
     */
    class INETS_API antenna_phase_shifter : virtual public gr::sync_interpolator
    {
     public:
      typedef boost::shared_ptr<antenna_phase_shifter> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of inets::antenna_phase_shifter.
       *
       * To avoid accidental use of raw pointers, inets::antenna_phase_shifter's
       * constructor is in a private implementation
       * class. inets::antenna_phase_shifter::make is the public interface for
       * creating new instances.
       */
      static sptr make(int develop_mode, int block_id, int noutput, double phase_shift);
    };

  } // namespace inets
} // namespace gr

#endif /* INCLUDED_INETS_ANTENNA_PHASE_SHIFTER_H */

