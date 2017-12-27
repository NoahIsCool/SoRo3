# things for GPS comunication
# make sure to run pip install sbp
from sbp.client.drivers.network_drivers import TCPDriver
from sbp.client import Handler, Framer
from sbp.mag import MsgMagRaw
from sbp.client.drivers.pyserial_driver import PySerialDriver

driver = TCPDriver('192.168.1.222', '55555')


with Handler(Framer(driver.read, None, verbose=True)) as source:
	# reads all the messages in a loop as they are received
        for msg, metadata in source.filter(SBP_MSG_MAG_RAW):
           print msg.mag_x