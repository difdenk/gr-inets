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

#ifndef INCLUDED_INETS_DIRECTION_FINDER_IMPL_H
#define INCLUDED_INETS_DIRECTION_FINDER_IMPL_H

#include <inets/direction_finder.h>
#include <vector>
#include <algorithm>

namespace gr {
  namespace inets {

    class direction_finder_impl : public direction_finder
    {
     private:
       int _develop_mode;
       double _update_interval;
       double _timeout_value;
       int _destination_address;
       int _virgin;
       bool _sweep_done;
       class radio {
         private:
           int node_number;
           std::vector<double> snr_radio;
           std::vector<double> angle_radio;
         public:
           radio();
           ~radio();
           void insert_snr(double element);
           void insert_angle(double element);
           std::vector<double>::iterator find_max_snr();
           double find_coresponding_angle(std::vector<double>::iterator it);
           void set_node_number(int number);
           bool check_node_number(int address);
           int get_node_number();
       };
       pmt::pmt_t _best_direction;
       std::vector<double> _best_direction_each;
       std::set<int> _nodes;
       std::vector<radio> _table;
       std::vector<int> _node_addresses;
       std::vector<double> _snr_values;
       std::vector<double> _angle_values;
       std::vector<double>::iterator _biggest;

     public:
      direction_finder_impl(int develop_mode, double update_interval, double timeout_value, int destination_address);
      ~direction_finder_impl();
      double find_best_direction(radio input);
      void sweep_done(pmt::pmt_t sweep_done);
      void generate_node_table(pmt::pmt_t beacon_reply_in);
      void calculate();
    };

  } // namespace inets
} // namespace gr

#endif /* INCLUDED_INETS_DIRECTION_FINDER_IMPL_H */