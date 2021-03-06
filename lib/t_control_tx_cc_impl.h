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

#ifndef INCLUDED_INETS_T_CONTROL_TX_CC_IMPL_H
#define INCLUDED_INETS_T_CONTROL_TX_CC_IMPL_H

#include <inets/t_control_tx_cc.h>
#include <uhd/usrp/multi_usrp.hpp>

namespace gr {
  namespace inets {

    class t_control_tx_cc_impl : public t_control_tx_cc
    {
     private:
      int _develop_mode;
      int _block_id;
      int _record_on;
      int _antenna_number;
      std::string _file_name_str;
      std::string _file_name_extension;
      int _name_with_timestamp;
      double _last_tx_time;
      double _bps;
      double _phase;
      double _t_pretx_interval_s;
      double _time_sum;
      double _frequency;
      double _sweep_mode;
      double _start;
      bool _initial_message;
      bool _first;
      bool _directional_mode;
      tag_t _packet_len_tag;
      uhd::usrp::multi_usrp::sptr _dev;


     public:
      t_control_tx_cc_impl(int develop_mode, int block_id, double bps, double t_pretx_interval_s, int record_on, std::string fime_name_extension, int name_with_timestamp, int antenna_number, double frequency, double sweep_mode, bool directional_mode);
      ~t_control_tx_cc_impl();
      int process_tags_info(std::vector <tag_t> tags);
      void set_phase(pmt::pmt_t phase_in);
      void shift_the_phase(gr_complex &temp);

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace inets
} // namespace gr

#endif /* INCLUDED_INETS_T_CONTROL_TX_CC_IMPL_H */
