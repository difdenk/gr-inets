#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Test_idle
# Author: PWA
# Generated: Wed Dec 14 13:32:04 2016
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import inets
import pmt
import sys
from gnuradio import qtgui


class Test_idle(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Test_idle")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Test_idle")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "Test_idle")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Variables
        ##################################################
        self.source_address = source_address = 1
        self.self_data_info = self_data_info = pmt.to_pmt({'frame_type': 1, 'frame_index': 1, 'destination_address': 2, 'source_address': 1, 'num_resend': 3, 'reserved_field_I': 1, 'reserved_field_II': 1, 'pay_load_length': 200})
        self.samp_rate = samp_rate = 320000
        self.reserved_field_II = reserved_field_II = 6
        self.reserved_field_I = reserved_field_I = 5
        self.max_num_retransmission = max_num_retransmission = 5
        self.max_buffer_size = max_buffer_size = 10
        self.len_source_address = len_source_address = 1
        self.len_reserved_field_II = len_reserved_field_II = 2
        self.len_reserved_field_I = len_reserved_field_I = 2
        self.len_payload_length = len_payload_length = 1
        self.len_num_transmission = len_num_transmission = 1
        self.len_frame_type = len_frame_type = 1
        self.len_frame_index = len_frame_index = 1
        self.len_destination_address = len_destination_address = 1
        self.increase_index = increase_index = 1
        self.frame_type = frame_type = 1
        self.frame_index = frame_index = 0
        self.experiment_duration_s = experiment_duration_s = 1000
        self.develop_mode = develop_mode = 2
        self.destination_address = destination_address = 3
        self.another_data_info = another_data_info = pmt.to_pmt({'frame_type': 1, 'frame_index': 1, 'destination_address': 1, 'source_address': 2, 'num_resend': 3, 'reserved_field_I': 1, 'reserved_field_II': 1, 'pay_load_length': 200})
        self.another_ack_info = another_ack_info = pmt.to_pmt({'frame_type': 2, 'frame_index': 1, 'destination_address': 1, 'source_address': 2, 'num_resend': 0, 'reserved_field_I': 1, 'reserved_field_II': 1, 'pay_load_length': 0})

        ##################################################
        # Blocks
        ##################################################
        self.inets_null_message_source_0 = inets.null_message_source()
        self.inets_message_tomb_0 = inets.message_tomb()
        self.inets_idle_0 = inets.idle(2, 1, experiment_duration_s, max_num_retransmission, max_buffer_size, frame_type, len_frame_type, frame_index, len_frame_index, destination_address, len_destination_address, source_address, len_source_address, reserved_field_I, len_reserved_field_I, reserved_field_II, len_reserved_field_II, len_payload_length, increase_index, len_num_transmission)
        self.blocks_socket_pdu_0 = blocks.socket_pdu("UDP_SERVER", 'localhost', '52001', 10000, False)
        self.blocks_message_strobe_random_0_0_0 = blocks.message_strobe_random(pmt.from_bool(True), blocks.STROBE_POISSON, 2000, 0)
        self.blocks_message_strobe_random_0_0 = blocks.message_strobe_random(another_ack_info, blocks.STROBE_POISSON, 2000, 0)
        self.blocks_message_debug_0 = blocks.message_debug()

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_socket_pdu_0, 'pdus'), (self.inets_idle_0, 'data_in'))    
        self.msg_connect((self.inets_idle_0, 'data_out'), (self.blocks_message_debug_0, 'print'))    
        self.msg_connect((self.inets_idle_0, 'successful_transmission'), (self.inets_message_tomb_0, 'message_in'))    
        self.msg_connect((self.inets_null_message_source_0, 'null_message_out'), (self.inets_idle_0, 'reset_idle'))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "Test_idle")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_source_address(self):
        return self.source_address

    def set_source_address(self, source_address):
        self.source_address = source_address

    def get_self_data_info(self):
        return self.self_data_info

    def set_self_data_info(self, self_data_info):
        self.self_data_info = self_data_info

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

    def get_reserved_field_II(self):
        return self.reserved_field_II

    def set_reserved_field_II(self, reserved_field_II):
        self.reserved_field_II = reserved_field_II

    def get_reserved_field_I(self):
        return self.reserved_field_I

    def set_reserved_field_I(self, reserved_field_I):
        self.reserved_field_I = reserved_field_I

    def get_max_num_retransmission(self):
        return self.max_num_retransmission

    def set_max_num_retransmission(self, max_num_retransmission):
        self.max_num_retransmission = max_num_retransmission

    def get_max_buffer_size(self):
        return self.max_buffer_size

    def set_max_buffer_size(self, max_buffer_size):
        self.max_buffer_size = max_buffer_size

    def get_len_source_address(self):
        return self.len_source_address

    def set_len_source_address(self, len_source_address):
        self.len_source_address = len_source_address

    def get_len_reserved_field_II(self):
        return self.len_reserved_field_II

    def set_len_reserved_field_II(self, len_reserved_field_II):
        self.len_reserved_field_II = len_reserved_field_II

    def get_len_reserved_field_I(self):
        return self.len_reserved_field_I

    def set_len_reserved_field_I(self, len_reserved_field_I):
        self.len_reserved_field_I = len_reserved_field_I

    def get_len_payload_length(self):
        return self.len_payload_length

    def set_len_payload_length(self, len_payload_length):
        self.len_payload_length = len_payload_length

    def get_len_num_transmission(self):
        return self.len_num_transmission

    def set_len_num_transmission(self, len_num_transmission):
        self.len_num_transmission = len_num_transmission

    def get_len_frame_type(self):
        return self.len_frame_type

    def set_len_frame_type(self, len_frame_type):
        self.len_frame_type = len_frame_type

    def get_len_frame_index(self):
        return self.len_frame_index

    def set_len_frame_index(self, len_frame_index):
        self.len_frame_index = len_frame_index

    def get_len_destination_address(self):
        return self.len_destination_address

    def set_len_destination_address(self, len_destination_address):
        self.len_destination_address = len_destination_address

    def get_increase_index(self):
        return self.increase_index

    def set_increase_index(self, increase_index):
        self.increase_index = increase_index

    def get_frame_type(self):
        return self.frame_type

    def set_frame_type(self, frame_type):
        self.frame_type = frame_type

    def get_frame_index(self):
        return self.frame_index

    def set_frame_index(self, frame_index):
        self.frame_index = frame_index

    def get_experiment_duration_s(self):
        return self.experiment_duration_s

    def set_experiment_duration_s(self, experiment_duration_s):
        self.experiment_duration_s = experiment_duration_s

    def get_develop_mode(self):
        return self.develop_mode

    def set_develop_mode(self, develop_mode):
        self.develop_mode = develop_mode

    def get_destination_address(self):
        return self.destination_address

    def set_destination_address(self, destination_address):
        self.destination_address = destination_address

    def get_another_data_info(self):
        return self.another_data_info

    def set_another_data_info(self, another_data_info):
        self.another_data_info = another_data_info

    def get_another_ack_info(self):
        return self.another_ack_info

    def set_another_ack_info(self, another_ack_info):
        self.another_ack_info = another_ack_info
        self.blocks_message_strobe_random_0_0.set_msg(self.another_ack_info)


def main(top_block_cls=Test_idle, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
