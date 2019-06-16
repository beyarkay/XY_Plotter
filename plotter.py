import math

from plottercontroller import *


class Plotter(object):
    def __init__(self):
        self.s = None
        self.t = None
        self.w = None
        self.plotter_controller = PlotterController()
        self.STEPS_PER_CM = 1000 / 4
        self.file = None
        self.restore_stw()
        # Wiggle all the actuators, to check let the user know the serial connection is working
        self.move_by_st(75, 75)
        self.pen_down()
        self.move_by_st(-75, -75)
        self.pen_up()

    def save_stw(self):
        with open('save.txt', 'w') as self.file:
            print(self.s, file=self.file)
            print(self.t, file=self.file)
            print(self.w, file=self.file)

    def restore_stw(self):
        with open('save.txt', 'r') as self.file:
            self.s = int(next(self.file))
            self.t = int(next(self.file))
            self.w = int(next(self.file))

    def pen_up(self):
        self.plotter_controller.pen_up()

    def pen_down(self):
        self.plotter_controller.pen_down()

    def move_to_st(self, s, t):
        self.move_by_st(s - self.s, t - self.t)

    def move_by_st(self, delta_s, delta_t, verbose=True):
        self.plotter_controller.move(delta_s, delta_t)
        self.s += delta_s
        self.t += delta_t
        if verbose:
            print(f"Moving: delta_s={delta_s}, delta_t={delta_t}, (s, t, w) = {self.get_stw_pos()}")

    def move_to_xy(self, x, y):
        s = int(math.sqrt(x ** 2 + y ** 2))
        t = int(math.sqrt((self.w - x) ** 2 + y ** 2))
        self.move_to_st(s, t)
        self.save_stw()

    def calibrate(self, s=None, t=None, w=None, verbose=True):
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
        print("Welcome to easy calibration!© We appreciate your patronage")

        print("Move the module underneath the left stepper")
        amount = input("s t:")
        while amount != "":
            # Assume default of zero if only one argument is given
            amount += " 0"
            s, t = int(amount.split()[0]), int(amount.split()[1])
            self.plotter_controller.move(s, t)
        self.s = 0

        print("Now move the module underneath the right stepper")
        self.control_using_st()
        self.w = self.s
        self.t = 0
        self.save_stw()
        print(f"Calibration complete! s={self.s}, t={self.t}, w={self.w}")

    def get_stw_pos(self):
        return self.s, self.t, self.w

    def control_using_st(self, verbose=True):
        amount = input("s t:")
        while amount != "":
            # Assume default of zero if only one argument is given
            amount = amount + " 0"
            s, t = int(amount.split()[0]), int(amount.split()[1])
            self.move_by_st(s, t, verbose=verbose)

    def control_using_xy(self, verbose=True):
        amount = input("x y:")
        while amount != "":
            # Assume default of zero for the second argument if only one argument is given
            amount = amount + " 0"
            x, y = int(amount.split()[0]), int(amount.split()[1])
            self.move_by_st(x, y, verbose=verbose)

    def control_from_cmd_line(self, verbose=True):
        mode = input("Control the module using st or xy coordinates? [xy/st]: ")

        while mode in ["xy", "st"]:
            if mode == "st":
                self.control_using_st(verbose=verbose)
            if mode == "xy":
                self.control_using_xy(verbose=verbose)
            mode = input("Control the module using st or xy coordinates? [xy/st]: ")


if __name__ == '__main__':
    plotter = Plotter()

    if "y" == input("s, t, w = " + str(plotter.get_stw_pos()) + ", Do you want to calibrate?[y/n]: "):
        plotter.easy_calibrate()

    # plotter.control_from_cmd_line()
    plotter.plotter_controller.move(-1000, -1000)
    # for x in range(100, 7100, 100):
    #     plotter.move_to_xy(x, 7000)
    #     time.sleep(0.5)

    input("TURN OFF THE STEPPERS OR YOU'LL BE REDIRECTED TO dev/null [okay/yes]: ")
    print("s, t, w = " + str(plotter.get_stw_pos()))
    print("Plotter Finished\n\nYOU DID TURN OFF THE STEPPERS, RIGHT?")
