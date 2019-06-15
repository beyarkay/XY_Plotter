from serial.tools import list_ports
import serial
import time


class PlotterController:
    def __init__(self, port_regex='/dev/cu.usbmodem14101'):
        available_ports = list_ports.grep(port_regex)
        if len(available_ports) == 1:
            self.ser = serial.Serial(available_ports[0], 9600)
            # Wait a bit for the Arduino to get a nice connection (;
            time.sleep(2)
        else:


    def close(self):
        self.ser.close()

    def pen_up(self):
        self.ser.write('pu\n'.encode())

    def pen_down(self):
        self.ser.write('pd\n'.encode())

    def move(self, s, t):
        """
        :param s: amount to change the length of the left string. Negative moves the plotter up
        :param t: amount to change the length of the right string. Negative moves the plotter up
        :return:
        """
        self.ser.write('m {} {}\n'.format(s, t).encode())
