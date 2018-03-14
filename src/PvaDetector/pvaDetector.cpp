/**
* @Copyright (c) 2016 by Jp Hu
* @Date 2017-06-27
* @Institude College of Surveying & Geo-Informatics, Tongji University
* @Function
*/

#include <iostream>
#include "pvaDetector.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


using namespace caffe;
using namespace cv;
using namespace std;


//Using for box sort
struct Info
{
    float score;
    const float* head;
};
bool scoreCompare(const Info& Info1, const Info& Info2)
{
    return Info1.score > Info2.score;
}


void PVADetector::init(std::string def, std::string net,
    std::vector<std::string> classes)
{
    m_classes = classes;
    //加载网络模型
    m_net = std::shared_ptr<Net<float> >(new Net<float>(def, caffe::TEST));
    m_net->CopyTrainedLayersFrom(net);
}
void PVADetector::setComputeMode(std::string mode, int id)
{
    if (mode == "gpu")
    {
        Caffe::SetDevice(id);

        Caffe::set_mode(Caffe::GPU);
    }
    else
        Caffe::set_mode(Caffe::CPU);
}
void PVADetector::setThresh(float conf_thresh, float nms_thresh)
{
    m_confThresh = conf_thresh;
    m_nmsThresh = nms_thresh;
}

