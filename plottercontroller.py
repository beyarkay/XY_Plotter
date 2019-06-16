from serial.tools import list_ports
import serial
import time


class PlotterController:
    """
    By default:
        * Stepper motor 1 is named S, and is on the LEFT
        * Stepper motor 2 is named T, and is on the RIGHT
        * Both steppers will wind in clockwise, and wind out counter-clockwise
        * Decreasing the s or t values will decrease the length of the string, winding the module upwards
    """
    def __init__(self, port_regex=r'(/dev/cu.usbmodem)'):
        available_ports = list(list_ports.grep(port_regex))

        while len(available_ports) == 0:
            input("No ports detected that match r'(/dev/cu.usbmodem)'. Check the connection, then press Enter")
            available_ports = list(list_ports.grep(port_regex))
        if len(available_ports) == 1:
            self.ser = serial.Serial(available_ports[0].device, 9600)
        else:
            print(available_ports)
            port = input("Which port?: ")
            self.ser = serial.Serial(port, 9600)
        # Wait a bit for the Arduino to get a solid serial connection
        time.sleep(2)

    def close(self):
        """
        Close the serial connection, freeing up resources
        """
        self.ser.close()

    def pen_up(self):
        """
        Move the pen up
        """
        self.ser.write('pu\n'.encode())

    def pen_down(self):
        """
        Move the pen down
        """
        self.ser.write('pd\n'.encode())

    def move(self, s, t):
        """
        :param s: amount to change the length of the left string.
        :param t: amount to change the length of the right string.
        :return:
        """
        self.ser.write('m {} {}\n'.format(s, t).encode())

    def turn_off(self):
        """
        Cut power to all the coils in both steppers
        The steppers will NOT keep their position in this state
        No special command is required to wake them up, just send through a move command
        The servo is not affected in any way
        :return:
        """
        self.ser.write('o\n'.encode())

