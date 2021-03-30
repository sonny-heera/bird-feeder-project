# Sandeep Heera
# protocolRecognition3.py
# This program communicates with a Moteino
# microcontroller via the UART port to send
# data regarding the status of the image recognition system.
# The Moteino is connected to the motor controller which actuates 
# the motor to move the device to the correct dispensing position
# corresponding to bird that is detected.

import cv2
import sys
import logging as log
import datetime as dt
import time
import serial
from time import sleep

ser=serial.Serial('/dev/ttyAMA0',9600)
vulturePath = "KingVulture20Stages.xml"
macawPath = "macaw13Stage.xml"
hornedBillPath = "HornedBill13Stage.xml"
facePath = "haarcascade_frontalface_default.xml"
vultureCascade = cv2.CascadeClassifier(vulturePath)
macawCascade = cv2.CascadeClassifier(macawPath)
hornedBillCascade = cv2.CascadeClassifier(hornedBillPath)
faceCascade = cv2.CascadeClassifier(facePath)
log.basicConfig(filename='webcam.log',level=log.INFO)

video_capture = cv2.VideoCapture(0)

video_capture.set(3,200)

video_capture.set(4,200)
anterior = 0

while True:
    moteinoCommand = ser.readline()
    moteinoCommand = moteinoCommand.rstrip()
    print moteinoCommand
    if moteinoCommand == "start":
        if not video_capture.isOpened():
            print('Unable to load camera.')
            sleep(5)
            pass

        # Capture frame-by-frame
        ret, frame = video_capture.read()

        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        vultures = vultureCascade.detectMultiScale(
            gray,
            scaleFactor=1.1,
            minNeighbors=5,
            minSize=(30, 30)
        )
        macaws = macawCascade.detectMultiScale(
            gray,
            scaleFactor=1.1,
            minNeighbors=7,
            minSize=(40, 40)
        )
        hornedBills = hornedBillCascade.detectMultiScale(
            gray,
            scaleFactor=1.1,
            minNeighbors=5,
            minSize=(30, 30)
        )
        faces = faceCascade.detectMultiScale(
            gray,
            scaleFactor=1.1,
            minNeighbors=5,
            minSize=(30, 30)
        )   
        if len(vultures) > 0:
            ser.write('0')
            print 'vulture'
        elif len(macaws) > 0:
            ser.write('1')
            print 'macaw'
        elif len(hornedBills) > 0:
            ser.write('2')
            print 'hornedBill'
        elif len(faces) > 0:
            ser.write('3')
            print 'face'
        else:
            ser.write('-1')
            print('none')
        # Draw a rectangle around the faces
        for (x, y, w, h) in vultures:
            cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)

        if anterior != len(vultures):
            anterior = len(vultures)
            log.info("vultures: "+str(len(vultures))+" at "+str(dt.datetime.now()))

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

        # Display the resulting frame
        #cv2.imshow('Video', frame)
    elif moteinoCommand == "end":
        print "In manual mode"


# When everything is done, release the capture
video_capture.release()
cv2.destroyAllWindows()
