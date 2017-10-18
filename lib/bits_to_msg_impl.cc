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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "bits_to_msg_impl.h"

namespace gr {
  namespace inets {

    bits_to_msg::sptr
    bits_to_msg::make(int develop_mode, int type, double placeholder0, double placeholder1)
    {
      return gnuradio::get_initial_sptr
        (new bits_to_msg_impl(develop_mode, type, placeholder0, placeholder1));
    }

    /*
     * The private constructor
     */
    bits_to_msg_impl::bits_to_msg_impl(int develop_mode, int type, double placeholder0, double placeholder1)
      : gr::sync_block("bits_to_msg",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(0, 0, 0))
    {
      message_port_register_in(pmt::mp("trigger"));
      set_msg_handler(pmt::mp("trigger"), boost::bind(&bits_to_msg_impl::trigger, this, _1));
      message_port_register_out(pmt::mp("output"));
    }

    /*
     * Our virtual destructor.
     */
    bits_to_msg_impl::~bits_to_msg_impl()
    {
    }

    int
    bits_to_msg_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];

      for(int i = 0; i < noutput_items; i++)
      {
        unsigned char input_seq = in[i];
        _payload.push_back(input_seq);
      }
      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    void bits_to_msg_impl::trigger (pmt::pmt_t trig)
    {
      message_port_pub(pmt::mp("output"), pmt::cons(pmt::make_dict(), pmt::init_u8vector(_payload.size(), _payload)));
    }

  } /* namespace inets */
} /* namespace gr */
