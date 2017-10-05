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
#include "direction_finder_impl.h"
#include <vector>
#include <algorithm>
#include <iterator>

namespace gr {
  namespace inets {

    direction_finder::sptr
    direction_finder::make(int develop_mode, double update_interval, double timeout_value, int destination_address)
    {
      return gnuradio::get_initial_sptr
        (new direction_finder_impl(develop_mode, update_interval, timeout_value, destination_address));
    }

    /*
     * The private constructor
     */
    direction_finder_impl::direction_finder_impl(int develop_mode, double update_interval, double timeout_value, int destination_address)
      : gr::block("direction_finder",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
              _develop_mode(develop_mode),
              _update_interval(update_interval),
              _timeout_value(timeout_value),
              _destination_address(destination_address),
              _sweep_done(false),
              _virgin(0),
              _counter(0)
    {
      if(develop_mode)
        std::cout << "develop_mode of Direction mapper is activated." << '\n';
      message_port_register_out(pmt::mp("best_direction_out"));
      message_port_register_in(pmt::mp("beacon_reply_in"));
      message_port_register_in(pmt::mp("sweep_done_in"));
      set_msg_handler(pmt::mp("beacon_reply_in"), boost::bind(&direction_finder_impl::generate_node_table, this, _1));
      set_msg_handler(pmt::mp("sweep_done_in"), boost::bind(&direction_finder_impl::sweep_done, this, _1));
    }

    /*
     * Our virtual destructor.
     */
    direction_finder_impl::~direction_finder_impl()
    {
    }

    direction_finder_impl::radio::radio(){
      int node_number = 0;
      std::vector<double> snr_radio;
      std::vector<double> angle_radio;
    }

    direction_finder_impl::radio::~radio(){}

    void direction_finder_impl::radio::insert_snr(double element) {
      this->snr_radio.push_back(element);
    }

    void direction_finder_impl::radio::insert_angle(double element) {
      this->angle_radio.push_back(element);
    }

    std::vector<double>::iterator direction_finder_impl::radio::find_max_snr(){
      return std::max_element(this->snr_radio.begin(), this->snr_radio.end());
    }

    double direction_finder_impl::radio::find_coresponding_angle(std::vector<double>::iterator it){
      int index = std::distance(this->snr_radio.begin(), it);
      double corresponding_angle = this->angle_radio[index];
      return corresponding_angle;
    }

    void direction_finder_impl::radio::set_node_number(int number) {
      if (this-> node_number == 0 || this-> node_number > 10 || this-> node_number < 0 ) {
        this->node_number = number;
      }
      /*else
        std::cout << "Node number was already set." << '\n';*/
    }

    bool direction_finder_impl::radio::check_node_number(int address) {
      if (this->node_number == address && this->node_number != 0) {
        return true;
      }
      else
        return false;
    }

    int direction_finder_impl::radio::get_node_number() {
      return this->node_number;
    }

    void direction_finder_impl::generate_node_table(pmt::pmt_t beacon_reply_in) {
      int update_interval = _update_interval;
      int timeout_value = _timeout_value;
      pmt::pmt_t not_found = pmt::from_long(7);
      int received_frame_address = pmt::to_long(pmt::dict_ref(beacon_reply_in, pmt::string_to_symbol("source_address"), not_found));
      _nodes.insert(received_frame_address);
      double snr = pmt::to_double(pmt::dict_ref(beacon_reply_in, pmt::string_to_symbol("reserved_field_I"), not_found));
      double angle = pmt::to_double(pmt::dict_ref(beacon_reply_in, pmt::string_to_symbol("reserved_field_II"), not_found));
      if (_develop_mode) {
        std::cout << "Incoming node address: " << received_frame_address <<'\n';
        std::cout << "SNR: " << snr <<'\n';
        std::cout << "Direction of the destined node : " << angle <<'\n';
      }
      _node_addresses.push_back(received_frame_address);
      _snr_values.push_back(snr);
      _angle_values.push_back(angle);
    }

    void direction_finder_impl::calculate(){
      int initial_size = _nodes.size();
      //std::cout << "initial_size: " << initial_size << '\n';
      for (size_t i = 0; i < initial_size; i++) {
        radio newRadio;
        _table.push_back(newRadio);
        double newAngle;
        _best_direction_each.push_back(newAngle);
      }
      std::vector<int>::iterator it;
      for (int i = 0; i < initial_size; i++) {
        std::vector<int> used_node_numbers;
        if (i != 0) {
          used_node_numbers.push_back(_table[i-1].get_node_number());
        }
        for (it = _node_addresses.begin(); it != _node_addresses.end(); it++){
          while (_counter < used_node_numbers.size()) {
            std::cout << "hellooo" << '\n';
            while (used_node_numbers[_counter] == *it && it != _node_addresses.end()) {
              it++;
            }
            _counter++;
          }
          _table[i].set_node_number(*it);
          std::cout << "node number: " << *it << '\n';
          if (_table[i].get_node_number() == *it) {
            int index = std::distance(_node_addresses.begin(), it);
            _table[i].insert_snr(_snr_values[index]);
            //std::cout << "snr: "  << _snr_values[index] << '\n';
            _table[i].insert_angle(_angle_values[index]);
            //std::cout << "angle: " << _angle_values[index] << '\n';
          }
        }
        _best_direction_each[i] = find_best_direction(_table[i]);
      }
    }

    double direction_finder_impl::find_best_direction(radio input){
      _biggest = input.find_max_snr();
      double snr_max = *_biggest;
      if (_develop_mode) {
        std::cout << "SNR Max: "<< snr_max << '\n';
      }
      double corresponding_angle = input.find_coresponding_angle(_biggest);
      if (_develop_mode) {
        std::cout << "Best Angle: " << corresponding_angle << '\n';
      }
      //_best_direction_each[input.get_node_number()] = corresponding_angle;
      //std::cout << "try: " << _best_direction_each[input.get_node_number()] << '\n';
      return corresponding_angle;
    }

    void direction_finder_impl::sweep_done(pmt::pmt_t sweep_done){
      _sweep_done = true;
      calculate();
      if (pmt::is_dict(sweep_done)) {
        _virgin = 1;
        pmt::pmt_t number = pmt::car(sweep_done);
        pmt::pmt_t angle = pmt::cdr(sweep_done);
        std::cout << "Sweeping Mode is disabled" << '\n';
        message_port_pub(pmt::mp("best_direction_out"), angle);
      }
      else if(_virgin != 1) {
        if (_angle_values.size() != 0) {
          if (_nodes.size() > 1) {
            for (size_t i = 0; i < _nodes.size(); i++) {
              std::cout << "asds" << '\n';
              _best_direction = pmt::cons(pmt::from_long(_table[i].get_node_number()), pmt::from_double(_best_direction_each[i]));
              message_port_pub(pmt::mp("best_direction_out"), _best_direction);
              std::cout << "Best Direction for Destination address " << _table[i].get_node_number() << " is: " << _best_direction_each[i] << '\n';
            }
          } else {
            _best_direction = pmt::cons(pmt::from_long(_table[0].get_node_number()), pmt::from_double(_best_direction_each[0]));
            message_port_pub(pmt::mp("best_direction_out"), _best_direction);
            std::cout << "THE DESTINATION NODE IS AT ANGLE "<< pmt::to_double(pmt::cdr(_best_direction)) << '\n';
            std::cout << "TRANSMITTER IS LOCKED TO ANGLE " <<  pmt::to_double(pmt::cdr(_best_direction)) << '\n';
          }
        }
        else {
          std::cout << "SWEEP DONE BUT NO BEACON REPLY RECEIVED!" << '\n';
          std::cout << "DIRECTING THE ANTTENNA TO BROADSIDE DIRECTION (0 DEGREES)" << '\n';
          _best_direction = pmt::from_long(0);
          message_port_pub(pmt::mp("best_direction_out"), _best_direction);
        }
      }
    }
  } /* namespace inets */
} /* namespace gr */
