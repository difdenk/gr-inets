/* -*- c++ -*- */
/* 
 * copyright 2016 <+you or your company+>.
 * 
 * this is free software; you can redistribute it and/or modify
 * it under the terms of the gnu general public license as published by
 * the free software foundation; either version 3, or (at your option)
 * any later version.
 * 
 * this software is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * merchantability or fitness for a particular purpose.  see the
 * gnu general public license for more details.
 * 
 * you should have received a copy of the gnu general public license
 * along with this software; see the file copying.  if not, write to
 * the free software foundation, inc., 51 franklin street,
 * boston, ma 02110-1301, usa.
 */

#ifdef have_config_h
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "decision_cpp_impl.h"

namespace gr {
  namespace inets {

    decision_cpp::sptr
    decision_cpp::make(std::vector<int> develop_mode_list, int decision_index)
    {
      return gnuradio::get_initial_sptr
        (new decision_cpp_impl(develop_mode_list, decision_index));
    }

    /*
     * the private constructor
     */
    decision_cpp_impl::decision_cpp_impl(std::vector<int> develop_mode_list, int decision_index)
      : gr::block("decision_cpp",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
        _develop_mode_list(develop_mode_list),
        _decision_index(decision_index)
    {
      _develop_mode = (std::find(_develop_mode_list.begin(), _develop_mode_list.end(), _decision_index) != _develop_mode_list.end());
      if(_develop_mode)
        std::cout << "develop_mode of the " << _decision_index << "th decision block is activated." << std::endl;
      message_port_register_in(pmt::mp("spark_in"));
      message_port_register_out(pmt::mp("spark_out_t"));
      message_port_register_out(pmt::mp("spark_out_f"));
      set_msg_handler(
        pmt::mp("spark_in"),
        boost::bind(&decision_cpp_impl::decision_making, this, _1)
      );
    }

    /*
     * our virtual destructor.
     */
    decision_cpp_impl::~decision_cpp_impl()
    {
    }

    void decision_cpp_impl::decision_making(pmt::pmt_t msg)
    {
      if(_develop_mode)
      {
        std::cout << "++++++++++++   decision_cpp   ++++++++++++++" << std::endl;
      }
      /*
       * check the received message. misconnecting message port may lead to this error.
       */
      if(pmt::is_bool(msg))
      {
        /*
         * if the received message is ture (in boolean form), then start the cs countdown.
         */
        if(pmt::to_bool(msg))
        {
          if(_develop_mode)
	    std::cout << "True output" << std::endl;
          message_port_pub(pmt::mp("spark_out_t"), pmt::from_bool(true));
        }
        else
        {
          if(_develop_mode)
            std::cout << "False outout" << std::endl;
          message_port_pub(pmt::mp("spark_out_f"), pmt::from_bool(true));
        }
      }
      else
      {
        // not a boolean pmt, most likely a gnuradio connection error
        std::cout << "not a spark signal" << std::endl;
      }
    }

  } /* namespace inets */
} /* namespace gr */

