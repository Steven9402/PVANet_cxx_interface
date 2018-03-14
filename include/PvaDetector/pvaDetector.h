#ifndef PVADETECTOR_H
#define PVADETECTOR_H

#include "caffe/caffe.hpp"
#include <opencv2/core/core.hpp>
#include "detection.h"
#include <vector>

class PVADetector
{
public:
    PVADetector() {}
	//初始化
	//加载模型，根据classes，修改net_pt文件（类别数）
	void init(std::string net_pt, std::string net_weights,
		std::vector<std::string> classes);
	void setComputeMode(std::string mode = "cpu", int id = 0);
	void setThresh(float conf_thresh = 0.7, float nms_thresh = 0.3);
	std::vector<Detection> detect(cv::Mat img);
	//得到给定类别的目标
	std::vector<Detection> detect(cv::Mat img, std::string class_mask);
	std::vector<Detection> detect(cv::Mat img, std::set<std::string> class_mask_set);
	//显示函数
	void drawBox(cv::Mat img, std::vector<Detection> &dets);
	std::vector<Detection> overThresh(int* keep, int num_out, float* sorted_pred_cls, float CONF_THRESH, std::string className);

private:
	float iou(const float A[], const float B[]);
	void nms(int* keep_out, int* num_out, const float* boxes_host, int boxes_num,
		int boxes_dim, float nms_overlap_thresh);
	void bbox_transform_inv(const int num, const float* box_deltas,
		const float* pred_cls, float* boxes, float* pred, int img_height, int img_width);
	void boxes_sort(int num, const float* pred, float* sorted_pred);
private:
	std::vector<std::string> m_classes;
	std::shared_ptr<caffe::Net<float> > m_net;
	float m_confThresh;
	float m_nmsThresh;

	const int SCALE_MULTIPLE_OF = 32;
	const int MAX_SIZE = 1280;//2000;
	const int SCALES = 640;
};

#endif //PVADETECTOR_H

