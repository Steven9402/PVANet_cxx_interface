/**
  * @Copyright (c) 2016 by Jp Hu
  * @Date 2016-10-05
  * @Institude College of Surveying & Geo-Informatics, Tongji University
  * @Function
  */
#include "ColorHistComparer.h"
#include <iostream>
#include <string>
#include <math.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define  INVALID_VALUE 99999

using namespace std;
using namespace cv;

//Debug
void printMat(const Mat mat)
{
    for(int i=0; i<mat.rows; ++i)
    {
        for(int j=0; j<mat.cols; ++j)
        {
            cout << mat.at<float>(i,j) << " ";
        }
        cout << endl;
    }
}

// aux
float MatSum(Mat mat)
{
    float sum = 0.0;
    for(int i=0; i<mat.rows; ++i)
    {
        for(int j=0; j<mat.cols; ++j)
        {
            sum += mat.at<float>(i,j);
        }
    }
    return sum;
}


//public

//将颜色空间划分为binNums等分，设置颜色比较模式为colorMode，距离模式
ColorHistComparer::ColorHistComparer(int binNums, int colorMode, int distMode)
    :m_binNums(binNums), m_colorMode(colorMode), m_distMode(distMode)
{}

double ColorHistComparer::ComputeColorHistDistance(const cv::Mat& srcImg, const cv::Mat& dstImg)
{
    std::vector<Mat> srcHists, dstHists;
    computeMultiHist(srcImg, srcHists, m_binNums, m_colorMode);
    computeMultiHist(dstImg, dstHists, m_binNums, m_colorMode);
    double dist = INVALID_VALUE;
    if(m_distMode == 1)
       dist = BhatDistance(srcHists, dstHists);
    return dist;
}
std::vector<double> ColorHistComparer::ComputeColorHistDistance(const cv::Mat& srcImg, const std::vector<cv::Mat>& dstImgs)
{
    vector<double> distRes;
    std::vector<Mat> srcHists,dstHists;
    computeMultiHist(srcImg, srcHists, m_binNums, m_colorMode);
    for(int i=0; i<dstImgs.size(); ++i)
    {
        computeMultiHist(dstImgs[i], dstHists, m_binNums, m_colorMode);
        if(m_distMode == 1)
            distRes.push_back(BhatDistance(srcHists,dstHists));
    }
    return distRes;
}
std::vector<std::vector<double> > ColorHistComparer::ComputeColorHistDistance(const std::vector<cv::Mat>& srcImgs,const std::vector<cv::Mat>& dstImgs)
{
    vector<vector<double> > distRes;
    std::vector<Mat> srcHists,dstHists;
    for(int i=0; i<srcImgs.size(); ++i)
    {
        computeMultiHist(srcImgs[i], srcHists, m_binNums, m_colorMode);
        vector<double> tmpVec;
        for(int j=0; j<dstImgs.size(); ++j)
        {
            computeMultiHist(dstImgs[j], dstHists, m_binNums, m_colorMode);
            tmpVec.push_back(BhatDistance(srcHists,dstHists));
        }
        if(m_distMode==1)
            distRes.push_back(tmpVec);
    }
    return distRes;
}

///
void ColorHistComparer::SetSrcImage(const cv::Mat& srcImg)
{
    computeMultiHist(srcImg, m_srcHists, m_binNums, m_colorMode);
}
double ColorHistComparer::ComputeColorHistDistance(const cv::Mat& dstImg)
{
    //存储目标图的多通道颜色直方图
    std::vector<cv::Mat> dstHists;
    computeMultiHist(dstImg, dstHists, m_binNums, m_colorMode);
    double dist = INVALID_VALUE;
    if(m_distMode == 1)
        dist = BhatDistance(m_srcHists, dstHists);
    return dist;
}

//private

