#include <iostream>

typedef unsigned char uint8_t;
extern "C" {
    extern int img_height;
    extern int img_width;
    bool initav(const char* url);
    uint8_t* decode();
    void logging(const char *fmt, ...);
};

class Matrix {
    public:
        Matrix(uint8_t* data, size_t rows, size_t cols, size_t channels) : m_rows(rows), m_cols(cols), m_channels(channels) {
            m_data = data;
        }
        uint8_t *data() { return m_data; }
        size_t rows() const { return m_rows; }
        size_t cols() const { return m_cols; }
        size_t channels() const { return m_channels; }
    private:
        size_t m_rows, m_cols, m_channels;
        uint8_t *m_data;
};


class SYSZUXav {
    public:
        SYSZUXav(const std::string &url) : url(url){
            initav(url.c_str());
            path = "gemfield.jpg";
            null_path = "";
        }
        Matrix decodeJpg();

    private:
        std::string url;
        std::string path;
        std::string null_path;
};

Matrix SYSZUXav::decodeJpg()
{
    uint8_t* buffer = decode();
    logging("HAHA: %p",buffer);
    if(buffer == NULL){
        return Matrix(NULL, 0, 0, 0);
    }
    return Matrix(buffer, img_width, img_height, 3);
}

int main(int argc, char** argv)
{
    SYSZUXav av("gemfield.mp4");
    for (int i=0; i < 100; i++){
        logging("====================== %d",i);
        av.decodeJpg();
    }
    logging("yyyyyyyyy");
}

/*
gcc -c -fPIC -L/opt/ffmpeg/lib -I/opt/ffmpeg/include/ ffmpeg.c -lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -o ffmpeg.o
g++ -O3 -Wall -shared -std=c++11 syszuxav.cpp -fPIC `python3 -m pybind11 --includes` ffmpeg.o -L/opt/ffmpeg/lib -lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -o syszuxav`python3-config --extension-suffix`
g++ test.cpp ffmpeg.o -L/opt/ffmpeg/lib -lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -o test

g++ -O3 -Wall -shared -std=c++11 syszuxav.cpp -fPIC -I/usr/local/include/python2.7 -I/root/.local/include/python2.7 -I/usr/include/python2.7 ffmpeg.o -L/opt/ffmpeg/lib -lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -o syszuxav`python-config --extension-suffix`

import cv2
import syszuxav                      
import numpy as np                   
v = syszuxav.SYSZUXav("gemfield.mp4")
for i in range(100):
  b =  np.array(v.decodeJpg())

c = b.transpose(2,1,0)

c[:,:, 0] = c[:,:, 0].clip(16, 235).astype(c.dtype) - 16
c[:,:,1:] = c[:,:,1:].clip(16, 240).astype(c.dtype) - 128
    
A = np.array([[1.164,  0.000,  1.793],[1.164, -0.213, -0.533],[1.164,  2.112,  0.000]])
    
rgb = np.dot(c, A.T).clip(0, 255).astype('uint8')

apt update
apt install software-properties-common
add-apt-repository ppa:jonathonf/ffmpeg-4
apt update
apt install ffmpeg libavutil-dev libswresample-dev libavcodec-dev libavformat-dev
*/
