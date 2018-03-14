#ifndef COLORHISTCOMPARER_H
#define COLORHISTCOMPARER_H

#include "opencv2/highgui/highgui.hpp"
#include <vector>

class ColorHistComparer
{
public:
	//ColorHistComparer(){} //默认构造函数
	//将颜色空间划分为binNums等分，设置颜色比较模式为colorMode，距离模式(包含 默认构造函数)
	ColorHistComparer(int binNums = 16, int colorMode = 2, int distMode = 1);

	//三种颜色直方图距离计算方式: 1 v 1; 1 v N; M v N(同时送入源图和目标图)
	double ComputeColorHistDistance(const cv::Mat& srcImg, const cv::Mat& dstImg);
	std::vector<double> ComputeColorHistDistance(const cv::Mat& srcImg, const std::vector<cv::Mat>& dstImgs);
	std::vector<std::vector<double> > ComputeColorHistDistance(const std::vector<cv::Mat>& srcImgs,const std::vector<cv::Mat>& dstImgs);

	//有时存在原图已知，但目标图需要间隔送入比较器，为了减少计算，可实现送入源图，在比较时刻再送入目标图进行比较
	void SetSrcImage(const cv::Mat& srcImg);
	double ComputeColorHistDistance(const cv::Mat& dstImg);

private:
	//计算多通道颜色直方图
	void computeMultiHist(const cv::Mat& img, std::vector<cv::Mat>& hists, int binNums, int colorMode);
	//rangeBottom,rangeTop 单通道取值的下、上限
	cv::Mat computeHist(const cv::Mat &img, int rangeBottom, int rangeTop,int binNums);
	//计算巴氏距离
	double BhatDistance(const std::vector<cv::Mat>& hists1, const std::vector<cv::Mat>& hists2);

private:
	int m_binNums; // bin的个数
	int m_colorMode; //颜色模式 gray:1, rgb:2, hsv:3
	int m_distMode; //距离模式 巴氏距离：1
	//存储原图的多通道颜色直方图
	std::vector<cv::Mat> m_srcHists;
};

/*
class HistComputer
{
public:
	HistComputer(int binNums = 16, int colorMode = 2);
	HistComputer(const cv::Mat& img, int binNums = 16, int colorMode = 2);
    //设置图片
	void setImage(const cv::Mat& img);

    
    //distMode : 距离模式 - 巴氏距离：1
    float compareHist(const cv::Mat& img, int distMode = 1);
	std::vector<std::pair<cv::Rect, float> > ovelapThresh(const cv::Mat& img, const std::vector<cv::Rect>& rects, float thresh = 0.7, int distMode = 1);
private:
	//计算多通道颜色直方图
	void computeMultiHist(const cv::Mat& img, std::vector<cv::Mat>& hists, int binNums, int colorMode);
    //rangeBottom,rangeTop 单通道取值的下、上限
    cv::Mat computeHist(const cv::Mat &img, int rangeBottom, int rangeTop,int binNums);
    //计算巴氏距离
    float BhatDistance(std::vector<cv::Mat> hists1, std::vector<cv::Mat> hists2);
private:
    cv::Mat m_img; //图片
    int m_binNums; // bin的个数
    int m_colorMode; //颜色模式 gray:1, rgb:2, hsv:3
    //由于颜色模式不同，可能有多通道直方图
    std::vector<cv::Mat> m_hists;
};*/

#endif //COLORHISTCOMPARER_H
