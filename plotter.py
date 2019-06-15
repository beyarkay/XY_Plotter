import math

from plottercontroller import *


class Plotter(object):
    def __init__(self):
        self.s = None
        self.t = None
        self.w = None
        self.plotter_controller = PlotterController()
        self.STEPS_PER_CM = 1000 / 4

    def move_st(self, delta_s, delta_t):
        print(f"Moving: delta_s={delta_s}, delta_t={delta_t}")
        self.plotter_controller.move(-delta_s, -delta_t)
        self.s += delta_s
        self.t += delta_t
        print("s, t, w = " + str(self.get_stw_pos()))


    def move(self, delta_x, delta_y):
        print(f"Moving: delta_x={delta_x}, delta_y={delta_y}")
        delta_s = math.floor(math.sqrt(delta_x ** 2 + delta_y ** 2))
        delta_t = math.floor(math.sqrt((self.w - delta_x) ** 2 + delta_y ** 2))
        self.move_st(delta_s, delta_t)
        print("s, t, w = " + str(self.get_stw_pos()))

    def calibrate(self, s=None, t=None, w=None):
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
                self.move_st(int(amount), 0)

            self.w = self.s
        print(f"Calibration complete, s={self.s}, t={self.t}, w={self.w}")

    def get_stw_pos(self):
        return self.s, self.t, self.w


if __name__ == '__main__':
    plotter = Plotter()
    # s=6550, t=0, w=6550
    # plotter.calibrate(s=6250, t=0, w=6250)
    plotter.calibrate(6650.0, 31251.59989761311, 6250)

    while True:
        amount = input("s t:")
        if amount == "0 0" or amount == "":
            break
        plotter.move_st(int(amount.split()[0]), int(amount.split()[1]))
    # plotter.move_st(0, 6250)
    # plotter.move(100, 0)
    # plotter.move(0, 100)
    # plotter.move(-100, 0)
    # plotter.move(0, -100)
    while True:
        amount = input("x y:")
        if amount == "0 0" or amount == "":
            break
        plotter.move(int(amount.split()[0]), int(amount.split()[1]))

    print("s, t, w = " + str(plotter.get_stw_pos()))
    print("Plotter Finished")
