from serial.tools import list_ports
import serial
import time

list_ports.main()  # list available ports


class Plotter:
    def __init__(self):
        self.ser = serial.Serial('/dev/cu.usbmodem14101', 9600)
        time.sleep(2)
    
    def close(self):
        self.ser.close()
    
    def pen_up(self):
        self.ser.write('pu\n'.encode())
    
    def pen_down(self):
        self.ser.write('pd\n'.encode())
    
    def move(self, s, t):
        self.ser.write('m {} {}\n'.format(s, t).encode())
