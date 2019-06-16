from serial.tools import list_ports
import serial
import time


class PlotterController:
    def __init__(self, port_regex=r'(/dev/cu.usbmodem)'):
        available_ports = list(list_ports.grep(port_regex))

        while len(available_ports) == 0:
            input("No ports detected that match r'(/dev/cu.usbmodem)'. Check the connection, then press Enter")
        if len(available_ports) == 1:
            # print(available_ports[0].device)
            self.ser = serial.Serial(available_ports[0].device, 9600)
            # Wait a bit for the Arduino to get a nice connection (;
        else:
            print(available_ports)
            port = input("Which port?: ")
            self.ser = serial.Serial(port, 9600)
        time.sleep(2)

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
