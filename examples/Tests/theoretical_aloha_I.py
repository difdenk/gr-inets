#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: theoretical_aloha_I
# Author: PWA
# Generated: Fri Sep  8 13:49:17 2017
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

import os
import sys
sys.path.append(os.environ.get('GRC_HIER_PATH', os.path.expanduser('~/.grc_gnuradio')))

from PyQt4 import Qt
from directional_sending import directional_sending  # grc-generated hier_block
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.qtgui import Range, RangeWidget
from optparse import OptionParser
import gnuradio
import inets
from gnuradio import qtgui


class theoretical_aloha_I(gr.top_block, Qt.QWidget):

    def __init__(self, phase=0, sweep_mode=0):
        gr.top_block.__init__(self, "theoretical_aloha_I")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("theoretical_aloha_I")
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

        self.settings = Qt.QSettings("GNU Radio", "theoretical_aloha_I")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Parameters
        ##################################################
        self.phase = phase
        self.sweep_mode = sweep_mode

        ##################################################
        # Variables
        ##################################################
        self.sps = sps = 4
        self.range_rx_gain = range_rx_gain = 0
        self.range_mu = range_mu = 0.6
        self.usrp_device_address = usrp_device_address = "addr0=192.168.10.2, addr1=192.168.10.3"
        self.tx_gain = tx_gain = 10
        self.tx_center_frequency = tx_center_frequency = 3.76e9
        self.timeout_duration_ms = timeout_duration_ms = 1000
        self.system_time_granularity_us = system_time_granularity_us = 10
        self.source_address = source_address = 1
        self.samp_rate = samp_rate = 400000
        self.rx_gain = rx_gain = range_rx_gain
        self.rx_center_frequency = rx_center_frequency = 3.76e9

        self.rrc = rrc = firdes.root_raised_cosine(1.0, sps, 1, 0.5, 11*sps)

        self.mu = mu = range_mu
        self.diff_preamble_128 = diff_preamble_128 = [1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0,0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0,0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1,1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0][0:128]
        self.destination_address = destination_address = 3
        self.cs_threshold = cs_threshold = 0.005

        ##################################################
        # Blocks
        ##################################################
        self._range_rx_gain_range = Range(0, 60, 1, 0, 200)
        self._range_rx_gain_win = RangeWidget(self._range_rx_gain_range, self.set_range_rx_gain, 'Rx Gain', "counter_slider", float)
        self.top_grid_layout.addWidget(self._range_rx_gain_win, 1,0,1,1)
        self._range_mu_range = Range(0, 1, 0.01, 0.6, 200)
        self._range_mu_win = RangeWidget(self._range_mu_range, self.set_range_mu, 'BB Derotation Gain', "counter_slider", float)
        self.top_grid_layout.addWidget(self._range_mu_win, 2,0,1,1)
        self.inets_run_0 = inets.run(5, 10)
        self.inets_general_timer_0_0 = inets.general_timer(0, 1, 0, 500000, 10, 0)
        self.inets_general_timer_0 = inets.general_timer(0, 3, 3, 67, 10, 0)
        self.inets_framing_0 = inets.framing(0, 17, 2, 1, 0, 1, destination_address, 1, source_address, 1, 318, 2, 524, 2, 2, 1, 1, 0, ([2, 3]), ([1000, 1000]), 2, 0, 300, 1)
        self.inets_frame_path_1 = inets.frame_path(0, 39)
        self.inets_frame_path_0 = inets.frame_path(0, 39)
        self.inets_frame_buffer_0 = inets.frame_buffer(0, 16, 1000, 1, 1, 0)
        self.inets_dummy_source_0 = inets.dummy_source(0, 23, 837, 3, 1)
        self.inets_counter_0_0_0 = inets.counter(0, 1, 1, "", 0, "/home/inets/source/gr-inets/results/", 1)
        self.inets_counter_0 = inets.counter(1, 2, 1, "sent", 0, "/home/inets/source/gr-inets/results/", 1)
        self.directional_sending_0 = directional_sending(
            block_id=11,
            center_frequency=tx_center_frequency,
            constellation=gnuradio.digital.constellation_qpsk().base(),
            develop_mode=1,
            file_name_extension_pending="Tfr",
            file_name_extension_t_control="t1TXs",
            interframe_interval_s=0,
            name_with_timestamp=0,
            phase=phase,
            preamble=diff_preamble_128,
            record_on=0,
            samp_rate=samp_rate,
            sps=sps,
            sweep_mode=sweep_mode,
            system_time_granularity_us=system_time_granularity_us,
            t_pretx_interval_s=0,
            tx_gain=tx_gain,
            usrp_device_address=usrp_device_address,
        )

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.directional_sending_0, 'ack_frame_out'), (self.inets_counter_0, 'message_in'))
        self.msg_connect((self.directional_sending_0, 'ack_frame_out'), (self.inets_frame_path_0, 'frame_in'))
        self.msg_connect((self.inets_dummy_source_0, 'output'), (self.inets_framing_0, 'data_in'))
        self.msg_connect((self.inets_frame_buffer_0, 'dequeue_element'), (self.directional_sending_0, 'in'))
        self.msg_connect((self.inets_frame_path_0, 'frame_out'), (self.inets_frame_buffer_0, 'dequeue'))
        self.msg_connect((self.inets_frame_path_1, 'frame_out'), (self.inets_general_timer_0, 'active_in'))
        self.msg_connect((self.inets_framing_0, 'frame_out'), (self.inets_counter_0_0_0, 'message_in'))
        self.msg_connect((self.inets_framing_0, 'frame_out'), (self.inets_frame_buffer_0, 'enqueue'))
        self.msg_connect((self.inets_general_timer_0, 'expire_signal_out'), (self.inets_dummy_source_0, 'trigger'))
        self.msg_connect((self.inets_general_timer_0, 'expire_signal_out'), (self.inets_frame_path_1, 'frame_in'))
        self.msg_connect((self.inets_general_timer_0_0, 'expire_signal_out'), (self.inets_general_timer_0, 'disable_timer_in'))
        self.msg_connect((self.inets_run_0, 'trigger_out'), (self.inets_general_timer_0, 'active_in'))
        self.msg_connect((self.inets_run_0, 'trigger_out'), (self.inets_general_timer_0_0, 'active_in'))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "theoretical_aloha_I")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_phase(self):
        return self.phase

    def set_phase(self, phase):
        self.phase = phase
        self.directional_sending_0.set_phase(self.phase)

    def get_sweep_mode(self):
        return self.sweep_mode

    def set_sweep_mode(self, sweep_mode):
        self.sweep_mode = sweep_mode
        self.directional_sending_0.set_sweep_mode(self.sweep_mode)

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps
        self.directional_sending_0.set_sps(self.sps)

    def get_range_rx_gain(self):
        return self.range_rx_gain

    def set_range_rx_gain(self, range_rx_gain):
        self.range_rx_gain = range_rx_gain
        self.set_rx_gain(self.range_rx_gain)

    def get_range_mu(self):
        return self.range_mu

    def set_range_mu(self, range_mu):
        self.range_mu = range_mu
        self.set_mu(self.range_mu)

    def get_usrp_device_address(self):
        return self.usrp_device_address

    def set_usrp_device_address(self, usrp_device_address):
        self.usrp_device_address = usrp_device_address
        self.directional_sending_0.set_usrp_device_address(self.usrp_device_address)

    def get_tx_gain(self):
        return self.tx_gain

    def set_tx_gain(self, tx_gain):
        self.tx_gain = tx_gain
        self.directional_sending_0.set_tx_gain(self.tx_gain)

    def get_tx_center_frequency(self):
        return self.tx_center_frequency

    def set_tx_center_frequency(self, tx_center_frequency):
        self.tx_center_frequency = tx_center_frequency
        self.directional_sending_0.set_center_frequency(self.tx_center_frequency)

    def get_timeout_duration_ms(self):
        return self.timeout_duration_ms

    def set_timeout_duration_ms(self, timeout_duration_ms):
        self.timeout_duration_ms = timeout_duration_ms

    def get_system_time_granularity_us(self):
        return self.system_time_granularity_us

    def set_system_time_granularity_us(self, system_time_granularity_us):
        self.system_time_granularity_us = system_time_granularity_us
        self.directional_sending_0.set_system_time_granularity_us(self.system_time_granularity_us)

    def get_source_address(self):
        return self.source_address

    def set_source_address(self, source_address):
        self.source_address = source_address

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.directional_sending_0.set_samp_rate(self.samp_rate)

    def get_rx_gain(self):
        return self.rx_gain

    def set_rx_gain(self, rx_gain):
        self.rx_gain = rx_gain

    def get_rx_center_frequency(self):
        return self.rx_center_frequency

    def set_rx_center_frequency(self, rx_center_frequency):
        self.rx_center_frequency = rx_center_frequency

    def get_rrc(self):
        return self.rrc

    def set_rrc(self, rrc):
        self.rrc = rrc

    def get_mu(self):
        return self.mu

    def set_mu(self, mu):
        self.mu = mu

    def get_diff_preamble_128(self):
        return self.diff_preamble_128

    def set_diff_preamble_128(self, diff_preamble_128):
        self.diff_preamble_128 = diff_preamble_128
        self.directional_sending_0.set_preamble(self.diff_preamble_128)

    def get_destination_address(self):
        return self.destination_address

    def set_destination_address(self, destination_address):
        self.destination_address = destination_address

    def get_cs_threshold(self):
        return self.cs_threshold

    def set_cs_threshold(self, cs_threshold):
        self.cs_threshold = cs_threshold


def argument_parser():
    parser = OptionParser(usage="%prog: [options]", option_class=eng_option)
    return parser


def main(top_block_cls=theoretical_aloha_I, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

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
