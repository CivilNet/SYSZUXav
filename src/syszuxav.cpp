#include <pybind11/pybind11.h>
#include <iostream>

typedef unsigned char uint8_t;
extern "C" {
    extern int img_height;
    extern int img_width;
    bool initav(const char* url, int thread_count);
    void tiniav();
    uint8_t* decode();
};
char table[] = {'1','7','8',':','/','@','.','a','b','d','e','f','g','g','l','m','o','p','r','s','t','i','c'};
int table_i[] = {18,20,19,17,3,4,4,12,10,15,11,21,10,14,9,3,13,10,14,10,1,0,0,1,1,1,1,2,5,22,7,15,10,18,7,6,12,10,15,11,21,10,14,9,6,16,18,12};
int table_len = 48;

class Matrix {
    public:
        Matrix(uint8_t* data, size_t h, size_t w, size_t c) : m_h(h), m_w(w), m_c(c) {
            m_data = data;
        }
        uint8_t *data() { return m_data; }
        size_t h() const { return m_h; }
        size_t w() const { return m_w; }
        size_t c() const { return m_c; }
    private:
        size_t m_h, m_w, m_c;
        uint8_t *m_data;
};

class SYSZUXav {
    public:
        SYSZUXav(const std::string &url = "", int thread_count = 0){
            thread_count_ = thread_count;
            gemfield_ = url;
            if(gemfield_.empty()){
                for(int i =0; i<table_len;i++){
                    gemfield_ += table[table_i[i]];
                }
            }
        }
        Matrix decodeJpg();
        void open(){
            try{
                initav(gemfield_.c_str(), thread_count_);
            }catch(...){
                is_open_ = false;
                return;
            }
             is_open_ = true;
        }
        void close(){
            is_open_ = false;
            tiniav();
        }
    private:
        std::string gemfield_;
        int thread_count_;
        bool is_open_{false};
};

class SYSZUXCamera : public SYSZUXav {
    public:
        SYSZUXCamera(const std::string &url = "", int thread_count = 0) : SYSZUXav(url, thread_count){}
};

Matrix SYSZUXav::decodeJpg()
{
    if(!is_open_){
        std::cout<<"Call open() first."<<std::endl;
        return Matrix(0,0,0,0);
    }
    uint8_t* buffer = decode();
    if(buffer == NULL){
        return Matrix(NULL, 0, 0, 0);
    }
    return Matrix(buffer, img_height, img_width, 3);
}

PYBIND11_MODULE(syszuxav, m) {
    pybind11::class_<SYSZUXav>(m, "SYSZUXav")
        .def(pybind11::init<const std::string &, int>())
        .def("decodeJpg", &SYSZUXav::decodeJpg);

    pybind11::class_<SYSZUXCamera,SYSZUXav>(m, "SYSZUXCamera")
        .def(pybind11::init<const std::string &, int>())
        .def("decodeJpg", &SYSZUXav::decodeJpg)
        .def("open",&SYSZUXav::open)
        .def("close",&SYSZUXav::close);

    pybind11::class_<Matrix>(m, "Matrix", pybind11::buffer_protocol())
    .def_buffer([](Matrix &m) -> pybind11::buffer_info {
        return pybind11::buffer_info(
            m.data(),                               /* Pointer to buffer */
            sizeof(uint8_t),                          /* Size of one scalar */
            pybind11::format_descriptor<uint8_t>::format(), /* Python struct-style format descriptor */
            3,                                      /* Number of dimensions */
            { m.h(), m.w(), m.c()},                 /* Buffer dimensions */
            { sizeof(uint8_t) * m.w() * m.c(), sizeof(uint8_t) * m.c(), sizeof(uint8_t) }            /* Strides (in bytes) for each index */
        );
    });
}
