#ifndef DISTANCECOMPARER_H
#define DISTANCECOMPARER_H

#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
class DistanceComparer
{
public:
    enum ReferPoint {CENTER, LEFTTOP};
    DistanceComparer(int distMode = 1);
    //点
    double ComputeDistance(const cv::Point& srcPt, const cv::Point& dstPt);
    std::vector<double> ComputeDistance(const cv::Point& srcPt, const std::vector<cv::Point>& dstPts);
    std::vector<std::vector<double> > ComputeDistance(const std::vector<cv::Point>& srcPts, const std::vector<cv::Point>& dstPts);
    //矩形
    double ComputeDistance(const cv::Rect& srcRt, const cv::Rect& dstRt, ReferPoint rp = CENTER);
    std::vector<double> ComputeDistance(const cv::Rect& srcRt, const std::vector<cv::Rect>& dstRts, ReferPoint rp = CENTER);
    std::vector<std::vector<double> > ComputeDistance(const std::vector<cv::Rect>& srcRts, const std::vector<cv::Rect>& dstRts, ReferPoint rp = CENTER);

    //正则化
    //注意：此处的正则化，并不严格缩放到[0,1];
    //举例而言：对于欧式距离，只有当dist<=thresh时，值才会不大于1，
    // normalizeMode = 1 : dist/thresh ; normalizeMode = 2 : sqrt(dist^2/(2*thresh^2)) -> alias of thresh is sigma
    double Normalize(double dist, double thresh = 50, int noramlizeMode = 1);
    std::vector<double> Normalize(const std::vector<double>& dists, double thresh = 50, int noramlizeMode = 1);
    std::vector<std::vector<double> > Normalize(const std::vector<std::vector<double> >& distVecs, double thresh = 50, int noramlizeMode = 1);
private:
    cv::Point computeCenter(cv::Rect rect);

private:
    int m_distMode; //欧式距离：1
};

#endif //DISTANCECOMPARER_H