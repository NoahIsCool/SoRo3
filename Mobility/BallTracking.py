import cv2
import numpy as np

# Function for the sliers to call when changed
def blah(x):
    pass

def findBall(image, gray, dp, minDist, param1, param2, minRadius, maxRadius, lean):
    largest_contour = 0
    cont_index = 0
    circles = None
    img, contours, hierarchy = cv2.findContours(image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    for idx, contour in enumerate(contours):
        area = cv2.contourArea(contour)
        if dp*minDist*param1*param2*maxRadius*minRadius is not 0:
            x,y,w,h = cv2.boundingRect(contour)
            print (x, y, w, h)
            subImage = image[x:x+w, y:y+h]
            #cv2.imshow("subImg", subImage)
            tmpCircles = cv2.HoughCircles(image, cv2.HOUGH_GRADIENT, dp, minDist, param1, param2, minRadius, maxRadius)
            
            if tmpCircles is not None and (area > largest_contour):

                largest_contour = area
                circles = tmpCircles
                cont_index = idx
    r=(0,0,2,2)
    if len(contours) > 0:
        r = cv2.boundingRect(contours[cont_index])
    return r,largest_contour,circles

# Capture video from camera
camera = cv2.VideoCapture(0)

# Create a window for the sliders and HSV image.
cv2.namedWindow("HSV")
cv2.namedWindow("YCrCb")
cv2.namedWindow("Final")

# Create the sliders
cv2.createTrackbar("H_LO", "HSV", 29, 180, blah)
cv2.createTrackbar("H_HI", "HSV", 38, 180, blah)
cv2.createTrackbar("S_LO", "HSV", 78, 255, blah)
cv2.createTrackbar("S_HI", "HSV", 215, 255, blah)
cv2.createTrackbar("V_LO", "HSV", 90, 255, blah)
cv2.createTrackbar("V_HI", "HSV", 215, 255, blah)

cv2.createTrackbar("Y_LO", "YCrCb", 0, 255, blah)
cv2.createTrackbar("Y_HI", "YCrCb", 0, 255, blah)
cv2.createTrackbar("Cr_LO", "YCrCb", 0, 255, blah)
cv2.createTrackbar("Cr_HI", "YCrCb", 0, 255, blah)
cv2.createTrackbar("Cb_LO", "YCrCb", 0, 255, blah)
cv2.createTrackbar("Cb_HI", "YCrCb", 0, 255, blah)

cv2.createTrackbar("dp", "Final", 1,5, blah)
cv2.createTrackbar("Min_Dist", "Final", 1000, 1920, blah)
cv2.createTrackbar("param1", "Final", 5,500, blah)
cv2.createTrackbar("param2", "Final", 5,500, blah)
#cv2.createTrackbar("Min_Radius", "Final", 10,50, blah)
cv2.createTrackbar("Max_Radius", "Final", 10,50, blah)
cv2.createTrackbar("Leaneancy", "Final", 5,100, blah)


# Keep running until 'q' is pressed
while(1):
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
    y_lo = cv2.getTrackbarPos("Y_LO", "YCrCb")
    y_hi = cv2.getTrackbarPos("Y_HI", "YCrCb")
    cr_lo = cv2.getTrackbarPos("Cr_LO", "YCrCb")
    cr_hi = cv2.getTrackbarPos("Cr_HI", "YCrCb")
    cb_lo = cv2.getTrackbarPos("Cb_LO", "YCrCb")
    cb_hi = cv2.getTrackbarPos("Cb_HI", "YCrCb")
    
    dp = cv2.getTrackbarPos("dp", "Final")
    minDist = cv2.getTrackbarPos("Min_Dist", "Final")
    param1 = cv2.getTrackbarPos("param1", "Final")
    param2 = cv2.getTrackbarPos("param2", "Final")
    #minRadius = cv2.getTrackbarPos("Min_Radius", "Final")
    maxRadius = cv2.getTrackbarPos("Max_Radius", "Final")
    lean = cv2.getTrackbarPos("Leaneancy", "Final")

    # Create arrays for the lower and upper HSV values
    lower_hsv = np.array([h_lo,s_lo,v_lo])
    upper_hsv = np.array([h_hi,s_hi,v_hi])
    lower_ycrcb = np.array([y_lo,cr_lo,cb_lo])
    upper_ycrcb = np.array([y_hi,cr_hi,cb_hi])

    # Use the inRange function to filter the HSV values
    frame3 = cv2.inRange(frame2, lower_hsv, upper_hsv)
    frame4 = cv2.inRange(ycrcb_img, lower_ycrcb, upper_ycrcb)

    # Morphological transformations
    erosion = cv2.erode(frame3, (7,7), iterations=4)
    dilation = cv2.dilate(erosion, (3,3), iterations=2)
    erosion2 = cv2.erode(frame4, (7,7), iterations=4)
    dilation2 = cv2.dilate(erosion2, (3,3), iterations=2)

    combined_img = dilation | dilation2

    loct, area, circles = findBall(combined_img, gray, dp, minDist, param1, param2, 1, maxRadius, lean)
    x,y,w,h = loct

    if (w*h) < 10:
        found = 0
    else:
        found = 1
        simg2 = cv2.rectangle(frame, (x,y), (x+w,y+h), 255,2)
        center_x=x+((w)/2)
        center_y=y+((h)/2)
        cv2.circle(frame,(int(center_x),int(center_y)),3,(0,110,255),-1)
        center_x-=80
        center_y=6--center_y

    if circles is not None:
        circles = np.uint16(np.around(circles))
        for i in circles:
            center = (i[0] + x, i[1] + y)
            radius = i[2]
            cv2.circle(frame, center, radius,(0,255,0),2)
            cv2.circle(frame, center, 2,(255,0,0),3)
    
    # Display the images
    cv2.imshow("Original", frame)
    cv2.imshow("HSV", frame2)
    cv2.imshow("YCrCb", ycrcb_img)
    cv2.imshow("Final", combined_img)

    # 'q' to quit
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

# Release the camera and close all windows.
camera.release()
cv2.destroyAllWindows()