// 计算多通道颜色直方图
void ColorHistComparer::computeMultiHist(const cv::Mat& img, std::vector<cv::Mat> &hists, int binNums, int colorMode)
{
    hists.clear();
    //单通道 gray
    if(img.channels() == 1)
    {
        Mat hist = computeHist(img, 0, 256, binNums);
        hists =  vector<Mat>(1, hist);
    }
    else if(img.channels() == 3)
    {
        Mat tempImg;
        //图片复制
        img.copyTo(tempImg);
        if(colorMode == 1)
        {
            cvtColor(tempImg,tempImg,CV_RGB2GRAY);
            Mat hist = computeHist(tempImg, 0, 256, binNums);
            hists = vector<Mat>(1, hist);
        }
        else if(colorMode == 2)
        {
            vector<Mat> rgb;
            //通道分割
            split(tempImg, rgb);
            Mat rHist,gHist,bHist;
            //这里要注意rgb的顺序,到时候需要测试
            rHist = computeHist(rgb.at(2), 0, 256, binNums);
            gHist = computeHist(rgb.at(1), 0, 256, binNums);
            bHist = computeHist(rgb.at(0), 0, 256, binNums);
            hists.push_back(rHist);
            hists.push_back(gHist);
            hists.push_back(bHist);
        }
        else if(colorMode == 3)
        {
            cvtColor(tempImg,tempImg,CV_RGB2HSV);
            vector<Mat> hsv;
            //通道分割
            split(tempImg, hsv);
            Mat hHist,sHist;
            //这里要注意rgb的顺序,到时候需要测试
            hHist = computeHist(hsv.at(0), 0, 180, binNums);
            sHist = computeHist(hsv.at(1), 0, 256, binNums);
            hists.push_back(hHist);
            hists.push_back(sHist);
        }
    }
    else
    {
        cout << "Sorry, We can't process this image!" << endl;
        cout << "It's not a gray or RGB image!" << endl;
    }
}

Mat ColorHistComparer::computeHist(const Mat &img, int rangeBottom, int rangeTop, int binNums)
{
    //单通道直方图求解
    float range[] = {rangeBottom, rangeTop};
    const float* histRange = {range};
    bool uniform = true;
    bool accumulate = false;
    cv::Mat hist;
    //计算直方图
    cv::calcHist(&img, 1, 0, cv::Mat(), hist, 1, &binNums, &histRange, uniform, accumulate);
    //归一化
    //cv::normalize(hist, hist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
    for(int i=0; i<hist.rows; ++i)
    {
        for(int j=0; j<hist.cols; ++j)
        {
            hist.at<float>(i,j) = hist.at<float>(i,j) / (1.0*img.rows*img.cols);
        }
    }
    return hist;
}

double ColorHistComparer::BhatDistance(const std::vector<cv::Mat>& hists1, const std::vector<cv::Mat>& hists2)
{

    int len1 = hists1.size();
    int len2 = hists2.size();
    if(len1!=len2)
    {
        cout << "The size of two histogram is not same!" << endl;
    }
    vector<double> sum1(len1,0);
    vector<double> sum2(len2,0);
    //对各通道求和
    for(int i=0; i<hists1.size(); ++i)
    {
        sum1[i] = MatSum(hists1.at(i));
    }
    for(int i=0; i<hists2.size(); ++i)
    {
        sum2[i] = MatSum(hists2.at(i));
    }
    //求解巴氏距离的分子、分母
    vector<double> sumup(len1,0);
    vector<double> sumdown(len1,0);
    //求解分子
    for(int i=0; i<len1; ++i)
    {
        Mat mulMat = hists1[i].mul(hists2[i]);
        cv::sqrt(mulMat, mulMat);
        sumup[i] = MatSum(mulMat);
    }
    //求解分母
    for(int i=0; i<len1; ++i)
    {
        sumdown[i] = std::sqrt(sum1[i]*sum2[i]);
    }
    //
    float distSum = 0;
    for(int i=0; i<len1; ++i)
    {
        distSum += std::sqrt(1-sumup[i]/sumdown[i]);
    }
    //巴氏距离
    return distSum/static_cast<double>(len1);
}


