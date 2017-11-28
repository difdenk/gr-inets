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
#include <ctime>
#include <iostream>
#include <cstdio>

namespace gr {
  namespace inets {

    class direction_finder_impl : public direction_finder
    {
     private:
       class radio {
         private:
           int node_number;
           std::vector<double> snr_radio;
           std::vector<double> angle_radio;
         public:
           radio();
           ~radio();
           void insert_snr(double element);
           void clean_the_node(std::vector<int> &n, std::vector<double> &s, std::vector<double> &a);
           std::vector<double>::iterator get_last_snr();
           void insert_angle(double element);
           std::vector<double>::iterator find_max_snr();
           double find_coresponding_angle(std::vector<double>::iterator it);
           double get_moving_average(std::vector<double>::iterator it, int update);
           void set_node_number(int number);
           bool check_node_number(int address);
           int get_node_number();
           int ack_size();
       };
       class timer {
        private:
         double duration;
         double start;
         int node;
       public:
         timer();
         ~timer();
         int start_timer(double duration);
         void reset_timer();
         void pause_timer();
       };
       int _develop_mode;
       int _destination_address;
       int _virgin;
       int _counter;
       int _lost;
       int _guilty;
       double _update_interval;
       double _timeout_value;
       bool _timeout;
       //double _difference;
       double _snr;
       bool _sweep_done;
       bool _ack_received;
       bool _first_time;
       bool _search_mode;
       bool _indicator;
       pmt::pmt_t _best_direction;
       std::vector<double> _best_direction_each;
       std::vector<double> _average_snr_each;
       std::set<int> _nodes;
       std::vector<timer> _timer;
       std::vector<radio> _table;
       std::vector<radio> _table_ack;
       std::vector<int> _node_addresses;
       std::vector<int> _node_addresses_ack;
       std::vector<double> _snr_values;
       std::vector<double> _difference;
       std::vector<double> _snr_values_ack;
       std::vector<double> _angle_values;
       std::vector<double>::iterator _biggest;

     public:
      direction_finder_impl(int develop_mode, double update_interval, double timeout_value, int destination_address);
      ~direction_finder_impl();
      double find_best_direction(radio input);
      void sweep_done(pmt::pmt_t sweep_done);
      void generate_node_table(pmt::pmt_t beacon_reply_in);
      void generate_ack_table(pmt::pmt_t ack_in);
      void calculate();
      void sort(int received_frame);
      void start_tracking(int lost);
      void start_tracking(int lost, double difference);
      void timeout(pmt::pmt_t expired);

    };

  } // namespace inets
} // namespace gr

#endif /* INCLUDED_INETS_DIRECTION_FINDER_IMPL_H */
