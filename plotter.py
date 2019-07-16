import math

from plottercontroller import *

SEGMENT_LENGTH = 50  # shorter segments means straighter lines


# TODO implement a get_xy() method

class Plotter(object):
    def __init__(self):
        self.x = None
        self.y = None
        self.s = None
        self.t = None
        self.w = None
        self.plotter_controller = PlotterController()
        self.save_file = None
        self.restore_stw()
        # Wiggle all the actuators, to check let the user know the serial connection is working
        self.move_by_st(50, 50)
        self.pen_down()
        time.sleep(0.5)
        self.move_by_st(-50, -50)
        self.pen_up()

    def restore_stw(self):
        """
        Restores the last s, t and w values from self.save_file
        """
        with open('save.txt', 'r') as self.save_file:
            self.s = int(next(self.save_file))
            self.t = int(next(self.save_file))
            self.w = int(next(self.save_file))

    def save_stw(self):
        """
        Saves the current s, t and w values to self.save_file
        """
        with open('save.txt', 'w') as self.save_file:
            print(self.s, file=self.save_file)
            print(self.t, file=self.save_file)
            print(self.w, file=self.save_file)

    def pen_up(self):
        """
        Move the pen up
        """
        self.plotter_controller.pen_up()

    def pen_down(self):
        """
        Move the pen down
        """
        self.plotter_controller.pen_down()

    def move_to_st(self, s, t, verbose=True):
        """
        Move to a location given in s, t coordinates
        This just calculates the difference between the given and current s, t coords,
        then moves by that difference
        """
        self.move_by_st(s - self.s, t - self.t, verbose=verbose)

    def move_by_st(self, delta_s, delta_t, verbose=True):
        """
        Move by an amount given in s, t values
        This also updates and saves the self.s and self.t values
        """
        self.plotter_controller.move(delta_s, delta_t)
        self.s += delta_s
        self.t += delta_t
        self.save_stw()
        if verbose:
            print(f"Moving: delta_s={delta_s}, delta_t={delta_t}, (s, t, w) = {self.get_stw_pos()}")

    def move_to_xy(self, x, y, verbose=True):
        """
        Move by an amount given in s, t values
        This also updates the self.s and self.t values
        The self.s and self.t values are saved in the self.move_to_st() method,
        which is always called
        """
        s = int(math.sqrt(x ** 2 + y ** 2))
        t = int(math.sqrt((self.w - x) ** 2 + y ** 2))
        if verbose:
            print(f"Moving: x={x}, y={y}")
        self.move_to_st(s, t, verbose=verbose)
        self.x = x
        self.y = y
    
    def move_straight_to_xy(self, x, y):
        if self.x is None:
            self.move_to_xy(x, y)
        else:
            delta_x = x - self.x
            delta_y = y - self.y
            segments_remaining = int(math.sqrt(delta_x**2 + delta_y**2) // SEGMENT_LENGTH)  # number of segments is line length / SEGMENT_LENGTH
            while segments_remaining > 0:
                delta_x = x - self.x
                delta_y = y - self.y
                self.move_to_xy(self.x + delta_x//segments_remaining, self.y + delta_y//segments_remaining)
                segments_remaining -= 1
            
            # move remaining distance
            self.move_to_xy(x, y)

    def calibrate(self, s=None, t=None, w=None, verbose=True):
        """
        Depreciated in favour of easy_calibrate
        """
        if s and t and w:
            self.s = s
            self.t = t
            self.w = w
        else:
            print("Starting calibration:")
            print("Stepper S going to home.")

            # Give the stepper a wiggle so you know which one you're working with
            self.plotter_controller.move(75, 0)
            self.plotter_controller.move(-75, 0)

            while True:
                amount = input("Move stepper S upwards by [integer]: ")
                if amount == "0" or amount == "":
                    break
                self.plotter_controller.move(int(amount), 0)

            # Give the stepper a wiggle so you know which one you're working with
            # Give the stepper a wiggle so you know which one you're working with
            self.plotter_controller.move(0, 75)
            self.plotter_controller.move(0, -75)
            print("Stepper T going to home.")
            while True:
                amount = input("Move stepper T upwards by [integer]: ")
                if amount == "0" or amount == "":
                    break
                self.plotter_controller.move(0, int(amount))

            # Now both steppers are calibrated to be zero
            self.s, self.t = 0, 0

            while True:
                amount = input("Move stepper S towards T by [integer]: ")
                if amount == "0" or amount == "":
                    break
                self.move_by_st(int(amount), 0, verbose=verbose)

            self.w = self.s
        print(f"Calibration complete, s={self.s}, t={self.t}, w={self.w}")
        self.save_stw()

    def easy_calibrate(self, verbose=True):
        """
        Calibrate the s, t and w values by manually moving the module to the
        top left, then top right corners just underneath the stepper motors
        """
        print("Welcome to easy calibration!© We appreciate your patronage")

        print("Move the module underneath the left stepper")
        amount = input("s t:")
        while amount != "":
            # Assume default of zero if only one argument is given
            amount += " 0"
            s, t = int(amount.split()[0]), int(amount.split()[1])
            self.plotter_controller.move(s, t)
            amount = input("s t:")
        self.s = 0

        print("Now move the module underneath the right stepper")
        self.control_using_st(verbose=verbose)
        self.w = self.s
        self.t = 0
        self.save_stw()
        print(f"Calibration complete! s={self.s}, t={self.t}, w={self.w}")

    def get_stw_pos(self):
        """
        :return: the current s, t and w values
        """
        return self.s, self.t, self.w

    def control_using_st(self, verbose=True):
        """
        Using s, t coordinates, control the motors until a newline is entered
        Enter a '?' to get a readout of the current stw values
        Entering just one number will default the second value to 0
        """
        amount = input("s t:")
        while amount != "":
            if amount == "?":
                print(f"(s, t, w) = {self.get_stw_pos()}")
                amount = input("x y:")
            # Assume default of zero if only one argument is given
            amount = amount + " 0"
            s, t = int(amount.split()[0]), int(amount.split()[1])
            self.move_by_st(s, t, verbose=verbose)
            amount = input("s t:")

    def control_using_xy(self, verbose=True):
        """
        Using x, y coordinates, control the motors until a newline is entered
        Enter a '?' to get a readout of the current stw values
        Entering just one number will default the second value to 0
        """
        amount = input("x y:")
        while amount != "":
            if amount == "?":
                print(f"(s, t, w) = {self.get_stw_pos()}")
                amount = input("x y:")
            # Assume default of zero for the second argument if only one argument is given
            amount = amount + " 0"
            x, y = int(amount.split()[0]), int(amount.split()[1])
            self.move_to_xy(x, y, verbose=verbose)
            amount = input("x y:")

    def control_from_cmd_line(self, verbose=True):
        """
        Allow control of the module from the command line, with either xy or st coordinate
        systems. The user can choose either or switch between them at will
        """
        mode = input("Control the module using st or xy coordinates? [xy/st/draw]: ")
        while mode in ["xy", "st", "draw"]:
            if mode == "st":
                self.control_using_st(verbose=verbose)
            if mode == "xy":
                self.control_using_xy(verbose=verbose)
            if mode == "draw":
                self.draw_square(4000)
            mode = input("Control the module using st or xy coordinates? [xy/st/draw]: ")

    def draw_square(self, side_length: int):
        """
        Draw a square
        :param side_length:
        :return:
        """

        middle = self.w / 2
        half_side = side_length / 2
        self.move_to_xy(middle - half_side, middle - half_side)  # Top Left

        self.pen_down()

        self.move_straight_to_xy(middle + half_side, middle - half_side)  # Top Right

        self.move_straight_to_xy(middle + half_side, middle + half_side)  # Bottom Right

        self.move_straight_to_xy(middle - half_side, middle + half_side)  # Bottom Left

        self.move_straight_to_xy(middle - half_side, middle - half_side)  # Top Left

        self.pen_up()

    def draw_axes(self):
        # Draw the horizontal axis
        self.move_to_xy(0, self.w / 2)

        self.pen_down()

        self.move_straight_to_xy(self.w, self.w / 2)

        self.pen_up()

        # Draw the vertical axis
        self.move_to_xy(self.w / 2, 0)

        self.pen_down()

        self.move_straight_to_xy(self.w / 2, self.w)

        self.pen_up()

    def draw_func(self, func=None):
        if not func:
            func = math.pow
        in_min = -10
        in_max = 10
        x_vals = [x for x in range(in_min, in_max)]
        y_vals = [func(x, 2) for x in x_vals]

        out_min = 0
        out_max = self.w
        # out_max = 12500
        x_mapped = [((x + abs(min(x_vals))) / (max(x_vals) - min(x_vals))) * (out_max - out_min) + out_min for x in x_vals]
        y_mapped = [((y + abs(min(y_vals))) / (max(y_vals) - min(y_vals))) * (out_max - out_min) + out_min for y in y_vals]

        print(x_mapped)
        print(y_mapped)




if __name__ == '__main__':
    plotter = Plotter()
    if "y" == input("s, t, w = " + str(plotter.get_stw_pos()) + ", Do you want to calibrate?[y/n]: "):
        plotter.easy_calibrate()

    plotter.draw_func()
    plotter.control_from_cmd_line()
    # for x in range(100, 7100, 100):
    #     plotter.move_to_xy(x, 7000)
    #     time.sleep(0.5)

    print("s, t, w = " + str(plotter.get_stw_pos()))
    print("Plotter Finished")