////////////////////////////////////////////////////
/*
HistComputer::HistComputer(int binNums, int colorMode)
	:m_binNums(binNums), m_colorMode(colorMode){}

HistComputer::HistComputer(const cv::Mat& img, int binNums, int colorMode)
	:m_binNums(binNums), m_colorMode(colorMode)
{
	img.copyTo(m_img);
	computeMultiHist(m_img, m_hists, m_binNums, m_colorMode);
}

void HistComputer::setImage(const cv::Mat& img)
{
	img.copyTo(m_img);
	computeMultiHist(m_img, m_hists, m_binNums, m_colorMode);
}

Mat HistComputer::computeHist(const Mat &img, int rangeBottom, int rangeTop, int binNums)
{
    //单通道直方图求解
    float range[] = {rangeBottom, rangeTop};
    const float* histRange = {range};
    bool uniform = true;
    bool accumulate = false;
    cv::Mat hist;
    //计算直方图
    cv::calcHist(&img, 1, 0, cv::Mat(), hist, 1, &binNums, &histRange, uniform, accumulate);
    //归一化
    //cv::normalize(hist, hist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
    for(int i=0; i<hist.rows; ++i)
    {
        for(int j=0; j<hist.cols; ++j)
        {
            hist.at<float>(i,j) = hist.at<float>(i,j) / (1.0*img.rows*img.cols);
        }
    }
    return hist;
}
vector<pair<Rect, float> > HistComputer::ovelapThresh(const Mat& img, const vector<cv::Rect>& rects, float thresh, int distMode)
{
	vector<pair<Rect, float> > res;
	for (int i = 0; i < rects.size(); ++i)
	{
		vector<Mat> hists;
		float simi = compareHist(img(rects[i]), distMode);
		if (simi >= thresh)
			res.push_back(pair<Rect, float>(rects[i], simi));
	}
	if (!res.size())
		return vector<pair<Rect, float> >();
	//对结果按照相似度从高到底进行排序
	for (int j = 1; j < res.size(); ++j)
	{
		pair<Rect, float> obj = res[j];
		int i = j - 1;
		while (i >= 0 && res[i].second < obj.second)
		{
			res[i + 1] = res[i];
			i = i - 1;
		}
		res[i + 1] = obj;
	}
	
	return res;
}
// 计算多通道颜色直方图
void HistComputer::computeMultiHist(const cv::Mat& img, std::vector<cv::Mat> &hists, int binNums, int colorMode)
{
	//单通道 gray
    if(img.channels() == 1)
    {
        Mat hist = computeHist(img, 0, 256, binNums);
        hists =  vector<Mat>(1, hist);
    }
    else if(img.channels() == 3)
    {
        Mat tempImg;
        //图片复制
        img.copyTo(tempImg);
        if(colorMode == 1)
        {
            cvtColor(tempImg,tempImg,CV_RGB2GRAY);
            Mat hist = computeHist(tempImg, 0, 256, binNums);
            hists = vector<Mat>(1, hist);
        }
        else if(colorMode == 2)
        {
            vector<Mat> rgb;
            //通道分割
            split(tempImg, rgb);
            Mat rHist,gHist,bHist;
            //这里要注意rgb的顺序,到时候需要测试
            rHist = computeHist(rgb.at(2), 0, 256, binNums);
            gHist = computeHist(rgb.at(1), 0, 256, binNums);
            bHist = computeHist(rgb.at(0), 0, 256, binNums);
            hists.push_back(rHist);
            hists.push_back(gHist);
            hists.push_back(bHist);
        }
        else if(colorMode == 3)
        {
            cvtColor(tempImg,tempImg,CV_RGB2HSV);
            vector<Mat> hsv;
            //通道分割
            split(tempImg, hsv);
            Mat hHist,sHist;
            //这里要注意rgb的顺序,到时候需要测试
            hHist = computeHist(hsv.at(0), 0, 180, binNums);
            sHist = computeHist(hsv.at(1), 0, 256, binNums);
            hists.push_back(hHist);
            hists.push_back(sHist);
        }
    }
    else
    {
        cout << "Sorry, We can't process this image!" << endl;
        cout << "It's not a gray or RGB image!" << endl;
    }
}

float HistComputer::compareHist(const cv::Mat& img, int distMode)
{
    //相似度与巴氏距离的关系
	vector<Mat> hists; 
	computeMultiHist(img, hists, m_binNums, m_colorMode);
    return 1 - BhatDistance(m_hists,hists);
}

float HistComputer::BhatDistance(std::vector<Mat> hists1, std::vector<Mat> hists2)
{

    int len1 = hists1.size();
    int len2 = hists2.size();
    if(len1!=len2)
    {
        cout << "The size of two histogram is not same!" << endl;
    }
    vector<float> sum1(len1,0);
    vector<float> sum2(len2,0);
    //对各通道求和
    for(int i=0; i<hists1.size(); ++i)
    {
//        printMat(hists1.at(i));
//        cout << endl;
        sum1[i] = MatSum(hists1.at(i));
    }
    for(int i=0; i<hists2.size(); ++i)
    {
//        printMat(hists2.at(i));
//        cout << endl;
        sum2[i] = MatSum(hists2.at(i));
    }
    //求解巴氏距离的分子、分母
    vector<float> sumup(len1,0);
    vector<float> sumdown(len1,0);
    //求解分子
    for(int i=0; i<len1; ++i)
    {
        Mat mulMat = hists1[i].mul(hists2[i]);
//        printMat(mulMat);
//        cout << endl;
        cv::sqrt(mulMat, mulMat);
//        printMat(mulMat);
//        cout << endl;
        sumup[i] = MatSum(mulMat);
    }
    //求解分母
    for(int i=0; i<len1; ++i)
    {
        sumdown[i] = std::sqrt(sum1[i]*sum2[i]);
    }
    //
    float distSum = 0;
    for(int i=0; i<len1; ++i)
    {
        distSum += std::sqrt(1-sumup[i]/sumdown[i]);
    }
    //巴氏距离
    return distSum/static_cast<double>(len1);
}
*/

