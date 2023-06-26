#ifndef CAMSTREAM_H
#define CAMSTREAM_H

#include <mutex>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

class CamStream {
  private:
    int api_id = cv::CAP_ANY;
    int device_id = 0;
    bool is_stopped = false;
    cv::VideoCapture stream;
    cv::Mat frame;
    std::mutex mut;

    CamStream& operator=(const CamStream& o);
    CamStream(const CamStream&) {};

    void init();

  public:
    CamStream();
    CamStream(int device, int api);
    void start();
    void update();
    void stop();
    cv::Mat *read();
};

#endif
