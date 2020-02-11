import sys
import cv2
import numpy as np
import logging
import math
from keras.models import load_model
from hand_coded_lane_follower import HandCodedLaneFollower
import os

import serial
#ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)



cap = cv2.VideoCapture(0)
#cap = cv2.VideoCapture(video_file + '.avi')

#functions to change the resolution of video imput
def make_320p():
    cap.set(3, 320)
    cap.set(4, 240)
    
def make_640p():
    cap.set(3, 640)
    cap.set(4, 480)

# change resolution:
make_320p()
#make_640p()

#configure the video output codec
video_type = cv2.VideoWriter_fourcc(*'XVID')


model = load_model('lane_navigation_final.h5')
        
try:
    i = 0
    while cap.isOpened():
        
        _, frame = cap.read()
        frame = cv2.flip( frame, 0 )
        
        #arduino_serial=ser.readline().decode('ascii')
        #angle=int(arduino_serial)
        # print(angle)
        #write the frame on hardisk
        #cv2.imwrite("video%d_%03d_%03d.png" % (random_number, i, angle), frame)
        #show the real time video outpu on screen
        cv2.imshow('Realtime Video Output',frame)


        def compute_steering_angle(self, frame):
            preprocessed = img_preprocess(frame)
            X = np.asarray([preprocessed])
            steering_angle = model.predict(X)[0]
            return steering_angle

        #increment value for frame name
        i += 1
        #if 'q' key is pressed the script is stoped and video/frames recodring is ended
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
finally:
    cap.release()
    cv2.destroyAllWindows()



