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
    
    def move_to_st(self, s, t):
        self.move_st(s - self.s, t - self.t)

    def move_st(self, delta_s, delta_t):
        print(f"Moving: delta_s={delta_s}, delta_t={delta_t}")
        self.plotter_controller.move(delta_s, delta_t)
        self.s += delta_s
        self.t += delta_t
        print("s, t, w = " + str(self.get_stw_pos()))

    def move(self, delta_x, delta_y):
        print(f"Moving: delta_x={delta_x}, delta_y={delta_y}")
        delta_s = math.floor(math.sqrt(delta_x ** 2 + delta_y ** 2))
        delta_t = math.floor(math.sqrt((self.w - delta_x) ** 2 + delta_y ** 2))
        self.move_st(delta_s, delta_t)
        print("s, t, w = " + str(self.get_stw_pos()))
    
    def move_to_xy(self, x, y):
        s = int(math.sqrt(x**2 + y**2))
        t = int(math.sqrt((self.w - x)**2 + y**2))
        self.move_to_st(s, t)
        self.save_stw()

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
        self.save_stw()

    def get_stw_pos(self):
        return self.s, self.t, self.w


if __name__ == '__main__':
    plotter = Plotter()
    plotter.calibrate()

    while True:
        amount = input("s t:")
        if amount == "0 0" or amount == "":
            break
        plotter.move_st(int(amount.split()[0]), int(amount.split()[1]))
    while True:
        amount = input("x y:")
        if amount == "0 0" or amount == "":
            break
        plotter.move_to_xy(int(amount.split()[0]), int(amount.split()[1]))

    print("s, t, w = " + str(plotter.get_stw_pos()))
    print("Plotter Finished")
