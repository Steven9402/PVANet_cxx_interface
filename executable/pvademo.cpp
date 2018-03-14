//
// Created by xlz on 18-1-10.
//
#include <memory>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "pvaDetector.h"
#include <glog/logging.h>
#include <chrono>
#include "../src/OpenCVputTextChinese/OpencvText.h"
#include "../src/myfUtils/FileOperator.h"


using namespace std;
using namespace cv;

//#define  TEST_DIR
#ifdef TEST_DIR
int main(int argc, char* argv[])
{


    /**
     * pva demo for images in a folder
     * step1: set input_dir
     */

    char* inputdir = "/home/cuizhou/data/VOCdevkit2007/VOC2007/JPEGImages";

    // chinese putText
    string font_path = "../../src/OpenCVputTextChinese/STKAITI.TTF";
    std::shared_ptr<Cv310Text> put_text_ptr_ = std::make_shared<Cv310Text>(font_path.c_str(), 30);

    //PVANET Parameters
    string net_model = "../../models/all/all_comp.model";
    string net_pt = "../../models/all/all_comp.pt";
    std::vector<std::string> classes = { "__background__",
                                             "aeroplane", "bicycle", "bird", "boat",
                                             "bottle", "bus", "car", "cat", "chair",
                                             "cow", "diningtable", "dog", "horse",
                                             "motorbike", "person", "pottedplant",
                                             "sheep", "sofa", "train", "tvmonitor" };
    //初始化
    PVADetector detector;
    detector.init(net_pt, net_model, classes);
    detector.setThresh(0.7, 0.3);
    detector.setComputeMode("gpu", 0);



    std::vector<std::string> filenames = readFileList(inputdir);
    for (auto filename:filenames) {
        string imddir = inputdir;

        Mat srcImage = imread(imddir + "/" + filename);

        std::vector<Detection> dets = detector.detect(srcImage);
        detector.drawBox(srcImage, dets);

        cv::imshow("result", srcImage);
        cv::waitKey(0);
    }

    return 0;
}
#else
int main(int argc, char* argv[])
{


    /**
     * pva demo for one image
     * step1: set input_dir
     */

    char* inputdir = "/home/cuizhou/data/VOCdevkit2007/VOC2007/JPEGImages/000010.jpg";

    // chinese putText
    string font_path = "../../src/OpenCVputTextChinese/STKAITI.TTF";
    std::shared_ptr<Cv310Text> put_text_ptr_ = std::make_shared<Cv310Text>(font_path.c_str(), 30);

    //PVANET Parameters
    string net_model = "../../models/all/all_comp.model";
    string net_pt = "../../models/all/all_comp.pt";
    std::vector<std::string> classes = { "__background__",
                                             "aeroplane", "bicycle", "bird", "boat",
                                             "bottle", "bus", "car", "cat", "chair",
                                             "cow", "diningtable", "dog", "horse",
                                             "motorbike", "person", "pottedplant",
                                             "sheep", "sofa", "train", "tvmonitor" };
    //初始化
    PVADetector detector;
    detector.init(net_pt, net_model, classes);
    detector.setThresh(0.7, 0.3);
    detector.setComputeMode("gpu", 0);



    string imddir = inputdir;

    Mat srcImage = imread(imddir);

    std::vector<Detection> dets = detector.detect(srcImage);
    detector.drawBox(srcImage, dets);

    cv::imshow("result", srcImage);
    cv::waitKey(0);


    return 0;
}


#endif