std::vector<Detection> PVADetector::detect(cv::Mat img)
{
    int rpn_num;
    float *pred = NULL;
    if (img.empty())
    {
        cout << "error" << endl;
    }

    int im_size_min = (std::min)(img.rows, img.cols);
    int im_size_max = (std::max)(img.rows, img.cols);
    float im_scale = float(SCALES) / im_size_min;// if im_scale have many value, it will support muti-scale detection
    if (round(im_scale * im_size_max) > MAX_SIZE)
        im_scale = float(MAX_SIZE) / im_size_max;
    // Make width and height be multiple of a specified number
    float im_scale_x = std::floor(img.cols * im_scale / SCALE_MULTIPLE_OF) * SCALE_MULTIPLE_OF / img.cols;
    float im_scale_y = std::floor(img.rows * im_scale / SCALE_MULTIPLE_OF) * SCALE_MULTIPLE_OF / img.rows;
    // keep image size less than 2000 * 640
    int height = int(img.rows * im_scale_y);
    int width = int(img.cols * im_scale_x);

    cv::Mat cv_resized;
    float im_info[6];	// (cv_resized)image's height, width, scale(equal 1 or 1/max_scale)
    //float data_buf[height*width * 3];	// each pixel value in cv_resized
    //float *data_buf = new float[height*width * 3];
    float *boxes = NULL;

    const float* bbox_delt;
    const float* rois;
    const float* pred_cls;

    //cv::Mat cv_new;
    //img.convertTo(cv_new, CV_32FC3);
    //cv_new = cv_new - Scalar_<float>(102.9801, 115.9465, 122.7717);
    cv::Mat cv_new(img.rows, img.cols, CV_32FC3, Scalar(0, 0, 0));
    for (int h = 0; h < img.rows; ++h)
    {
        for (int w = 0; w < img.cols; ++w)
        {
            cv_new.at<cv::Vec3f>(cv::Point(w, h))[0] = float(img.at<cv::Vec3b>(cv::Point(w, h))[0]) -float(102.9801);
            cv_new.at<cv::Vec3f>(cv::Point(w, h))[1] = float(img.at<cv::Vec3b>(cv::Point(w, h))[1]) -float(115.9465);
            cv_new.at<cv::Vec3f>(cv::Point(w, h))[2] = float(img.at<cv::Vec3b>(cv::Point(w, h))[2]) -float(122.7717);

        }
    }

    cv::resize(cv_new, cv_resized, cv::Size(width, height),0,0);

    im_info[0] = cv_resized.rows;
    im_info[1] = cv_resized.cols;
    im_info[2] = im_scale_x;
    im_info[3] = im_scale_y;
    im_info[4] = im_scale_x;
    im_info[5] = im_scale_y;

    // sequence data
    float *data_buf = new float[height*width * 3];
    //float *data_buf = (float*)malloc(height*width * 3* sizeof(float));
    for (int h = 0; h < height; ++h)
    {
        for (int w = 0; w < width; ++w)
        {
            data_buf[(0 * height + h)*width + w] = float(cv_resized.at<cv::Vec3f>(cv::Point(w, h))[0]);
            data_buf[(1 * height + h)*width + w] = float(cv_resized.at<cv::Vec3f>(cv::Point(w, h))[1]);
            data_buf[(2 * height + h)*width + w] = float(cv_resized.at<cv::Vec3f>(cv::Point(w, h))[2]);
        }
    }
    int channel_nums = cv_resized.channels();
    m_net->blob_by_name("data")->Reshape(1, channel_nums, height, width);
    m_net->Reshape();
    //m_net->blob_by_name("data")->set_cpu_data(data_buf);
    float* input_data = m_net->blob_by_name("data")->mutable_cpu_data();
    vector<cv::Mat> input_channels;
    for (int i = 0; i < channel_nums; ++i)
    {
        cv::Mat channel(height, width, CV_32FC1, input_data);
        input_channels.push_back(channel);
        input_data += height*width;
    }
    cv::split(cv_resized, input_channels);
    m_net->blob_by_name("im_info")->set_cpu_data(im_info);

    m_net->ForwardFrom(0);

    bbox_delt = m_net->blob_by_name("bbox_pred")->cpu_data();	// bbox_delt is offset ratio of bounding box, get by bounding box regression

    rpn_num = m_net->blob_by_name("rois")->num();	// number of region proposals
    rois = m_net->blob_by_name("rois")->cpu_data();	// scores and bounding boxes coordinate
    pred_cls = m_net->blob_by_name("cls_prob")->cpu_data();
    boxes = new float[rpn_num * 4];

    for (int n = 0; n < rpn_num; n++)
    {
        for (int c = 0; c < 4; c++)
        {
            // resize function may increase img size, if that, we should increase bounding boxes coordinate
            boxes[n * 4 + c] = rois[n * 5 + c + 1] / im_info[c + 2];
        }
    }
    pred = new float[rpn_num * 5 * m_classes.size()];
    bbox_transform_inv(rpn_num, bbox_delt, pred_cls, boxes, pred, img.rows, img.cols);

    float *pred_per_class = NULL;
    float *sorted_pred_cls = NULL;
    int *keep = NULL;
    int num_out;

    pred_per_class = new float[rpn_num * 5];
    sorted_pred_cls = new float[rpn_num * 5];
    keep = new int[rpn_num];	// index of bounding box?
    vector<Detection> dets;
    for (int i = 1; i < m_classes.size(); i++)		// i = 0, means background
    {
        for (int j = 0; j< rpn_num; j++)
        {
            for (int k = 0; k<5; k++)
                pred_per_class[j * 5 + k] = pred[(i*rpn_num + j) * 5 + k];
        }
        boxes_sort(rpn_num, pred_per_class, sorted_pred_cls);
        // you can read vis_detection() function to understand these variables
        // @num_out: after nms, the number of bounding box
        // @keep: after nms, the index of bounding box in sorted_pred_cls
        nms(keep, &num_out, sorted_pred_cls, rpn_num, 5, m_nmsThresh);
        //vis_detections(img, keep, num_out, sorted_pred_cls, m_confThresh, m_classes[i]);
        vector<Detection> singleDets = overThresh(keep, num_out, sorted_pred_cls, m_confThresh, m_classes[i]);
        dets.insert(dets.end(), singleDets.begin(), singleDets.end());
    }

    delete data_buf;
    //free(data_buf);
    data_buf = NULL;
    delete boxes;
    boxes = NULL;
    delete keep;
    keep = NULL;
    delete pred_per_class;
    pred_per_class = NULL;
    delete sorted_pred_cls;
    sorted_pred_cls = NULL;
    delete pred;
    pred = NULL;

    return dets;
}
std::vector<Detection> PVADetector::detect(cv::Mat img, std::string class_mask)
{
    std::vector<Detection> dets = detect(img);
    std::vector<Detection> mask_dets;
    for (int i = 0; i < dets.size(); ++i)
    {
        if (dets[i].getClass() == class_mask)
        {
            mask_dets.push_back(dets[i]);
        }
    }
    if (!mask_dets.size())
        mask_dets = std::vector<Detection>();
    return mask_dets;
}

