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
#include "direction_mapper_impl.h"
#include <math.h>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#define _PI 3.14159265359

namespace gr {
  namespace inets {

    direction_mapper::sptr
    direction_mapper::make(int develop_mode, int block_id, double phase_1, double phase_2, double phase_3, double phase_4)
    {
      return gnuradio::get_initial_sptr
        (new direction_mapper_impl(develop_mode, block_id, phase_1, phase_2, phase_3, phase_4));
    }

    /*
     * The private constructor
     */
    direction_mapper_impl::direction_mapper_impl(int develop_mode, int block_id, double phase_1, double phase_2, double phase_3, double phase_4)
      : gr::block("direction_mapper",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
              _develop_mode(develop_mode),
              _block_id(block_id),
              _indicator(0),
              _count(0),
              //_baseline(0),
              //_trial_angle(0.01),
              _max(0),
              _phase_1(phase_1*_PI/180),
              _phase_2(phase_2*_PI/180),
              _phase_3(phase_3*_PI/180),
              _phase_4(phase_4*_PI/180),
              _side(true),
              _search_mode(false)
    {
      if(develop_mode)
      std::cout << "develop_mode of Direction mapper is activated." << '\n';
      message_port_register_out(pmt::mp("phase_out"));
      message_port_register_in(pmt::mp("movement_tracker_in"));
      message_port_register_in(pmt::mp("frame_in"));
      message_port_register_in(pmt::mp("destination_address_check_in")); // checks the destination address of the incoming frames
      set_msg_handler(pmt::mp("frame_in"), boost::bind(&direction_mapper_impl::accept_frame, this, _1));
      set_msg_handler(pmt::mp("destination_address_check_in"), boost::bind(&direction_mapper_impl::check_destination, this, _1));
      set_msg_handler(pmt::mp("movement_tracker_in"), boost::bind(&direction_mapper_impl::track_movement, this, _1));
      _phase_values = pmt::make_dict();
      _phase_key1 = pmt::string_to_symbol("phase_key1");
      _phase_key2 = pmt::string_to_symbol("phase_key2");
      _phase_key3 = pmt::string_to_symbol("phase_key3");
      _phase_key4 = pmt::string_to_symbol("phase_key4");
    }

    /*
     * Our virtual destructor.
     */
    direction_mapper_impl::~direction_mapper_impl()
    {
    }

    void direction_mapper_impl::check_destination(pmt::pmt_t frame_in) {
      if (_search_mode) {
        _max++;
        if (_max < 4) {
          _trial_angle = _tracking_angle1;
        }
        else if(_max  >= 4 && _max < 8 ) {
          _trial_angle = _tracking_angle2;
        }
        else {
          _max = 0;
          _trial_angle = _tracking_angle2;
        }
      }
      _phase_values = pmt::make_dict();
      if (_indicator == 1 || _count != 0) {
        pmt::pmt_t frame_type;
        pmt::pmt_t not_found = pmt::from_long(7);
        if (pmt::is_dict(frame_in) && ((pmt::to_long(pmt::dict_ref(frame_in, pmt::string_to_symbol("frame_type"), not_found)) == 1) || (pmt::to_long(pmt::dict_ref(frame_in, pmt::string_to_symbol("frame_type"), not_found)) == 2))) {
          if (_develop_mode) {
            //std::cout << "frame_in is a dict" << '\n';
          }
          pmt::pmt_t destination = pmt::dict_ref(frame_in, pmt::string_to_symbol("destination_address"), not_found);
          int destination_address = pmt::to_long(destination);
          std::cout << "incoming frame address: " << destination_address << '\n';
          if (_nodes.size() != 0) {
            _index = std::distance(_nodes.begin(), std::find(_nodes.begin(), _nodes.end(), destination_address));
            if (_search_mode && (_guilty == destination_address)) {
              //std::cout << "trial: " << _trial_angle << '\n';
              pmt::pmt_t phase_value1 = pmt::from_double(_trial_angle*_PI/180);
              pmt::pmt_t phase_value2 = pmt::from_double(_trial_angle*_PI/180);
              pmt::pmt_t phase_value3 = pmt::from_double(_trial_angle*_PI/180);
              pmt::pmt_t phase_value4 = pmt::from_double(_trial_angle*_PI/180);
              _phase_values = pmt::dict_add(_phase_values, _phase_key1, phase_value1);
              _phase_values = pmt::dict_add(_phase_values, _phase_key2, phase_value2);
              _phase_values = pmt::dict_add(_phase_values, _phase_key3, phase_value3);
              _phase_values = pmt::dict_add(_phase_values, _phase_key4, phase_value4);
              std::cout << "Angle mapper sends to t_control for a change:  " << _trial_angle <<'\n';
              message_port_pub(pmt::mp("phase_out"), _phase_values);
            }
            else {
              double corresponding_angle = _angles[_index];
              pmt::pmt_t phase_value1 = pmt::from_double(corresponding_angle*_PI/180);
              pmt::pmt_t phase_value2 = pmt::from_double(corresponding_angle*_PI/180);
              pmt::pmt_t phase_value3 = pmt::from_double(corresponding_angle*_PI/180);
              pmt::pmt_t phase_value4 = pmt::from_double(corresponding_angle*_PI/180);
              _phase_values = pmt::dict_add(_phase_values, _phase_key1, phase_value1);
              _phase_values = pmt::dict_add(_phase_values, _phase_key2, phase_value2);
              _phase_values = pmt::dict_add(_phase_values, _phase_key3, phase_value3);
              _phase_values = pmt::dict_add(_phase_values, _phase_key4, phase_value4);
              std::cout << "Angle mapper sends to t_control:  " << corresponding_angle <<'\n';
              message_port_pub(pmt::mp("phase_out"), _phase_values);
            }
          }
        }
      }
    }

    void direction_mapper_impl::track_movement(pmt::pmt_t track) {
      if (_baseline.size() == 0) {
        std::cout << "Default angles of the nodes are initialized !" << '\n';
        for (size_t i = 0; i < _angles.size(); i++) {
          _baseline.push_back(_angles[i]);
        }
      }
      double radiation_pattern_function;
      double variance_a;
      bool one_or_zero = rand() % 2;
      if (one_or_zero) {
        variance_a = rand() % 3;
      } else {
        variance_a = -(rand() % 3);
      }
      _search_mode = true;
      if (pmt::is_dict(track)) {
        std::cout << "Node is getting away !!" << '\n';
        //_phase_values = pmt::make_dict();
        _guilty = pmt::to_long(pmt::car(track));
        std::cout << "Node number of tracking: " << _guilty << '\n';
        double difference = pmt::to_double(pmt::cdr(track));
        radiation_pattern_function = (10*sqrt((0.3669 + (3*difference)/5)))/3 + 1.1;
        std::cout << "angle change: " << radiation_pattern_function << '\n';
        //if (difference = 30) {
        //  _timeout = true;
        //}
        _index = std::distance(_nodes.begin(), std::find(_nodes.begin(), _nodes.end(), _guilty));
        //std::cout << "index: " << index << '\n';
        double corresponding_angle = _angles[_index];
        /*if ( _baseline == 0 || !_search_mode) {
          std::cout << "Baseline is initialized !!" << '\n';
          _baseline = _angles[_index];
        }*/
        std::cout << "this should be same as Baseline: " << _angles[_index] << '\n';
        std::cout << "baseline: " << _baseline[_index] <<'\n';
        if(_search_mode) {
          if (true) {
              _tracking_angle1 = _baseline[_index] + radiation_pattern_function + variance_a;
              //_angles[index] = _tracking_angle1;
              std::cout << "tracking angle1: " << _tracking_angle1 << '\n';
              _tracking_angle2 = _baseline[_index] - radiation_pattern_function + variance_a;
              //_angles[index] = _tracking_angle2;
              std::cout << "tracking angle2: " << _tracking_angle2 << '\n';
          } /*else {
              std::cout << "TIMEOUT !" << '\n';
              _tracking_angle1 = _baseline + 9 + variance_a;
              std::cout << "tracking angle1: " << _tracking_angle1 << '\n';
              //_angles[index] = _tracking_angle1;
              //std::cout << "newish angle: " << _angles[index] << '\n';

              _tracking_angle2 = _baseline - 9 + variance_a;
              std::cout << "tracking angle2: " << _tracking_angle2 << '\n';
              //_angles[index] = _tracking_angle2;
          }*/
        } else {
          std::cout << "Movement is not dict !" << '\n';
        }
      }
    }

    void direction_mapper_impl::accept_frame(pmt::pmt_t trigger) {
      _phase_values = pmt::make_dict();
      if (pmt::is_dict(trigger)) {
        if (_count == 0) {
          int node_number = pmt::to_long(pmt::car(trigger));
          double angle = pmt::to_double(pmt::cdr(trigger));
          _nodes.push_back(node_number);
          _angles.push_back(angle);
          double best_direction = angle;
          pmt::pmt_t phase_value1 = pmt::from_double(best_direction*_PI/180);
          pmt::pmt_t phase_value2 = pmt::from_double(best_direction*_PI/180);
          pmt::pmt_t phase_value3 = pmt::from_double(best_direction*_PI/180);
          pmt::pmt_t phase_value4 = pmt::from_double(best_direction*_PI/180);
          _phase_values = pmt::dict_add(_phase_values, _phase_key1, phase_value1);
          _phase_values = pmt::dict_add(_phase_values, _phase_key2, phase_value2);
          _phase_values = pmt::dict_add(_phase_values, _phase_key3, phase_value3);
          _phase_values = pmt::dict_add(_phase_values, _phase_key4, phase_value4);
          message_port_pub(pmt::mp("phase_out"), _phase_values);
          //pmt::print(_phase_values);
          if (_develop_mode) {
            std::cout << "Best direction sent" << '\n';
            std::cout << "angles" << _angles[0] << '\n';
          }
          _count ++;
        }
        else {
          _indicator = 1;
          int node_number = pmt::to_long(pmt::car(trigger));
          double angle = pmt::to_double(pmt::cdr(trigger));
          _nodes.push_back(node_number);
          _angles.push_back(angle);
          if (_develop_mode) {
            //std::cout << "Destination Address: " << node_number << '\n';
            //std::cout << "Location in Angles: " << angle << '\n';
          }
        }
      }
      else if(pmt::is_complex(trigger)) {
        std::complex<double> node = pmt::to_complex(trigger);
        int destination_address = std::real(node);
        std::cout << "New angle for destination " << destination_address << " arrived!!" << '\n';
        _index = std::distance(_nodes.begin(), std::find(_nodes.begin(), _nodes.end(), destination_address));
        _search_mode = false;
        std::cout << "trrr " << _trial_angle << '\n';
        double new_angle = _trial_angle;
        _angles[_index] = new_angle;
        _baseline[_index] = _angles[_index];
        _timeout = false;
        std::cout << "Baseline has been reset !" << '\n';
        std::cout << "New angle: " << _angles[_index] <<'\n';
        pmt::pmt_t phase_value1 = pmt::from_double(new_angle*_PI/180);
        pmt::pmt_t phase_value2 = pmt::from_double(new_angle*_PI/180);
        pmt::pmt_t phase_value3 = pmt::from_double(new_angle*_PI/180);
        pmt::pmt_t phase_value4 = pmt::from_double(new_angle*_PI/180);
        _phase_values = pmt::dict_add(_phase_values, _phase_key1, phase_value1);
        _phase_values = pmt::dict_add(_phase_values, _phase_key2, phase_value2);
        _phase_values = pmt::dict_add(_phase_values, _phase_key3, phase_value3);
        _phase_values = pmt::dict_add(_phase_values, _phase_key4, phase_value4);
        message_port_pub(pmt::mp("phase_out"), _phase_values);
      }
      else {
        pmt::pmt_t phase_value1 = pmt::from_double(_phase_1);
        pmt::pmt_t phase_value2 = pmt::from_double(_phase_2);
        pmt::pmt_t phase_value3 = pmt::from_double(_phase_3);
        pmt::pmt_t phase_value4 = pmt::from_double(_phase_4);
        _phase_values = pmt::dict_add(_phase_values, _phase_key1, phase_value1);
        _phase_values = pmt::dict_add(_phase_values, _phase_key2, phase_value2);
        _phase_values = pmt::dict_add(_phase_values, _phase_key3, phase_value3);
        _phase_values = pmt::dict_add(_phase_values, _phase_key4, phase_value4);
        message_port_pub(pmt::mp("phase_out"), _phase_values);
        if (_develop_mode) {
          std::cout << "Default phase sent" << '\n';
        }
      }
    }
  } /* namespace inets */
} /* namespace gr */
