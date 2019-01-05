import cv2
from syszuxav import syszuxav                      
import numpy as np
#128 means the decode thread number
v = syszuxav.SYSZUXav("gemfield.mp4", 128)
count = 0
while True:
  b = np.array(v.decodeJpg())
  count +=1
  if b.shape[0] == 0:
    continue

  cv2.imwrite("gemfield{}.jpg".format(count),b)