std::vector<Detection> PVADetector::detect(cv::Mat img, std::set<std::string> class_mask_set)
{
    std::vector<Detection> dets = detect(img);
    std::vector<Detection> mask_dets;
    for (int i = 0; i < dets.size(); ++i)
    {
        if (class_mask_set.count(dets[i].getClass()) == 1)
        {
            mask_dets.push_back(dets[i]);
        }
    }
    if (!mask_dets.size())
        mask_dets = std::vector<Detection>();
    return mask_dets;
}

void PVADetector::drawBox(cv::Mat img, std::vector<Detection> &dets)
{
    for (int i = 0; i<dets.size(); ++i)
    {
        rectangle(img, dets[i].getRect(), cv::Scalar(0, 0, 255), 2);
        std::ostringstream os;
        os.precision(2); //保留2位有效数字
        os << dets[i].getScore();
        std::string showTxt = dets[i].getClass(); //+ "," + os.str();;
        putText(img, showTxt, // Mat, text
                cv::Point(dets[i].getRect().x, dets[i].getRect().y - 2), //oriPt
                cv::FONT_HERSHEY_SIMPLEX, 1, //Font
                cv::Scalar(0,255, 255), 1); // color, linear width
//        putText(img, showTxt, // Mat, text
//            cv::Point(dets[i].getRect().x, dets[i].getRect().y - 2), //oriPt
//            cv::FONT_HERSHEY_SIMPLEX, 1, //Font
//            cv::Scalar(0, 0, 255), 1); // color, linear width
    }
}

float PVADetector::iou(const float A[], const float B[])
{
    if (A[0] > B[2] || A[1] > B[3] || A[2] < B[0] || A[3] < B[1]) {
        return 0;
    }

    // overlapped region (= box)
    const float x1 = max(A[0], B[0]);
    const float y1 = max(A[1], B[1]);
    const float x2 = min(A[2], B[2]);
    const float y2 = min(A[3], B[3]);

    // intersection area
    const float width = max((float)0, x2 - x1 + (float)1);
    const float height = max((float)0, y2 - y1 + (float)1);
    const float area = width * height;

    // area of A, B
    const float A_area = (A[2] - A[0] + (float)1) * (A[3] - A[1] + (float)1);
    const float B_area = (B[2] - B[0] + (float)1) * (B[3] - B[1] + (float)1);

    // IoU
    return area / (A_area + B_area - area);
}

void PVADetector::nms(int* keep_out, int* num_out, const float* boxes_host, int boxes_num,
    int boxes_dim, float nms_overlap_thresh)
{
    int count = 0;
    vector<char> is_dead(boxes_num);
    for (int i = 0; i < boxes_num; ++i)
    {
        is_dead[i] = 0;
    }
    for (int i = 0; i < boxes_num; ++i)
    {
        if (is_dead[i])
        {
            continue;
        }
        keep_out[count++] = i;
        for (int j = i + 1; j < boxes_num; ++j)
        {
            if (!is_dead[j] && iou(&boxes_host[i * 5], &boxes_host[j * 5])>nms_overlap_thresh)
            {
                is_dead[j] = 1;
            }
        }
    }
    *num_out = count;
    is_dead.clear();
}


