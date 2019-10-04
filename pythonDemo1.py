import numpy as np
import cv2
from time import localtime, strftime

cap = cv2.VideoCapture(0)

fourcc = cv2.VideoWriter_fourcc(*'XVID')
out = cv2.VideoWriter('output.avi', fourcc, 24.0, (640,480), True)
font = cv2.FONT_HERSHEY_SIMPLEX

while(cap.isOpened()):
    ret, frame = cap.read()
    if ret == True:
        time = strftime("%Y-%m-%d %H:%M:%S", localtime())
        cv2.putText(frame, time, (10,50), font, 1, (255,255,255), 2, cv2.LINE_AA)
        out.write(frame)
        cv2.imshow('frame', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
        
    else:
        break
        
cap.release()
out.release()
cv2.destroyAllWindows()
        
