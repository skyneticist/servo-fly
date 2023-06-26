#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <thread>

#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

#include "CamStream.h"
#include "motor_controller.cpp"
#include "CatInfo.h"
#include "src/arg_keys.cpp"
#include "fps.h"
#include "src/dnn_utils.cpp"
#include "src/servo_utils.cpp"

CatInfo info;
static ServoDoor door;

// Create a window
std::string kWin = create_named_window();

int main(int argc, char **argv)
{
  Fps fps;

  CamStream cs;
  cv::Mat *frame;

  cv::CommandLineParser parser(argc, argv, keys);
  parser = cv::CommandLineParser(argc, argv, keys);
  parser.about("Use this script to run object detection deep learning networks using OpenCV.");

  // Set initial confidence
  float confThreshold, nmsThreshold;
  int initialConf = (int)(confThreshold * 100);
  // createTrackbar("Confidence threshold, %", kWinName, &initialConf, 99, callback);

  // Probably need to use a queue for passing data
  std::thread flag_thread(thtask_handle_update_flags, false);

  // Grab parsed model, config paths
  DnnPaths dnn_paths = get_dnn_paths(parser);

  // Load a model
  cv::dnn::Net net = cv::dnn::readNet(dnn_paths.model, dnn_paths.config, parser.get<std::string>("framework"));
  set_dnn_model(parser, net);

  std::vector<std::string> class_names = set_dnn_classes(parser);

  while (1)
  {
    cv::Mat image = *cs.read();
    detect_cats(net, class_names, image, kWin);

    int k = cv::waitKey(10);
    if (k == 113)
    {
      break;
    }
  }
  cv::destroyAllWindows();
}
