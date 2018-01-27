import cv2
import numpy as np
import serial
import thread


# Tilt up/down
tilt = 70
# Pan left/right condinitional
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

    #look for a serial port in use
    for i in range(0, 10):
        try:
            
            print '../../../../../dev/ttyACM' + str(i)
            ser.port = '../../../../../dev/ttyACM' + str(i)
            ser.timeout = 1
            ser.open()
            break
        except:
            try:
                print '../../../../../dev/ttyUSB' + str(i)
                ser.port = '../../../../../dev/ttyUSB' + str(i)
                ser.timeout = 1
                ser.open()
                break
            except:
                try:
                    print 'COM' + str(i)
                    ser.port = 'COM' + str(i)
                    ser.timeout = 1
                    ser.open()
                    break
                except:
                    continue
    
        


# Function for the sliers to call when changed
def blah(x):
    pass

def findBall(image):
    largest_contour = 0
    cont_index = 0
    img, contours, hierarchy = cv2.findContours(image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    for idx, contour in enumerate(contours):
        area = cv2.contourArea(contour)
        if (area > largest_contour):
            largest_contour = area

            cont_index = idx
    r = (0,0,2,2)
    if len(contours) > 0:
        r = cv2.boundingRect(contours[cont_index])
    return r,largest_contour

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
    #print tilt, pan, pos_x, pos_y
    
   
    

def message():
    global myString
    global tilt
    global pan
    while(threadTwoEnd):
        myHash = pan + tilt
        myString = str(pan) + "," + str(tilt) + "," + str(myHash) 
        ser.write(myString +  '\n')
        print(myString + '{' + ser.readline() + '}')
        # 'q' to quit
    thread.exit()
        
    # print("no message")0


    
    
def ballTracking():
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
    cv2.createTrackbar("H_LO", "HSV", 28, 60, blah)
    cv2.createTrackbar("H_HI", "HSV", 40, 60, blah)
    cv2.createTrackbar("S_LO", "HSV", 83, 255, blah)
    cv2.createTrackbar("S_HI", "HSV", 255, 255, blah)
    cv2.createTrackbar("V_LO", "HSV", 72, 255, blah)
    cv2.createTrackbar("V_HI", "HSV", 255, 255, blah)
    cv2.createTrackbar("e_itr", "HSV", 2, 4, blah)
    cv2.createTrackbar("d_itr", "HSV", 2, 4, blah)
    cv2.createTrackbar("e_box", "HSV", 2, 10, blah)
    cv2.createTrackbar("d_box", "HSV", 2, 10, blah)
    
    # Keep running until 'q' is pressed
    #look for a camera port in use
    connect()
##    for i in range(0, 10):
##        try:
##            print 'CAM' + str(i)
##            camera = cv2.VideoCapture(i)
##            ret, cap = camera.read()
##            frame2 = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
##            break
##        except:
##            continue
    
    thread.start_new_thread(message, ())
    camera = cv2.VideoCapture(0)
    
    while(threadTwoEnd):
        # Get an imaage from the camera
        ret, frame = camera.read()

        # Copy frame to frame2. frame3 is the HSV image.
        frame2 = frame
        
        # Convert frame from BGR to HSV, store it in frame2
        frame2 = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        
        ycrcb_img = cv2.cvtColor(frame, cv2.COLOR_BGR2YCrCb)

        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        # Get the values of all of the sliders
        h_lo = cv2.getTrackbarPos("H_LO", "HSV")
        h_hi = cv2.getTrackbarPos("H_HI", "HSV")
        s_lo = cv2.getTrackbarPos("S_LO", "HSV")
        s_hi = cv2.getTrackbarPos("S_HI", "HSV")
        v_lo = cv2.getTrackbarPos("V_LO", "HSV")
        v_hi = cv2.getTrackbarPos("V_HI", "HSV")
        e_itr =cv2.getTrackbarPos("e_itr", "HSV")
        d_itr =cv2.getTrackbarPos("d_itr", "HSV")
        e_box =cv2.getTrackbarPos("e_box", "HSV")
        d_box =cv2.getTrackbarPos("d_box", "HSV")


        # Create arrays for the lower and upper HSV values
        lower_hsv = np.array([h_lo,s_lo,v_lo])
        upper_hsv = np.array([h_hi,s_hi,v_hi])

        # Use the inRange function to filter the be submerged by watHSV values
        frame3 = cv2.inRange(frame2, lower_hsv, upper_hsv)

        # Morphological transformations
        if e_itr < 1 or d_itr < 1 or e_box<2 or d_box < 2:
            combined_img = frame3
        else:
            erosion = cv2.erode(frame3, (e_box,e_box), iterations=e_itr)
            dilation = cv2.dilate(erosion, (e_box,e_box), iterations=d_itr)
            combined_img = dilation

        loct, area = findBall(combined_img)
        x,y,w,h = loct

        if (w*h) < 10:
            found = False
        else:
            found = True
            simg2 = cv2.rectangle(frame, (x,y), (x+w,y+h), 255,2)
            center_x=x+((w)/2)
            center_y=y+((h)/2)
            x_pos = "X: {}".format(center_x)
            y_pos = "Y: {}".format(center_y)
            cv2.putText(frame, x_pos, (0,20), cv2.FONT_HERSHEY_DUPLEX, 0.5, (255,255,1),1,cv2.LINE_AA)
            cv2.putText(frame, y_pos, (0,40), cv2.FONT_HERSHEY_DUPLEX, 0.5, (255,255,1),1,cv2.LINE_AA)
            cv2.circle(frame,(int(center_x),int(center_y)),3,(0,110,255),-1)
            #center_x-=80
            #center_y=6--center_y
            pos_x = center_x
            pos_y = center_y

        track()
        #if pan is 0 and not tiltMoved:
            
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

thread.start_new_thread(ballTracking, ())


    



