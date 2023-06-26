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
#include "arg_keys.cpp"
#include "Fps.h"
#include "servo_utils.cpp"

typedef struct
{
    static std::string model;
    static std::string config;
} DnnPaths;

DnnPaths get_dnn_paths(cv::CommandLineParser parser)
{
    DnnPaths dn_paths;

    // supposedly can use findFile() here
    dn_paths.model = parser.get<std::string>("model");
    dn_paths.model = parser.get<std::string>("config");
    return dn_paths;
}

void set_dnn_model(cv::CommandLineParser parser, cv::dnn::Net net)
{
    // Load a model
    net.setPreferableBackend(parser.get<int>("backend"));
    net.setPreferableTarget(parser.get<int>("target"));
    std::vector<std::string> outNames = net.getUnconnectedOutLayersNames();
}

std::vector<std::string> set_dnn_classes(cv::CommandLineParser parser)
{
    std::vector<std::string> class_names;
    std::ifstream ifs(std::string("../../input/classification_classes_ILSVRC2012.txt").c_str());

    // Open file with classes names
    std::vector<std::string> classes;
    if (parser.has("classes"))
    {
        std::string file = parser.get<std::string>("classes");
        std::ifstream ifs(file.c_str());
        if (!ifs.is_open())
            CV_Error(cv::Error::StsError, "File " + file + " not found");
        std::string line;
        while (std::getline(ifs, line))
        {
            classes.push_back(line);
        }
    }
    return classes;
}

void detect_cats(cv::dnn::Net net, std::vector<std::string> class_names, cv::Mat image, std::string kWin)
{
    cv::Mat blob = cv::dnn::blobFromImage(image, 1.0, cv::Size(300, 300), cv::Scalar(127.5, 127.5, 127.5), true, false);
    net.setInput(blob);
    cv::Mat output = net.forward();
    cv::Mat detectionMat(output.size[2], output.size[3], CV_32F, output.ptr<float>());

    for (int i = 0; i < detectionMat.rows; i++)
    {
        int class_id = detectionMat.at<float>(i, 1);
        float confidence = detectionMat.at<float>(i, 2);

        if (confidence > 0.4)
        {
            int box_x = static_cast<int>(detectionMat.at<float>(i, 3) * image.cols);
            int box_y = static_cast<int>(detectionMat.at<float>(i, 4) * image.rows);
            int box_width = static_cast<int>(detectionMat.at<float>(i, 5) * image.cols - box_x);
            int box_height = static_cast<int>(detectionMat.at<float>(i, 6) * image.rows - box_y);

            compute_distance(box_width + box_height);

            rectangle(image, cv::Point(box_x, box_y), cv::Point(box_x + box_width, box_y + box_height), cv::Scalar(255, 255, 255), 2);
            putText(image, class_names[class_id - 1].c_str(), cv::Point(box_x, box_y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 1);
        }
    }
    // might not work here ?
    imshow(kWin, image);
}
