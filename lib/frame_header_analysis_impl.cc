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
#include <cmath>
#include "frame_header_analysis_impl.h"
#include <gnuradio/digital/crc32.h>
#include <volk/volk.h>
#include <boost/crc.hpp>

namespace gr {
  namespace inets {

    frame_header_analysis::sptr
    frame_header_analysis::make(int develop_mode, int block_id, int len_frame_type, int len_frame_index, int len_destination_address, int len_source_address, int len_num_transmission, int len_reserved_field_I, int len_reserved_field_II, int len_payload_length, int apply_address_check, int my_address)
    {
      return gnuradio::get_initial_sptr
        (new frame_header_analysis_impl(develop_mode, block_id, len_frame_type, len_frame_index, len_destination_address, len_source_address, len_num_transmission, len_reserved_field_I, len_reserved_field_II, len_payload_length, apply_address_check, my_address));
    }

    /*
     * The private constructor
     */
    frame_header_analysis_impl::frame_header_analysis_impl(int develop_mode, int block_id, int len_frame_type, int len_frame_index, int len_destination_address, int len_source_address, int len_num_transmission, int len_reserved_field_I, int len_reserved_field_II, int len_payload_length, int apply_address_check, int my_address)
      : gr::block("frame_header_analysis",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
        _block_id(block_id),
        _develop_mode(develop_mode),
        _len_frame_type(len_frame_type), // Bytes
        _len_frame_index(len_frame_index), // Bytes
        _len_destination_address(len_destination_address), // Bytes
        _len_source_address(len_source_address), // Bytes
        _len_num_transmission(len_num_transmission), // Bytes
        _len_reserved_field_I(len_reserved_field_I), // Bytes
        _len_reserved_field_II(len_reserved_field_II), // Bytes
        _len_payload_length(len_payload_length), // Bytes
        _apply_address_check(apply_address_check),
        _my_address(my_address)
    {
      if(_develop_mode == 1)
        std::cout << "develop_mode of frame_header_analysis ID: " << _block_id << " is activated." << std::endl;
      message_port_register_in(pmt::mp("frame_in"));
      message_port_register_out(pmt::mp("frame_info_out"));
      set_msg_handler(pmt::mp("frame_in"), boost::bind(&frame_header_analysis_impl::frame_analysis, this, _1 ));
    }

    /*
     * Our virtual destructor.
     */
    frame_header_analysis_impl::~frame_header_analysis_impl()
    {
    }

    void
    frame_header_analysis_impl::frame_analysis(pmt::pmt_t rx_frame)
    {
      if(_develop_mode == 1)
      {
        std::cout << "++++++++  frame_header_analysis ID: " << _block_id << "  +++++++" << std::endl;
      }
      if(pmt::is_pair(rx_frame)) 
      {
        bool good_frame;
        pmt::pmt_t meta = pmt::car(rx_frame);
        pmt::pmt_t frame_pmt = pmt::cdr(rx_frame);
        std::vector<unsigned char> frame_array; 
        if(pmt::is_u8vector(frame_pmt))
        {
          std::vector<unsigned char> frame_array = pmt::u8vector_elements(frame_pmt);
          int _frame_length = frame_array.size(); 
          /*
           * frame type check
           */
          int frame_type = static_cast<unsigned>(frame_array[0]);
          if(frame_type <= 5 && frame_type > 0)
          {
            pmt::pmt_t frame_info = frame_decompose(frame_pmt, frame_type);
            message_port_pub(pmt::mp("frame_info_out"), frame_info);
          }
          else if(frame_type == 8)
          {
	    // Create delimiter array
            int delimiter_length = get_ampdu_delimiter_length();
            std::vector<unsigned char> delimiter_array;
            delimiter_array.insert(delimiter_array.end(), frame_array.begin(), frame_array.begin() + delimiter_length);
	    // Get reserved field ampdu 
            int reserved_field_ampdu_pos = _len_frame_type;
	    std::vector<unsigned char> reserved_field_ampdu_array(delimiter_array.begin() + reserved_field_ampdu_pos, delimiter_array.begin() + reserved_field_ampdu_pos + _len_reserved_field_I);
	    disp_vec(reserved_field_ampdu_array);
            int reserved_field_ampdu = BytesToint(&reserved_field_ampdu_array);
	    std::cout << "reserved_field_ampdu is: " << reserved_field_ampdu << std::endl;
	    // Get mpdu length
            int mpdu_length_pos = reserved_field_ampdu_pos + _len_reserved_field_I;
	    std::vector<unsigned char> mpdu_length_array(delimiter_array.begin() + mpdu_length_pos, delimiter_array.begin() + mpdu_length_pos + _len_payload_length);
	    disp_vec(mpdu_length_array);
            int mpdu_length = BytesToint(&mpdu_length_array);
	    std::cout << "mpdu length is: " << mpdu_length << std::endl;
          }
        }
        else
          std::cout << "pmt is not a u8vector" << std::endl;
      }
      else 
        std::cout << "pmt is not a pair" << std::endl;
    }

    int
    frame_header_analysis_impl::get_frame_header_length()
    {
      return _len_frame_type + _len_frame_index + _len_destination_address + _len_source_address + _len_reserved_field_I + _len_reserved_field_II + _len_payload_length;
    } 

    int
    frame_header_analysis_impl::get_ampdu_delimiter_length()
    {
      return _len_frame_type + _len_reserved_field_I + _len_payload_length;
    } 

    int 
    frame_header_analysis_impl::BytesToint(std::vector<unsigned char> *bytes)
    {
      int int_num = 0;
      int size = bytes->size();
      for(int i = 0; i < size; ++i)  
      {
        int_num = static_cast<int>(bytes->back() << ((size - i - 1) * 8)) + int_num;
        bytes->pop_back();
      } 
      return int_num;
    }
    
    pmt::pmt_t
    frame_header_analysis_impl::frame_decompose(pmt::pmt_t frame_pmt, int frame_type)
    {
      std::vector<unsigned char> frame_array = pmt::u8vector_elements(frame_pmt);
      pmt::pmt_t frame_info;
      int header_length = get_frame_header_length();
      std::vector<unsigned char> frame_header_array;
      frame_header_array.insert(frame_header_array.end(), frame_array.begin(), frame_array.begin() + header_length);
      int frame_index = frame_header_array[_len_frame_type];
      int dest_pos = _len_frame_type + _len_frame_type;
      int destination_address = frame_header_array[dest_pos];
      int src_pos = dest_pos + _len_destination_address;
      int source_address = frame_header_array[src_pos];
      int ntr_pos = src_pos + _len_source_address;
      int num_transmission = frame_header_array[ntr_pos];
      int re_I_pos = src_pos + _len_num_transmission;
      int reserved_field_I = frame_header_array[re_I_pos]+frame_header_array[re_I_pos + 1];
      int re_II_pos = re_I_pos + _len_reserved_field_I;
      int reserved_field_II = frame_header_array[re_II_pos]+frame_header_array[re_II_pos + 1];
      int plen_pos = re_II_pos + _len_reserved_field_II;
      int payload_length = frame_header_array[plen_pos];
      int address_check = !(_apply_address_check);
      //std::cout << "block_id: " << _block_id << ", my address: " << _my_address << ", source_address: " << source_address << ", frame_index: " << frame_index <<  ", frame_type: " << frame_type <<  ", destination_address: " << destination_address <<  std::endl;
      int is_good_frame = 0;

      if(_develop_mode == 1)
      {
        std::cout << "length of frame_header_array is: " << frame_header_array.size() << std::endl;
        std::cout << "frame type is: " << frame_type << std::endl;
        std::cout << "frame index is: " << frame_index << std::endl;
        std::cout << "destination address is: " << destination_address << std::endl;
        std::cout << "source address is: " << source_address << std::endl;
        std::cout << "number of transmission is: " << num_transmission << std::endl;
        std::cout << "reserved field I is: " << reserved_field_I << std::endl;
        std::cout << "reserved field II is: " << reserved_field_II << std::endl;
        std::cout << "payload length is: " << payload_length << std::endl;
        std::cout << "frame header length is: " << header_length << std::endl;
        std::cout << "address check is initialized to: " << address_check << std::endl;
        std::cout << "frame verification (good_frame) is initialized to: " << is_good_frame << std::endl;
      }
      // Get frame index
      int index_pos = _len_frame_type;
      std::vector<unsigned char> frame_index_array(frame_header_array.begin() + index_pos, frame_header_array.begin() + index_pos + _len_frame_index);
      disp_vec(frame_index_array);
      frame_index = BytesToint(&frame_index_array);
      std::cout << "new frame index is: " << frame_index << std::endl;

      // Get destination address
      dest_pos = index_pos + _len_frame_index;
      std::vector<unsigned char> dest_array(frame_header_array.begin() + dest_pos, frame_header_array.begin() + dest_pos + _len_destination_address);
      disp_vec(dest_array);
      destination_address = BytesToint(&dest_array);
      std::cout << "new destination address is: " << destination_address << std::endl;
      
      // Get source address
      src_pos = dest_pos + _len_destination_address;
      std::vector<unsigned char> src_array(frame_header_array.begin() + src_pos, frame_header_array.begin() + src_pos + _len_source_address);
      disp_vec(src_array);
      source_address = BytesToint(&src_array);
      std::cout << "new source address is: " << source_address << std::endl;

      // Get number of transmissions
      int ntrans_pos = src_pos + _len_source_address;
      std::vector<unsigned char> ntrans_array(frame_header_array.begin() + ntrans_pos, frame_header_array.begin() + ntrans_pos + _len_num_transmission);
      disp_vec(ntrans_array);
      num_transmission = BytesToint(&ntrans_array);
      std::cout << "new number of transmission is: " << num_transmission << std::endl;
      
      // Get reserved field I
      re_I_pos = ntrans_pos + _len_num_transmission;
      std::vector<unsigned char> re_I_array(frame_header_array.begin() + re_I_pos, frame_header_array.begin() + re_I_pos + _len_reserved_field_I);
      disp_vec(re_I_array);
      reserved_field_I = BytesToint(&re_I_array);
      std::cout << "new reserved field I is: " << reserved_field_I << std::endl;
      
      // Get reserved field II
      re_II_pos = re_I_pos + _len_reserved_field_I;
      std::vector<unsigned char> re_II_array(frame_header_array.begin() + re_II_pos, frame_header_array.begin() + re_II_pos + _len_reserved_field_II);
      disp_vec(re_II_array);
      reserved_field_II = BytesToint(&re_II_array);
      std::cout << "new reserved field II is: " << reserved_field_II << std::endl;
      
      // Get payload length
      int payload_length_pos = re_II_pos + _len_reserved_field_II;
      std::vector<unsigned char> payload_length_array(frame_header_array.begin() + payload_length_pos, frame_header_array.begin() + payload_length_pos + _len_payload_length);
      disp_vec(payload_length_array);
      payload_length = BytesToint(&payload_length_array);
      std::cout << "new payload length is: " << payload_length << std::endl;
      
      // Get payload
      int payload_pos = payload_length_pos + _len_payload_length;
      std::vector<unsigned char> payload_array(frame_array.begin() + payload_pos, frame_array.begin() + payload_pos + payload_length);
      std::cout << "payload is: " << std::endl;
      //disp_vec(payload_array);
      
      // Get CRC
      int crc_pos = payload_pos + payload_length;
      std::vector<unsigned char> crc_array(frame_array.begin() + crc_pos, frame_array.begin() + crc_pos + 4);
      std::cout << "crc is: " << std::endl;
      disp_vec(crc_array);

      if(source_address != _my_address)
      {
        pmt::pmt_t meta = pmt::make_dict();
        pmt::pmt_t frame_header_array_u8vector = pmt::init_u8vector(header_length, frame_header_array);
        pmt::pmt_t frame_header_array_pmt = pmt::cons(meta, frame_header_array_u8vector);
      
        pmt::pmt_t frame_info  = pmt::make_dict();
        frame_info  = pmt::dict_add(frame_info, pmt::string_to_symbol("frame_type"), pmt::from_long(frame_type));
        frame_info  = pmt::dict_add(frame_info, pmt::string_to_symbol("frame_index"), pmt::from_long(frame_index));
        frame_info  = pmt::dict_add(frame_info, pmt::string_to_symbol("destination_address"), pmt::from_long(destination_address));
        frame_info  = pmt::dict_add(frame_info, pmt::string_to_symbol("source_address"), pmt::from_long(source_address));
        frame_info  = pmt::dict_add(frame_info, pmt::string_to_symbol("num_transmission"), pmt::from_long(num_transmission));
        frame_info  = pmt::dict_add(frame_info, pmt::string_to_symbol("reserved_field_I"), pmt::from_long(reserved_field_I));
        frame_info  = pmt::dict_add(frame_info, pmt::string_to_symbol("reserved_field_II"), pmt::from_long(reserved_field_II));
        frame_info  = pmt::dict_add(frame_info, pmt::string_to_symbol("payload_length"), pmt::from_long(payload_length));
        frame_info  = pmt::dict_add(frame_info, pmt::string_to_symbol("header_length"), pmt::from_long(header_length));
        frame_info  = pmt::dict_add(frame_info, pmt::string_to_symbol("address_check"),pmt::from_long(address_check));
        frame_info  = pmt::dict_add(frame_info, pmt::string_to_symbol("good_frame"),pmt::from_long(is_good_frame));

        frame_info  = pmt::dict_add(frame_info, pmt::string_to_symbol("frame_pmt"), pmt::cons(meta, frame_pmt));
        if(_develop_mode == 2)
        {
          struct timeval t; 
          gettimeofday(&t, NULL);
          double current_time = t.tv_sec - double(int(t.tv_sec/100)*100) + t.tv_usec / 1000000.0;
          if(frame_type == 1)
            std::cout << "* header analysis ID: " << _block_id << " get the " << num_transmission <<"th transmission of data frame "<< frame_index << " at time " << current_time << " s" << std::endl;
          else
            std::cout << "* header analysis ID: " << _block_id << " get the ack frame of the " << num_transmission <<"th transmission of data frame "<< frame_index << " at time " << current_time << " s" << std::endl;
        }
	return frame_info;
      } 
    }

    void 
    frame_header_analysis_impl::disp_vec(std::vector<unsigned char> vec)
    {
      for(int i=0; i<vec.size(); ++i)
        std::cout << static_cast<unsigned>(vec[i]) << ' ';
      std::cout << ". total length is: " << vec.size() << std::endl;
    }

  } /* namespace inets */
} /* namespace gr */

