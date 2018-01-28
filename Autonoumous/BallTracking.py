import cv2
import numpy as np
import serial
import _thread

# Tilt up/down
tilt = 70
# Pan left/right conditional
pan = 0
myString = ""

threadTwoEnd = True
tiltMoved = False
found = False
pos_x = 0
pos_y = 0
ser = serial.Serial()


def connect():
    global camera

    # Open serial port
    ser.baudrate = 9600

    # look for a serial port in use
    for i in range(0, 10):
        try:
            print('../../../../../dev/ttyACM' + str(i))
            ser.port = '../../../../../dev/ttyACM' + str(i)
            ser.timeout = 1
            ser.open()
            break
        except:
            try:
                print('../../../../../dev/ttyUSB' + str(i))
                ser.port = '../../../../../dev/ttyUSB' + str(i)
                ser.timeout = 1
                ser.open()
                break
            except:
                try:
                    print('COM' + str(i))
                    ser.port = 'COM' + str(i)
                    ser.timeout = 1
                    ser.open()
                    break
                except:
                    continue


def find_ball(image):
    largest_contour = 0
    cont_index = 0
    img, contours, hierarchy = cv2.findContours(image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    for idx, contour in enumerate(contours):
        area = cv2.contourArea(contour)
        if area > largest_contour:
            largest_contour = area
            cont_index = idx
    r = (0, 0, 2, 2)
    if len(contours) > 0:
        r = cv2.boundingRect(contours[cont_index])
    return r, largest_contour


def track():
    global tilt
    global tiltMoved
    global pan
    global pos_x
    global pos_y
    global found
    if pos_x < 100 and found:
        # pan left
        pan = 1
    else:
        if pos_x > 540 and found:
            # pan right
            pan = 2
        else:
            pan = 0

    if pos_y < 100 and found:
        tilt = tilt + 1
        tiltMoved = True
    else:
        if pos_y > 340 and found:
            tilt = tilt - 1
            tiltMoved = True
        else:
            tiltMoved = False


def message():
    global myString
    global tilt
    global pan
    while threadTwoEnd:
        myHash = pan + tilt
        myString = str(pan) + "," + str(tilt) + "," + str(myHash)
        ser.write(myString + '\n')
        print(myString + '{' + ser.readline() + '}')
        # 'q' to quit
    _thread.exit()

    # print("no message")


def ball_tracking():
    # Capture video from camera
    global tilt
    global found
    global pan
    global pos_x
    global pos_y
    # Create a window for the sliders and HSV image.
    cv2.namedWindow("Original")
    cv2.namedWindow("HSV")
    cv2.namedWindow("Final")

    # Create the sliders
    # ToDo: Create Trackbar Sliders

    # Keep running until 'q' is pressed
    # look for a camera port in use
    connect()

    _thread.start_new_thread(message, ())
    camera = cv2.VideoCapture(0)

    while threadTwoEnd:
        # Get an image from the camera
        ret, frame = camera.read()

        # Convert frame from BGR to HSV, store it in frame2
        frame2 = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

        # Get the values of all of the sliders
        # TODO: Get Trackbar Info

        # TEMP TRACKBAR DATA ARRAY
        trackbar_data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

        h_lo = trackbar_data[0]
        h_hi = trackbar_data[1]
        s_lo = trackbar_data[2]
        s_hi = trackbar_data[3]
        v_lo = trackbar_data[4]
        v_hi = trackbar_data[5]
        e_itr = trackbar_data[6]
        d_itr = trackbar_data[7]
        e_box = trackbar_data[8]
        d_box = trackbar_data[9]

        # Create arrays for the lower and upper HSV values
        lower_hsv = np.array([h_lo, s_lo, v_lo])
        upper_hsv = np.array([h_hi, s_hi, v_hi])

        # Use the inRange function to filter the be submerged by watHSV values
        frame3 = cv2.inRange(frame2, lower_hsv, upper_hsv)

        # Morphological transformations
        if e_itr < 1 or d_itr < 1 or e_box < 2 or d_box < 2:
            combined_img = frame3
        else:
            erosion = cv2.erode(frame3, (e_box, e_box), iterations=e_itr)
            dilation = cv2.dilate(erosion, (e_box, e_box), iterations=d_itr)
            combined_img = dilation

        loct, area = find_ball(combined_img)
        x, y, w, h = loct

        if (w * h) < 10:
            found = False
        else:
            found = True
            center_x = x + (w / 2)
            center_y = y + (h / 2)
            x_pos = "X: {}".format(center_x)
            y_pos = "Y: {}".format(center_y)
            cv2.putText(frame, x_pos, (0, 20), cv2.FONT_HERSHEY_DUPLEX, 0.5, (255, 255, 1), 1, cv2.LINE_AA)
            cv2.putText(frame, y_pos, (0, 40), cv2.FONT_HERSHEY_DUPLEX, 0.5, (255, 255, 1), 1, cv2.LINE_AA)
            cv2.circle(frame, (int(center_x), int(center_y)), 3, (0, 110, 255), -1)
            pos_x = center_x
            pos_y = center_y

        track()

        # Display the images
        cv2.imshow("Original", frame)
        cv2.imshow("HSV", frame2)

        cv2.imshow("Final", combined_img)
        if cv2.waitKey(1) & 0xFF == ord("q"):
            global threadTwoEnd
            threadTwoEnd = False
            camera.release()
            cv2.destroyAllWindows()
            break


# Release the camera and close all windows.

_thread.start_new_thread(ball_tracking, ())
