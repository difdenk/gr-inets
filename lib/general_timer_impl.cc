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
#include "general_timer_impl.h"
//#include <algorithm>

namespace gr {
  namespace inets {

    general_timer::sptr
    general_timer::make(int develop_mode, int block_id, int timer_type, int duration_ms, int system_time_granularity_us, double reserved_time_ms)
    {
      return gnuradio::get_initial_sptr
        (new general_timer_impl(develop_mode, block_id, timer_type, duration_ms, system_time_granularity_us, reserved_time_ms));
    }

    /*
     * The private constructor
     */
    general_timer_impl::general_timer_impl(int develop_mode, int block_id, int timer_type, int duration_ms, int system_time_granularity_us, double reserved_time_ms)
      : gr::block("general_timer",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
        _develop_mode(develop_mode),
        _block_id(block_id),
        _timer_type(timer_type),
        _duration_ms(duration_ms),
        _system_time_granularity_us(system_time_granularity_us),
        _reserved_time_ms(reserved_time_ms),
        _timer_bias_s(0),
        _in_active(false)
    {
      if(_develop_mode)
        std::cout << "develop_mode of general_timer ID: " << _block_id << " is activated." << std::endl;
      message_port_register_in(pmt::mp("active_in"));
      set_msg_handler(pmt::mp("active_in"), boost::bind(&general_timer_impl::start_timer, this, _1 ));
      message_port_register_in(pmt::mp("kill_timer_in"));
      set_msg_handler(pmt::mp("kill_timer_in"), boost::bind(&general_timer_impl::kill_timer, this, _1 ));
      message_port_register_out(pmt::mp("expire_signal_out"));
      message_port_register_out(pmt::mp("address_check_out"));
      _frame_info = pmt::from_long(0);
    }

    /*
     * Our virtual destructor.
     */
    general_timer_impl::~general_timer_impl()
    {
    }

    void
    general_timer_impl::start_timer(pmt::pmt_t trigger)
    {
       if(pmt::is_dict(trigger))
         _frame_info = trigger;
       pmt::pmt_t not_found;
       struct timeval t; 
       gettimeofday(&t, NULL);
       double current_time = t.tv_sec - double(int(t.tv_sec/100)*100) + t.tv_usec / 1000000.0;
       if(!_in_active)
       {
         if(_develop_mode)
           std::cout << "general_timer " << _block_id << " is triggered at time " << current_time << " s" << std::endl;
         _in_active = true;
         if(_timer_type == 0)
           boost::thread thrd(&general_timer_impl::countdown_oneshot_timer, this);       
         else if(_timer_type == 1)
           boost::thread thrd(&general_timer_impl::countdown_periodic_timer, this);       
         else if(_timer_type == 2)
         {
           int node_id = pmt::to_long(pmt::dict_ref(trigger, pmt::string_to_symbol("destination_address"), not_found));
           double slot_time = pmt::to_double(pmt::dict_ref(trigger, pmt::string_to_symbol("slot_time"), not_found));
           int address_check = pmt::to_long(pmt::dict_ref(trigger, pmt::string_to_symbol("address_check"), not_found));
           if(node_id == 0 && slot_time == 0 && address_check == 0)
           {
             _in_active = false;
             if(_develop_mode)
               std::cout << "general_timer ID: " << _block_id << " receives new scheduling information" << std::endl;
             _duration_list_ms.clear();
             _node_id_list.clear();
             _address_check_list.clear();
           }
           else
           {

             if(_develop_mode)
             { 
               std::cout << "general_timer ID: " << _block_id << " new waiting request are stored." << std::endl;
               std::cout << " slot_time is: " << slot_time << " and node ID is: " << node_id << std::endl;
             }
             _duration_list_ms.push_back(slot_time);
             _node_id_list.push_back(node_id);
             _address_check_list.push_back(address_check);
             boost::thread thrd(&general_timer_impl::countdown_continual_timer, this);
           }
         }
       }
       else
       {
         if(_timer_type == 0)
         {
           if(_develop_mode)
             std::cout << "general_timer ID: " << _block_id << " cannot be triggered before the last one finish." << std::endl;
         }
         else if(_timer_type == 1)
         {
           if(_develop_mode)
             std::cout << "general_timer id: " << _block_id << " is set to periodic mode so it can only be triggered once." << std::endl;
         }
         else if(_timer_type == 2)
         { 

           if(pmt::dict_has_key(trigger, pmt::string_to_symbol("slot_time")))
           {
             double slot_time = pmt::to_double(pmt::dict_ref(trigger, pmt::string_to_symbol("slot_time"), not_found));
             int node_id = pmt::to_double(pmt::dict_ref(trigger, pmt::string_to_symbol("destination_address"), not_found));
             int address_check = pmt::to_double(pmt::dict_ref(trigger, pmt::string_to_symbol("address_check"), not_found));
             if(node_id == 0 && slot_time == 0 && address_check == 0)
             {
               if(_develop_mode)
                 std::cout << "general_timer ID: " << _block_id << " terminates all scheduled timer because new scheduling information is received" << std::endl;
               _in_active = false;
               _duration_list_ms.clear();
               _node_id_list.clear();
               _address_check_list.clear();
             }
             else
             {
               double slot_time = pmt::to_double(pmt::dict_ref(trigger, pmt::string_to_symbol("slot_time"), not_found));
               double node_id = pmt::to_double(pmt::dict_ref(trigger, pmt::string_to_symbol("destination_address"), not_found));
               int address_check = pmt::to_double(pmt::dict_ref(trigger, pmt::string_to_symbol("address_check"), not_found));
               if(_develop_mode)
               {
                 std::cout << "general_timer ID: " << _block_id << " new waiting request are stored." << std::endl;
                 std::cout << " slot_time is: " << slot_time << " and node ID is: " << node_id << std::endl;
               }
               _duration_list_ms.push_back(slot_time);
               _node_id_list.push_back(node_id);
               _address_check_list.push_back(address_check);
             }
           }
           else
           {
             std::cout << "general_timer ID: " << _block_id << ": input pmt has no slot_time field. please check your connections." << std::endl;
           }
         }
       } 
    }
 
    void
    general_timer_impl::countdown_continual_timer()
    {
      while(_duration_list_ms.size() > 0 && _in_active)
      {
        pmt::pmt_t address_check = pmt::make_dict();
        address_check = pmt::dict_add(address_check, pmt::string_to_symbol("beacon_address_check"), pmt::from_long(_address_check_list.front()));
        message_port_pub(pmt::mp("address_check_out"), address_check);
        struct timeval t;
        gettimeofday(&t, NULL);
        double current_time = t.tv_sec + t.tv_usec / 1000000.0;
        double start_time = current_time;
        double start_time_show = t.tv_sec - double(int(t.tv_sec/100)*100) + t.tv_usec / 1000000.0;
        double current_time_show = start_time_show;
        double duration_ms = std::max(double(_duration_list_ms.front()) - _reserved_time_ms, double(0));
        if(_develop_mode)
        {
          std::cout << "general timer start time: " << start_time_show;
          if(_address_check_list.front())
            std::cout << ", it is my time slot " << std::endl;
          else
            std::cout << ", it is not my time slot " << std::endl;
        }
        while((current_time < start_time + double(duration_ms) / 1000 - _timer_bias_s) && _in_active)
        {
          gettimeofday(&t, NULL);
          current_time = t.tv_sec + t.tv_usec / 1000000.0;
          boost::this_thread::sleep(boost::posix_time::microseconds(_system_time_granularity_us));
          current_time_show = t.tv_sec - double(int(t.tv_sec/100)*100) + t.tv_usec / 1000000.0;
          // std::cout << "timeout is running at: " << current_time_show << std::endl;
        }
        std::cout << " _in_active after waiting is: " << _in_active << std::endl;
        gettimeofday(&t, NULL);
        current_time = t.tv_sec + t.tv_usec / 1000000.0;
        if(_develop_mode)
        {
          current_time_show = t.tv_sec - double(int(t.tv_sec/100)*100) + t.tv_usec / 1000000.0;
          if(_in_active)
          {
            _timer_bias_s = _timer_bias_s + current_time - start_time - duration_ms/1000;
            std::cout << "general timer ID: " << _block_id << " is expired at time " << current_time_show << " s. " << " actual duration is: " << current_time - start_time << " [s]" << "and the time bias is: " << _timer_bias_s << std::endl;
          }
          else
            std::cout << "general timer ID: " << _block_id << " is killed at time " << current_time_show << " s. " << " actual timeout duration is: " << current_time - start_time << " [s]" << std::endl;
        }
        pmt::pmt_t expire = pmt::make_dict();
        expire = pmt::dict_add(expire, pmt::string_to_symbol("time_stamp"), pmt::from_double(current_time));
        message_port_pub(pmt::mp("expire_signal_out"), expire);
        if(_in_active)
        {
          _duration_list_ms.erase(_duration_list_ms.begin());
          _node_id_list.erase(_node_id_list.begin());
          _address_check_list.erase(_address_check_list.begin());
        }
      }
      if(!_in_active)
      {
        _duration_list_ms.clear();
        _node_id_list.clear();
        _address_check_list.clear();
      }
      _in_active = false;
      pmt::pmt_t address_check = pmt::make_dict();
      address_check = pmt::dict_add(address_check, pmt::string_to_symbol("beacon_address_check"), pmt::from_long(0));
      message_port_pub(pmt::mp("address_check_out"), address_check);
    }

    void
    general_timer_impl::kill_timer(pmt::pmt_t trigger)
    {
      if(_in_active)
      {
        _in_active = false;
      }
      else
      {
        if(_develop_mode)
          std::cout << "general_timer ID: " << _block_id << " cannot be killed in inactive node. " << std::endl;
       
      }
    }
 
    void
    general_timer_impl::countdown_oneshot_timer()
    {
      struct timeval t;
      gettimeofday(&t, NULL);
      double current_time = t.tv_sec + t.tv_usec / 1000000.0;
      double start_time = current_time;
      double start_time_show = t.tv_sec - double(int(t.tv_sec/100)*100) + t.tv_usec / 1000000.0;
      double current_time_show = start_time_show;
      if(_develop_mode)
        std::cout << "timeout timer start time: " << start_time_show << std::endl;
      while((current_time < start_time + double(_duration_ms) / 1000 - _timer_bias_s) && _in_active)
      {
        gettimeofday(&t, NULL);
        current_time = t.tv_sec + t.tv_usec / 1000000.0;
        boost::this_thread::sleep(boost::posix_time::microseconds(_system_time_granularity_us));
        current_time_show = t.tv_sec - double(int(t.tv_sec/100)*100) + t.tv_usec / 1000000.0;
        // std::cout << "timeout is running at: " << current_time_show << std::endl;
      }
      gettimeofday(&t, NULL);
      current_time = t.tv_sec + t.tv_usec / 1000000.0;
      if(_develop_mode)
      {
        current_time_show = t.tv_sec - double(int(t.tv_sec/100)*100) + t.tv_usec / 1000000.0;
        if(_in_active)
        {
          _timer_bias_s = _timer_bias_s + current_time - start_time - double(_duration_ms)/1000;
          std::cout << "* general timer ID: " << _block_id << " is expired at time " << current_time_show << " s. " << " actual duration is: " << current_time - start_time << " [s]" << "and the time bias is: " << _timer_bias_s << std::endl;
        }
        else
          std::cout << "* general timer ID: " << _block_id << " is killed at time " << current_time_show << " s. " << " actual timeout duration is: " << current_time_show - start_time_show << " s" << std::endl;
      }
      if(pmt::is_dict(_frame_info))
      {
        _frame_info = pmt::dict_add(_frame_info, pmt::string_to_symbol("time_stamp"), pmt::from_double(current_time));
        message_port_pub(pmt::mp("expire_signal_out"), _frame_info);
      }
      else
      {
        pmt::pmt_t expire = pmt::make_dict();
        expire = pmt::dict_add(expire, pmt::string_to_symbol("time_stamp"), pmt::from_double(current_time));
        message_port_pub(pmt::mp("expire_signal_out"), expire);
      }
      _in_active = false;
    }
 
    void
    general_timer_impl::countdown_periodic_timer()
    {
      while(_in_active)
      {
        struct timeval t;
        gettimeofday(&t, NULL);
        double current_time = t.tv_sec + t.tv_usec / 1000000.0;
        double start_time = current_time;
        double start_time_show = t.tv_sec - double(int(t.tv_sec/100)*100) + t.tv_usec / 1000000.0;
        double current_time_show = start_time_show;
        while((current_time < start_time + double(_duration_ms) / 1000 - _timer_bias_s) && _in_active)
        {
          gettimeofday(&t, NULL);
          current_time = t.tv_sec + t.tv_usec / 1000000.0;
          boost::this_thread::sleep(boost::posix_time::microseconds(_system_time_granularity_us));
          current_time_show = t.tv_sec - double(int(t.tv_sec/100)*100) + t.tv_usec / 1000000.0;
          // std::cout << "timeout is running at: " << current_time_show << std::endl;
        }
        gettimeofday(&t, NULL);
        current_time = t.tv_sec + t.tv_usec / 1000000.0;
        if(_develop_mode)
        {
          current_time_show = t.tv_sec - double(int(t.tv_sec/100)*100) + t.tv_usec / 1000000.0;
          if(_in_active)
          {
            _timer_bias_s = _timer_bias_s + current_time - start_time - double(_duration_ms)/1000;
            std::cout << "* general timer ID: " << _block_id << " is expired at time " << current_time_show << " s. " << " actual duration is: " << current_time - start_time << " [ms]" << "and the time bias is: " << _timer_bias_s << std::endl;
          }
          else
          {
            std::cout << "* general timer ID: " << _block_id << " is killed at time " << current_time_show << " s. " << " actual timeout duration is: " << current_time_show - start_time_show << " s" << std::endl;
          }
        }
        if(pmt::is_dict(_frame_info))
        {
          _frame_info = pmt::dict_add(_frame_info, pmt::string_to_symbol("time_stamp"), pmt::from_double(current_time));
          message_port_pub(pmt::mp("expire_signal_out"), _frame_info);
        }
        else
        {
          pmt::pmt_t expire = pmt::make_dict();
          expire = pmt::dict_add(expire, pmt::string_to_symbol("time_stamp"), pmt::from_double(current_time));
          message_port_pub(pmt::mp("expire_signal_out"), expire);
        }
      }
    }

  } /* namespace inets */
} /* namespace gr */
