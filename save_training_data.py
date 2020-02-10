import numpy as np
import cv2
import sys
import random
import os

#import serial library and inilising the serial conection with Arduino (for example port: ttyACM0), this port need to be changed according to port where Arduino is connected
import serial
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)



random_number=random.randrange(1, 1000,1)
                               
#define location where to save the video and frames:
#video_file='/media/pi/Storage/AI/Keyes_L298P_RF_Robo_RF_Servo/training_video/video'

random_number=random.randrange(1, 1000,1)
print(random_number)    
   

def createFolder(directory):
    try:
        if not os.path.exists(directory):
            os.makedirs(directory)
    except OSError:
        print ('Error: Creating directory. ' +  directory)
        

# Example
createFolder('./training_video/%d' %random_number)
# Creates a folder in the current directory called data
print(f"pwd={os.getcwd()}") # current working directory
os.chdir("training_video/%d"%random_number) #change directory to the random created one
print(f"pwd={os.getcwd()}") # current working directory

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

#video write function, set output video name fps, resolution (resolution is set by int(cap.get(3)), int(cap.get(4))) that is the same as video imput resolution, if you a different resolution the video will not be exported)
out = cv2.VideoWriter("video_%d.avi" %random_number, video_type, 20.0, (int(cap.get(3)), int(cap.get(4))))

try:
    i = 0
    while cap.isOpened():
        
        _, frame = cap.read()
        frame = cv2.flip( frame, 0 )
        arduino_serial=ser.readline().decode('ascii')
        angle=int(arduino_serial)
        print(angle)
        #write the frame on hardisk
        cv2.imwrite("video%d_%03d_%03d.png" % (random_number, i, angle), frame)
        #show the real time video outpu on screen
        cv2.imshow('Realtime Video Output',frame)
        #write the video framese in video export
        out.write(frame)
        #increment value for frame name
        i += 1
        #if 'q' key is pressed the script is stoped and video/frames recodring is ended
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
finally:
    cap.release()
    out.release()
    cv2.destroyAllWindows()


#if __name__ == '__main__':
#    save_image_and_steering_angle('/media/pi/Storage/AI/Keyes_L298P_RF_Robo_RF_Servo/training_video/video')
