from plottercontroller import *


list_available_ports()

plotter_controller = PlotterController(input("Which port?: "))

class Plotter(object):
    def __init__(self):
        self.s = None
        self.t = None
        self.w = None

    def calibrate():
        print("Starting calibration:")
        print("Stepper S going to home.")
        while True:
            amount = input("Move stepper S upwards by [integer]: ")
            if amount == "0" or amount == "":
                break
            plotter_controller.move(int(amount), 0)

        print("Stepper T going to home.")
        while True:
            amount = input("Move stepper T upwards by [integer]: ")
            if amount == "0" or amount == "":
                break
            plotter_controller.move(0, int(amount))

        # Now both steppers are calibrated to be zero
        s, t = 0, 0

        print("Stepper S going to Stepper T.")
        while True:
            amount = input("Move stepper S downwards by [integer]: ")
            if amount == "0" or amount == "":
                break
            plotter_controller.move(0, -int(amount))



if __name__ == '__main__':
    calibrate()
