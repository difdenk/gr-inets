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
#include "frame_check_impl.h"
#include <gnuradio/digital/crc32.h>
#include <volk/volk.h>
#include <boost/crc.hpp>

namespace gr {
  namespace inets {

    frame_check::sptr
    frame_check::make(int develop_mode, int block_id)
    {
      return gnuradio::get_initial_sptr
        (new frame_check_impl(develop_mode, block_id));
    }

    /*
     * The private constructor
     */
    frame_check_impl::frame_check_impl(int develop_mode, int block_id)
      : gr::block("frame_check",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
       _block_id(block_id),
       _develop_mode(develop_mode),
       _reference_payload(generate()),
       _error(0),
       _success(0)
    {
      if(_develop_mode)
        std::cout << "develop_mode of frame_check ID: " << _block_id << " is activated." << std::endl;
      message_port_register_in(pmt::mp("frame_info_in"));
      message_port_register_out(pmt::mp("good_frame_info_out"));
      message_port_register_out(pmt::mp("bad_frame_info_out"));
      message_port_register_out(pmt::mp("payload_out"));
      set_msg_handler(pmt::mp("frame_info_in"), boost::bind(&frame_check_impl::check_frame, this, _1 ));
    }

    /*
     * Our virtual destructor.
     */
    frame_check_impl::~frame_check_impl()
    {
    }

    void
    frame_check_impl::check_frame(pmt::pmt_t frame_info)
    {
      if(_develop_mode == 1)
      {
        std::cout << "++++++  frame_check ID: " << _block_id << "  ++++++++" << std::endl;
      }
      if(pmt::dict_has_key(frame_info, pmt::string_to_symbol("frame_pmt")))
      {
        int is_good_frame;
        pmt::pmt_t not_found;
	/*
        // frame_type
        pmt::pmt_t frame_type_pmt = pmt::dict_ref(frame_info, pmt::string_to_symbol("frame_type"), not_found);
        int frame_type = pmt::to_long(frame_type_pmt);
        // frame_index
        pmt::pmt_t frame_index_pmt = pmt::dict_ref(frame_info, pmt::string_to_symbol("frame_index"), not_found);
        int frame_index = pmt::to_long(frame_index_pmt);
        // destination_address
        pmt::pmt_t destination_address_pmt = pmt::dict_ref(frame_info, pmt::string_to_symbol("destination_address"), not_found);
        int destination_address = pmt::to_long(destination_address_pmt);
        // source_address
        pmt::pmt_t source_address_pmt = pmt::dict_ref(frame_info, pmt::string_to_symbol("source_address"), not_found);
        int source_address = pmt::to_long(source_address_pmt);
        // payload_length
        pmt::pmt_t payload_length_pmt = pmt::dict_ref(frame_info, pmt::string_to_symbol("payload_length"), not_found);
        int payload_length = pmt::to_long(payload_length_pmt);
        // rx_frame_address_check
        pmt::pmt_t rx_frame_address_check_pmt = pmt::dict_ref(frame_info, pmt::string_to_symbol("address_check"), not_found);
        int rx_frame_address_check = pmt::to_long(rx_frame_address_check_pmt);
	*/
        /*
         * CRC
         */
        // rx_frame with crc

        // header_length
        pmt::pmt_t header_length_pmt = pmt::dict_ref(frame_info, pmt::string_to_symbol("header_length"), not_found);
        int header_length = pmt::to_long(header_length_pmt);

        pmt::pmt_t frame_pmt = pmt::dict_ref(frame_info, pmt::string_to_symbol("frame_pmt"), not_found);
        // rx_frame without crc in vector form
        std::vector<unsigned char> frame_array = pmt::u8vector_elements(pmt::cdr(frame_pmt));
        std::vector<unsigned char> frame_for_recrc_vector;
        frame_for_recrc_vector.insert(frame_for_recrc_vector.end(), frame_array.begin(), frame_array.end() - 4);
        std::vector<unsigned char> payload_vector;
        payload_vector.insert(payload_vector.end(), frame_for_recrc_vector.begin() + header_length, frame_for_recrc_vector.end());
        pmt::pmt_t frame_for_recrc_pmt = pmt::init_u8vector(frame_for_recrc_vector.size(), frame_for_recrc_vector);
        pmt::pmt_t meta = pmt::make_dict();
        pmt::pmt_t frame_before_recrc_pmt = pmt::cons(meta, frame_for_recrc_pmt);
        pmt::pmt_t frame_after_recrc_pmt = crc32_bb_calc(frame_before_recrc_pmt);
        std::vector<unsigned char> frame_after_recrc_vector = pmt::u8vector_elements(pmt::cdr(frame_after_recrc_pmt));
        is_good_frame = (frame_after_recrc_vector == frame_array);
        // BER Calculation//
        ber_calculate(payload_vector);
        if(_develop_mode == 2)
        {
          struct timeval t;
          gettimeofday(&t, NULL);
          double current_time = t.tv_sec - double(int(t.tv_sec/100)*100) + t.tv_usec / 1000000.0;
          std::cout << "* frame verification ID: " << _block_id << " verifies frame at time " << current_time << " s" << std::endl;
        }
        if(_develop_mode != 3 && _develop_mode)
        {
          std::cout << "received frame with crc32 bytes (last four): ";
	        disp_vec(frame_array);
          std::cout << "frame with recalculated crc32 bytes : ";
	        disp_vec(frame_after_recrc_vector);
		/*
          std::cout << "dict has key frame_type: " << pmt::dict_has_key(frame_info, pmt::string_to_symbol("frame_type")) << " with value: " << frame_type << std::endl;
          std::cout << "dict has key frame_index: " << pmt::dict_has_key(frame_info, pmt::string_to_symbol("frame_index")) << " with value: " << frame_index << std::endl;
          std::cout << "dict has key destination_address: " << pmt::dict_has_key(frame_info, pmt::string_to_symbol("destination_address")) << " with value: " << destination_address << std::endl;
          std::cout << "dict has key source address: " << pmt::dict_has_key(frame_info, pmt::string_to_symbol("source_address")) << " with value: " << source_address << std::endl;
          std::cout << "dict has key payload_length: " << pmt::dict_has_key(frame_info, pmt::string_to_symbol("payload_length")) << " with value: " << payload_length << std::endl;
          std::cout << "dict has key header_length: " << pmt::dict_has_key(frame_info, pmt::string_to_symbol("header_length")) << " with value: " << header_length << std::endl;
          std::cout << "address check is: " << rx_frame_address_check << ", (1: passed, 0: failed)" << std::endl;
	  */
          std::cout << "Frame verification result: " << is_good_frame << ", (1: passed, 0: failed)" << std::endl;
        }
        pmt::pmt_t payload_u8vector = pmt::init_u8vector(payload_vector.size(), payload_vector);
        pmt::pmt_t payload = pmt::cons(meta, payload_u8vector);
        frame_info = pmt::dict_delete(frame_info, pmt::string_to_symbol("good_frame"));
        frame_info = pmt::dict_add(frame_info, pmt::string_to_symbol("good_frame"), pmt::from_long(is_good_frame));
	      if(is_good_frame)
	      {
          message_port_pub(pmt::mp("good_frame_info_out"), frame_info);
          message_port_pub(pmt::mp("payload_out"), payload);
	      }
        else
          message_port_pub(pmt::mp("bad_frame_info_out"), frame_info);
        }
        else
          std::cout << "pmt is not a dict" << std::endl;
    }
    pmt::pmt_t frame_check_impl::crc32_bb_calc(pmt::pmt_t msg)
    {
      // extract input pdu
      pmt::pmt_t meta(pmt::car(msg));
      pmt::pmt_t bytes(pmt::cdr(msg));

      unsigned int crc;
      size_t pkt_len(0);
      const uint8_t* bytes_in = pmt::u8vector_elements(bytes, pkt_len);
      uint8_t* bytes_out = (uint8_t*)volk_malloc(4 + pkt_len*sizeof(uint8_t),
                                                 volk_get_alignment());

      _crc_impl.reset();
      _crc_impl.process_bytes(bytes_in, pkt_len);
      crc = _crc_impl();
      memcpy((void*)bytes_out, (const void*)bytes_in, pkt_len);
      memcpy((void*)(bytes_out + pkt_len), &crc, 4); // FIXME big-endian/little-endian, this might be wrong

      pmt::pmt_t output = pmt::init_u8vector(pkt_len+4, bytes_out); // this copies the values from bytes_out into the u8vector
      return pmt::cons(meta, output);
    }

    void
    frame_check_impl::disp_vec(std::vector<unsigned char> vec)
    {
      for(int i=0; i<vec.size(); ++i)
        std::cout << static_cast<unsigned>(vec[i]) << ' ';
      std::cout << ". total length is: " << vec.size() << std::endl;
    }

    void frame_check_impl::ber_calculate(std::vector<unsigned char> payload_vector) {
      std::vector<uint8_t> received_payload = payload_vector;
      for (size_t i = 0; i < received_payload.size(); i++) {
        if (received_payload[i] == _reference_payload[i]) {
          _success++;
        }
        else
          _error++;
      }
      double total_bits_received = _error + _success;
      double error = _error;
      double BER = error/total_bits_received;
      if (_develop_mode == 3) {
        std::cout << "Total number of bits Received: " << total_bits_received << '\n';
        std::cout << "Erronous Bits: " << _error << '\n';
        std::cout << "BER: " << BER << '\n';
      }
    }

    std::vector<uint8_t> frame_check_impl::generate() {
      uint8_t start_state = 0x01;  /* Any nonzero start state will work. */
      uint8_t lfsr = start_state;
      uint8_t bit;
      std::vector<uint8_t> payload;
      payload.push_back(start_state);
      do
      {
        bit  = ((lfsr >> 0) ^ (lfsr >> 1)) & 1; // Polynomial is x^7 + x^6 + 1
        lfsr =  (lfsr >> 1) | (bit << 6);
        payload.push_back(lfsr);
      } while (lfsr != start_state);
      return payload;
      }

  } /* namespace inets */
} /* namespace gr */