/*
* ===  FUNCTION  ======================================================================
*         Name:  boxes_sort
*  Description:  Sort the bounding box according score
* =====================================================================================
*/
void PVADetector::boxes_sort(const int num, const float* pred, float* sorted_pred)
{
    vector<Info> my;
    Info tmp;
    for (int i = 0; i< num; i++)

    {
        tmp.score = pred[i * 5 + 4];

        tmp.head = pred + i * 5;

        my.push_back(tmp);
    }
    std::sort(my.begin(), my.end(), scoreCompare);
    for (int i = 0; i<num; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            sorted_pred[i * 5 + j] = my[i].head[j];	// sequence data
        }

    }
}

/*
* ===  FUNCTION  ======================================================================
*         Name:  bbox_transform_inv
*  Description:  Compute bounding box regression value
* =====================================================================================
*/
void PVADetector::bbox_transform_inv(int num, const float* box_deltas, const float* pred_cls, float* boxes, float* pred, int img_height, int img_width)
{
    float width, height, ctr_x, ctr_y, dx, dy, dw, dh, pred_ctr_x, pred_ctr_y, pred_w, pred_h;
    for (int i = 0; i< num; i++)
    {
        width = boxes[i * 4 + 2] - boxes[i * 4 + 0] + 1.0;
        height = boxes[i * 4 + 3] - boxes[i * 4 + 1] + 1.0;
        ctr_x = boxes[i * 4 + 0] + 0.5 * width;		// (ctr_x, ctr_y) is center coordinater of bounding box
        ctr_y = boxes[i * 4 + 1] + 0.5 * height;
        int class_num = m_classes.size();
        for (int j = 0; j< class_num; j++)
        {
            dx = box_deltas[(i*class_num + j) * 4 + 0];
            dy = box_deltas[(i*class_num + j) * 4 + 1];
            dw = box_deltas[(i*class_num + j) * 4 + 2];
            dh = box_deltas[(i*class_num + j) * 4 + 3];
            pred_ctr_x = ctr_x + width*dx;
            pred_ctr_y = ctr_y + height*dy;
            pred_w = width * exp(dw);
            pred_h = height * exp(dh);
            pred[(j*num + i) * 5 + 0] = (std::max)((std::min)(pred_ctr_x - 0.5* pred_w, (img_width - 1)*1.0), 0.0);	// avoid over boundary
            pred[(j*num + i) * 5 + 1] = (std::max)((std::min)(pred_ctr_y - 0.5* pred_h, (img_height - 1)*1.0), 0.0);
            pred[(j*num + i) * 5 + 2] = (std::max)((std::min)(pred_ctr_x + 0.5* pred_w, (img_width - 1)*1.0), 0.0);
            pred[(j*num + i) * 5 + 3] = (std::max)((std::min)(pred_ctr_y + 0.5* pred_h, (img_height - 1)*1.0), 0.0);
            pred[(j*num + i) * 5 + 4] = pred_cls[i*class_num + j];

        }
    }
}

vector<Detection> PVADetector::overThresh(int* keep, int num_out, float* sorted_pred_cls, float CONF_THRESH, string className)
{
    vector<Detection> dets;
    int i = 0;
    while (i < num_out)
    {
        if (sorted_pred_cls[keep[i] * 5 + 4] < CONF_THRESH)
            break;
        Detection det;
        det.setClass(className);
        det.setRect(Rect(Point(sorted_pred_cls[keep[i] * 5 + 0], sorted_pred_cls[keep[i] * 5 + 1]),
                         Point(sorted_pred_cls[keep[i] * 5 + 2], sorted_pred_cls[keep[i] * 5 + 3])));
        det.setScore(sorted_pred_cls[keep[i] * 5 + 4]);
        dets.push_back(det);
        i++;
    }
    return dets;
}

