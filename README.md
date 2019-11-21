# SYSZUXav
Python bindings for ffmpeg.
- SYSZUXav now only tested on Ubuntu 16.04/18.04.
- Other Linux distributions may work as well.


# Install
SYSZUXav has the dependency on ffmpeg 4.0 library.
On Ubuntu 16.04/18.04:
- install ffmpeg library dependencies

```
apt update
apt install software-properties-common
add-apt-repository ppa:jonathonf/ffmpeg-4
apt update
apt install ffmpeg libavutil-dev libswresample-dev libavcodec-dev libavformat-dev
```
If the ffmpeg libraries failed on the installation, then the runtime of syszuxav may throw exceptions like this:
`ImportError: libavcodec.so.58: cannot open shared object file: No such file or directory`

- install syszuxav
```
pip install syszuxav
```

# Usage
You can use SYSZUXav by simply import syszuxav:

```python
import cv2
from syszuxav import syszuxav                      
import numpy as np
#128 means decode thread number                 
v = syszuxav.SYSZUXav("gemfield.mp4", 128)
count = 0
while True:
  b = np.array(v.decodeJpg())
  if b.shape[0] == 0:
    continue
  cv2.imwrite("gemfield{}.jpg".format(count),b)
  count += 1
```

# Docker Image
You can pull a pre-built docker image from Docker Hub and run with
```bash
docker run -it --rm gemfield/syszuxav_build bash
```

Dockerfile is also supplied to build images on Ubuntu 16.04
```bash
cd docker
docker build  -t gemfield/syszuxav_build -f Dockerfile.syszuxav_build .
```
