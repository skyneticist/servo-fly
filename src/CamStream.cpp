#include <iostream>
#include <thread>
#include <mutex>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/videoio.hpp>

#include "CamStream.h"

CamStream& CamStream::operator=(const CamStream& o) {
  if (this != &o) {
    frame = o.frame;
  }
  return *this;
}

void CamStream::init() {
  this->stream.open(this->device_id, this->api_id);
  if (!this->stream.isOpened()) {
    std::cerr << "(!)ERROR: Unable to open camera\n";
    exit(EXIT_FAILURE);
  }

  this->stream.read(this->frame);
  if (this->frame.empty()) {
    std::cerr << "(!)ERROR1: Empty frame was grabbed\n";
    return;
  }
}

CamStream::CamStream() {
  this->init();
}

CamStream::CamStream(int device, int api) : device_id(device), api_id(api) {
  this->init();
}

void CamStream::start() {
  std::thread thread_1(&CamStream::update, this);
  thread_1.detach();
}

void CamStream::update() {
  std::cout << "Updating frames..." << std::endl
    << "Press any key to close program" << std::endl;

  while (1) {
    if (this->is_stopped)
      return;

    this->mut.lock();

    this->stream.read(this->frame);
    if (this->frame.empty()) {
      std::cerr << "(!)Error2: Empty frame grabbed\n";
      return;
    }

    this->mut.unlock();
  }
}

cv::Mat *CamStream::read() {
  return &this->frame;
}

void CamStream::stop() {
  this->is_stopped = true;
}
