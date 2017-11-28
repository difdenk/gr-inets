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
              _first_time(true),
              _virgin(0),
              _counter(0),
              _timeout(false),
              _snr(0),
              _ack_received(false),
              _search_mode(false),
              //_difference(0),
              _indicator(false)

    {
      if(develop_mode)
        std::cout << "develop_mode of Direction mapper is activated." << '\n';
      message_port_register_out(pmt::mp("best_direction_out"));
      message_port_register_out(pmt::mp("movement_tracker_out"));
      message_port_register_in(pmt::mp("beacon_reply_in"));
      message_port_register_in(pmt::mp("ack_in"));
      message_port_register_in(pmt::mp("sweep_done_in"));
      message_port_register_in(pmt::mp("timeout_in"));
      set_msg_handler(pmt::mp("beacon_reply_in"), boost::bind(&direction_finder_impl::generate_node_table, this, _1));
      set_msg_handler(pmt::mp("ack_in"), boost::bind(&direction_finder_impl::generate_ack_table, this, _1));
      set_msg_handler(pmt::mp("sweep_done_in"), boost::bind(&direction_finder_impl::sweep_done, this, _1));
      set_msg_handler(pmt::mp("timeout_in"), boost::bind(&direction_finder_impl::timeout, this, _1));
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

    void direction_finder_impl::radio::clean_the_node(std::vector<int> &n, std::vector<double> &s, std::vector<double> &a) {
      this->snr_radio.clear();
      this->angle_radio.clear();
      n.clear();
      s.clear();
      a.clear();
    }

    void direction_finder_impl::radio::insert_angle(double element) {
      this->angle_radio.push_back(element);
    }

    double direction_finder_impl::radio::get_moving_average(std::vector<double>::iterator it, int update){
      //std::cout << "The value of the last snr according to the iterator: " << *it << '\n';
      //std::cout << "The value of the beginning snr according to the iterator: " << *(it - update/2) << '\n';
      std::vector<double> avrgVec(it - update, it);
      double sum = 0;
      for (size_t i = 0; i < avrgVec.size(); i++) {
        sum = sum + avrgVec[i];
      }
      double average = sum / avrgVec.size();
      std::cout << "Current Average SNR: " << average << '\n';
      return average;
    }

    std::vector<double>::iterator direction_finder_impl::radio::get_last_snr() {
      return this->snr_radio.end()-1;

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
      //else
        //std::cout << "Node number was already set." << '\n';
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

    int direction_finder_impl::radio::ack_size() {
      return this->snr_radio.size();
    }

    direction_finder_impl::timer::timer(){
      this->start = 0;
    };

    direction_finder_impl::timer::~timer(){};

    int direction_finder_impl::timer::start_timer(double duration){
      if (this->start == 0) {
        this->start = clock();
      }
      if (duration > ((clock() - start) / (double) CLOCKS_PER_SEC)) {
        return this->node;
      }
      else {
        return 0;
      }

    }


    void direction_finder_impl::generate_node_table(pmt::pmt_t beacon_reply_in) {
      int update_interval = _update_interval;
      int timeout_value = _timeout_value;
      pmt::pmt_t not_found = pmt::from_long(7);
      int received_frame_address = pmt::to_long(pmt::dict_ref(beacon_reply_in, pmt::string_to_symbol("source_address"), not_found));
      _nodes.insert(received_frame_address);
      double snr = pmt::to_double(pmt::dict_ref(beacon_reply_in, pmt::string_to_symbol("reserved_field_I"), not_found));
      double angle = pmt::to_double(pmt::dict_ref(beacon_reply_in, pmt::string_to_symbol("reserved_field_II"), not_found));
      if (_develop_mode && !_sweep_done) {
        std::cout << "Incoming node address: " << received_frame_address <<'\n';
        std::cout << "SNR: " << snr <<'\n';
        std::cout << "Direction of the destined node : " << angle <<'\n';
      }
      _node_addresses.push_back(received_frame_address);
      _snr_values.push_back(snr);
      _angle_values.push_back(angle);
      /*if (_sweep_done && _ack_received ) {
        _beacon_reply_received = true;
        std::cout << "BEACON REPLY HAS BEEN RECEIVED FOR TRACKING !" << '\n';
        std::cout << "INCOMING SNR: " << snr << '\n';
        std::cout << "INCOMING ANGLE: " << angle << '\n';
        double snr_difference =snr - _average_snr_each[0];
        std::cout << " BEACON SNR DIFFERENCE: "<< snr_difference << '\n';
        if (snr_difference > -1) {
          _difference = 0;
          std::cout << "Node is assumed to be moving !" << '\n';
          std::cout << "Tracking... " << '\n';
          std::cout << "New node location is at : " << angle << " degrees." << '\n';
          sweep_done(pmt::pmt_from_complex(received_frame_address, angle));
          _average_snr_each[0] = snr;
          std::cout << "Antennas are Set !!" << '\n';
          _search_mode = false;
        }
      }*/
    }

    void direction_finder_impl::timeout(pmt::pmt_t expired) {
      if (_nodes.size() > 1) {
        std::vector<int>::iterator it = _node_addresses_ack.end() - 1;
        int last_ack_received_neighbour = *it;
        //std::cout << "the value of *it: " << *it << '\n';
        std::set<int> nodes = _nodes; //temp storage
        nodes.erase(last_ack_received_neighbour);
        start_tracking(*nodes.begin(), 12);
        _guilty = *nodes.begin();
        //std::cout << "/* message */" << *nodes.begin() << '\n';
      }
      else {
        start_tracking(*_nodes.begin(), 12);
        _guilty = *_nodes.begin();
      }
      std::cout << "Timeout !!! No ACK received for 3 seconds !!" << '\n';
      std::cout << "Connection lost with node number " << _guilty << '\n';
      _search_mode = true;
      _timeout = true;
      std::cout << "this is the real guilty:"  << _guilty <<'\n';
    }

    void direction_finder_impl::generate_ack_table(pmt::pmt_t ack_in) {
      //std::cout << _average_snr << " times this function is called." << '\n';
      int update_interval = _update_interval;
      //int timeout_value = _timeout_value;
      pmt::pmt_t not_found = pmt::from_long(7);
      int received_frame_address = pmt::to_long(pmt::dict_ref(ack_in, pmt::string_to_symbol("source_address"), not_found));
      /*if (_timer.size() == 0) {
        for (size_t i = 0; i < _nodes.size(); i++) {
          timer newTimer;
          _timer.push_back(newTimer);
        }
      }*/
      _nodes.insert(received_frame_address);
      _snr = pmt::to_double(pmt::dict_ref(ack_in, pmt::string_to_symbol("reserved_field_I"), not_found));
      if (_develop_mode) {
        std::cout << "Incoming node address: " << received_frame_address <<'\n';
        std::cout << "SNR: " << _snr <<'\n';
      }
      _node_addresses_ack.push_back(received_frame_address);
      _snr_values_ack.push_back(_snr);
      if (_snr_values_ack.size() > 2 * update_interval) {
        sort(received_frame_address);
        _snr_values_ack.erase(_snr_values_ack.begin());
        _node_addresses_ack.erase(_node_addresses_ack.begin());
      }
    }

    void direction_finder_impl::sort(int received_frame) {
      int timer = rand()%40;
      int initial_size = _nodes.size();
      //std::cout << "initial_size: " << initial_size << '\n';
      for (size_t i = 0; i < initial_size; i++) {
        if (!_indicator) {
          double newAvrg;
          _average_snr_each.push_back(newAvrg);
          double newDiff;
          _difference.push_back(newDiff);
          radio newRadio;
          _table_ack.push_back(newRadio);
        }
      }
      _indicator = true;
      //std::cout << "dif: " << _difference.size() << '\n';
      //std::cout << "Table: " << _table_ack.size() << '\n';
      //std::cout << "avg_snr: " << _average_snr_each.size() << '\n';
      std::vector<int>::iterator it;
      for (int i = 0; i < initial_size; i++) {
        std::cout << "i: " << i <<'\n';
        std::vector<int> used_node_numbers;
        if (i != 0) {
          used_node_numbers.push_back(_table[i-1].get_node_number());
          //std::cout << "used_node_numbers: " << used_node_numbers[i-1] <<'\n';
        }
        for (it = _node_addresses_ack.begin(); it != _node_addresses_ack.end(); it++){
          while (_counter < used_node_numbers.size()) {
            while (used_node_numbers[_counter] == *it && it != _node_addresses.end()) {
              it++;
            }
            _counter++;
          }
          _table_ack[i].set_node_number(*it);
          //std::cout << "node number: " << *it << '\n';
          if (_table_ack[i].get_node_number() == *it) {
            int index = std::distance(_node_addresses_ack.begin(), it);
            //std::cout << "index: " << index << '\n';
            _table_ack[i].insert_snr(_snr_values_ack[index]);
            //std::cout << "snr: "  << _snr_values_ack[index] << '\n';
          }
        }
        //std::cout << "debug: " << _table_ack[i].get_node_number() << '\n';
        if (_first_time != false && !_search_mode) {
          _average_snr_each[i] = _table_ack[i].get_moving_average(_table_ack[i].get_last_snr(), _table_ack[i].ack_size());
          //double old_avrg = _table_ack[i].get_moving_average((_table_ack[i].get_last_snr()) - 10, _update_interval / 2);
          std::cout << "Default Average for node number "<< _table_ack[i].get_node_number() << " : " << _average_snr_each[i] << '\n';
        }
        if (_first_time == false && !_search_mode && timer == 7) {
          _average_snr_each[i] = _table_ack[i].get_moving_average(_table_ack[i].get_last_snr(), 20);
          //double old_avrg = _table_ack[i].get_moving_average((_table_ack[i].get_last_snr()) - 10, _update_interval / 2);
          std::cout << "Default Average for node number "<< _table_ack[i].get_node_number() << " : " << _average_snr_each[i] << '\n';
        }
        //std::vector<double>::iterator ptr = _table[i].find_max_snr();
        if (_search_mode) {
          //std::cout << "search_mode is on" << '\n';
          _difference[i] = _average_snr_each[i] - _snr;
        }
        else {
          //std::cout << "search_mode is off" << '\n';
          _difference[i] = _average_snr_each[i] - _table_ack[i].get_moving_average(_table_ack[i].get_last_snr(), 15);
        }
        //int v = rand()%100;
        /*if (v < 50) {
          std::cout << "MAX SNR: " << *ptr <<'\n';
        }*/
        std::cout << "Difference in SNR: " << _difference[i] << '\n';
        if (_difference[i] > 3 && !_search_mode) {
          _search_mode = true;
          _ack_received = true;
          start_tracking(_table_ack[i].get_node_number(), _difference[i]);
        }
        if (_difference[i] < -2 && !_search_mode) {
          _average_snr_each[i] = _snr;
        }
        std::cout << "missing node: " << _guilty << '\n';
        if ((received_frame == _guilty) && (_timeout || (_search_mode && (_difference[i] < 5)))) {
          _search_mode = false;
          _timeout = false;
          //_difference = 0;
          std::cout << "Tracking... " << '\n';
          sweep_done(pmt::pmt_from_complex(_guilty, 1));
          std::cout << "should be always 2: " << received_frame << '\n';
          _average_snr_each[i] = _table_ack[i].get_moving_average(_table_ack[i].get_last_snr(), 1);
          std::cout << "Antennas are Set !!" << '\n';


        }
        /*else if(_search_mode && !_beacon_reply_received) {
          _lost = _table_ack[i].get_node_number();
          start_tracking(_lost);
        }*/
      }
      _first_time = false;
    }

    /*void direction_finder_impl::start_tracking(int lost) {
      std::cout << "The node other is possibly moving !!" << '\n';
      pmt::pmt_t change_direction = pmt::cons(pmt::from_long(lost), pmt::from_double(_difference));
      message_port_pub(pmt::mp("movement_tracker_out"), change_direction);
    }*/

    void direction_finder_impl::start_tracking(int lost, double difference) {
      if (_timeout_value == 0) {
        if (_develop_mode) {
          std::cout << "Tracking is disabled ! " << '\n';
        }
      } else {
        std::cout << "The other node "<< lost << " is possibly moving !!" << '\n';
        pmt::pmt_t change_direction = pmt::cons(pmt::from_long(lost), pmt::from_double(difference));
        message_port_pub(pmt::mp("movement_tracker_out"), change_direction);
      }
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
            //std::cout << "hellooo" << '\n';
            while (used_node_numbers[_counter] == *it && it != _node_addresses.end()) {
              it++;
            }
            _counter++;
          }
          _table[i].set_node_number(*it);
          //std::cout << "node number: " << *it << '\n';
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
      if (pmt::is_dict(sweep_done)) {
        _virgin = 1;
        pmt::pmt_t number = pmt::car(sweep_done);
        pmt::pmt_t angle = pmt::cdr(sweep_done);
        std::cout << "SWEEPING MODE IS DISABLED !" << '\n';
        std::cout << "DEFAULT DIRECTION IS " << angle << " DEGREES !" << '\n';
        message_port_pub(pmt::mp("best_direction_out"), angle);
      }
      else if (pmt::is_complex(sweep_done)) {
        std::complex<double> ID = pmt::to_complex(sweep_done);
        int node = std::real(ID);
        double angle = std::imag(ID);
        _best_direction = pmt::pmt_from_complex(node, angle);
        message_port_pub(pmt::mp("best_direction_out"), _best_direction);
        //std::cout << "New best Direction for Destination address " << node << " is: " << angle << '\n';
      }
      else if(_virgin != 1) {
        calculate();
        if (_angle_values.size() != 0) {
          if (_nodes.size() > 1) {
            for (size_t i = 0; i < _nodes.size(); i++) {
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
