import cv2
import numpy as np
import serial
import thread
import math
import time

"""
This code provides the rover with tennis ball tracking capabilities.
When this code is ran, it looks for a tennis ball and, if found,
    returns the bearing and distance of the tennis ball in respect
    to the rover.
"""

class BallTracker:
    def __init__(self):
        # Tilt up/down
        self.tilt = 70

        # Pan left/right conditional
        self.pan = 0
        self.my_string = ""

        # control variables
        self.thread_two_end = True
        self.tilt_moved = False
        self.found = False
        self.pos_x = 0
        self.pos_y = 0
        self.ser = serial.Serial()
        self.midpoint = 320
        self.dpp = 82.1 / 640
        # Angle left of center -> -(320 - pos_x)
        # Angle right of center -> pos_x - 320
        self.angle = 0.0
        self.distance = 0.0
        self.hasControl = False

        #HSV stuff
        trackbar_data = [30, 34, 15, 255, 126, 255, 1, 1, 2, 2]
        self.h_lo = trackbar_data[0]
        self.h_hi = trackbar_data[1]
        self.s_lo = trackbar_data[2]
        self.s_hi = trackbar_data[3]
        self.v_lo = trackbar_data[4]
        self.v_hi = trackbar_data[5]
        self.e_itr = trackbar_data[6]
        self.d_itr = trackbar_data[7]
        self.e_box = trackbar_data[8]
        self.d_box = trackbar_data[9]

        # start thread on pi
        thread.start_new_thread(self.ball_tracking, ())

    def connect(self):
        # Open serial port
        self.ser.baudrate = 9600

        # look for a serial port in use
        for i in range(0, 10):
            try:
                #
                self.ser.port = '../../../../../dev/ttyACM' + str(i)
                self.ser.timeout = 1
                self.ser.open()
                print('../../../../../dev/ttyACM' + str(i))
                break
            except:
                try:
                    #
                    self.ser.port = '../../../../../dev/ttyUSB' + str(i)
                    self.ser.timeout = 1
                    self.ser.open()
                    print('../../../../../dev/ttyUSB' + str(i))
                    break
                except:
                    try:
                        #
                        self.ser.port = 'COM' + str(i)
                        self.ser.timeout = 1
                        self.ser.open()
                        print('COM' + str(i))
                        break
                    except:
                        continue

    def track(self):
        if self.pos_x < 100 and self.found:
            # pan left
            self.pan = 1
        else:
            if self.pos_x > 540 and self.found:
                # pan right
                self.pan = 2
            else:
                # Don't pan
                self.pan = 0

        if self.pos_y < 100 and self.found:
            # If a ball is found and the ball is in the top 100 pixels of the image..
            self.tilt = self.tilt + 1
            self.tilt_moved = True
        else:
            if self.pos_y > 340 and self.found:
                # IF the ball is in the bottom 100 pixels
                self.tilt = self.tilt - 1
                self.tilt_moved = True
            else:
                self.tilt_moved = False

    def message(self):
        while self.thread_two_end:
            my_hash = self.pan + self.tilt
            self.my_string = str(self.pan) + "," + str(self.tilt) + "," + str(my_hash)
            self.ser.write(self.my_string + '\n')
            print(self.my_string + '{' + self.ser.readline() + '}')
            # 'q' to quit
        thread.exit()

    def ball_tracking(self):
        # Keep running until 'q' is pressed

        # look for a camera port in use
        self.connect()

        # start communication thread
        #thread.start_new_thread(self.message, ())

        # capture video from camera
        camera = cv2.VideoCapture(0)

        while True:
            # Get an image from the camera
            ret, frame = camera.read()

            # Convert frame from BGR to HSV, store it in frame2
            frame2 = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)


            # Create arrays for the lower and upper HSV values
            lower_hsv = np.array([self.h_lo, self.s_lo, self.v_lo])
            upper_hsv = np.array([self.h_hi, self.s_hi, self.v_hi])

            # Use the inRange function to filter the be submerged by watHSV values
            frame3 = cv2.inRange(frame2, lower_hsv, upper_hsv)

            # Morphological transformations
            if self.e_itr < 1 or self.d_itr < 1 or self.e_box < 2 or self.d_box < 2:
                combined_img = frame3
            else:
                erosion = cv2.erode(frame3, (self.e_box, self.e_box), iterations=self.e_itr)
                dilation = cv2.dilate(erosion, (self.e_box, self.e_box), iterations=self.d_itr)
                combined_img = dilation

            # Locates the Ball
            location, area = self.find_ball(combined_img)
            if area is None:
                continue
            x, y, w, h = location  # top_left_x, top_left_y, width, height
            
            # identify and track if large enough
            if (w * h) < 10:
                self.found = False
            else:
                self.found = True
                center_x = x + (w / 2)
                center_y = y + (h / 2)
                x_pos = "X: {}".format(center_x)
                y_pos = "Y: {}".format(center_y)
                cv2.putText(frame, x_pos, (0, 20), cv2.FONT_HERSHEY_DUPLEX, 0.5, (255, 255, 1), 1, cv2.CV_AA)
                cv2.putText(frame, y_pos, (0, 40), cv2.FONT_HERSHEY_DUPLEX, 0.5, (255, 255, 1), 1, cv2.CV_AA)
                cv2.circle(frame, (int(center_x), int(center_y)), 3, (0, 110, 255), -1)
                self.pos_x = center_x
                self.pos_y = center_y

                # Find the angle of the ball from the center of the image.
                if self.pos_x < 320:
                    self.angle = -(320 - center_x) * self.dpp
                else:
                    self.angle = (center_x - 320) * self.dpp
                self.distance = .0686 / (2.0 * math.tan(math.radians(w * self.dpp /2)))
                                                         
            self.track()

            # Check if q is pressed
            if cv2.waitKey(1) & 0xFF == ord("q"):
                self.thread_two_end = False

                # Release the camera and close all windows.
                camera.release()
                cv2.destroyAllWindows()

    @staticmethod
    def find_ball(image):
        largest_contour = 0
        cont_index = 0
        r = None
        largest_contour = None
        contours, hierarchy = cv2.findContours(image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        for idx, contour in enumerate(contours):
            
            area = cv2.contourArea(contour)
            if area is not None:
               if area > largest_contour:
                    largest_contour = area
                    cont_index = idx
               r = (0, 0, 2, 2)
               if len(contours) > 0:
                    r = cv2.boundingRect(contours[cont_index])
        return r, largest_contour

    def hasFound(self):
        return self.found

    def setControl(control):
        self.hasControl = control

    def getControl(self):
        return self.hasControl

    def getAngle(self):
        return self.angle
