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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "decouple_frame_info_impl.h"

namespace gr {
  namespace inets {

    decouple_frame_info::sptr
    decouple_frame_info::make(std::vector<int> develop_mode_list)
    {
      return gnuradio::get_initial_sptr
        (new decouple_frame_info_impl(develop_mode_list));
    }

    /*
     * The private constructor
     */
    decouple_frame_info_impl::decouple_frame_info_impl(std::vector<int> develop_mode_list)
      : gr::block("decouple_frame_info",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
        _my_develop_mode(13),
        _develop_mode_list(develop_mode_list)
    { 
      _develop_mode = (std::find(_develop_mode_list.begin(), _develop_mode_list.end(), _my_develop_mode) != _develop_mode_list.end());
      if(_develop_mode)
        std::cout << "develop_mode of decouple_frame_info is activated." << std::endl;
      message_port_register_in(pmt::mp("frame_cluster_in"));
      message_port_register_out(pmt::mp("frame_pmt_out"));
      message_port_register_out(pmt::mp("frame_info_out"));
      set_msg_handler(pmt::mp("frame_cluster_in"), boost::bind(&decouple_frame_info_impl::decoupling, this, _1 ));
    }

    /*
     * Our virtual destructor.
     */
    decouple_frame_info_impl::~decouple_frame_info_impl()
    {
    }

    void decouple_frame_info_impl::decoupling(pmt::pmt_t frame_cluster)
    {
      pmt::pmt_t not_found;
      if(pmt::is_dict(frame_cluster))
      {
        if(pmt::dict_has_key(frame_cluster, pmt::string_to_symbol("frame_pmt")))
        { 
          pmt::pmt_t frame_pmt = pmt::dict_ref(frame_cluster, pmt::string_to_symbol("frame_pmt"), not_found);
          pmt::pmt_t frame_info = pmt::dict_delete(frame_cluster, pmt::string_to_symbol("frame_pmt"));
          message_port_pub(pmt::mp("frame_pmt_out"), frame_pmt);
          message_port_pub(pmt::mp("frame_info_out"), frame_info);
        }
        else
          std::cout << "Warning: frame_cluster does not have payload. Please check your connection." << std::endl;
      }
      else
        std::cout << "Warning: wrong pmt type. Please check your connection." << std::endl;
    }

  } /* namespace inets */
} /* namespace gr */
