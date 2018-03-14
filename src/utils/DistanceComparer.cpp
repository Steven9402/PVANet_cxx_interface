#include "DistanceComparer.h"
using namespace std;
using namespace cv;

#define INVALID_VALUE 99999

DistanceComparer::DistanceComparer(int distMode)
    :m_distMode(distMode)
{}

//点
double DistanceComparer::ComputeDistance(const cv::Point& srcPt, const cv::Point& dstPt)
{
    if(m_distMode == 1)
        return sqrt((srcPt.x-dstPt.x)*(srcPt.x-dstPt.x)+(srcPt.y-dstPt.y)*(srcPt.y-dstPt.y));
    else
        return INVALID_VALUE;
}
std::vector<double> DistanceComparer::ComputeDistance(const cv::Point& srcPt, const std::vector<cv::Point>& dstPts)
{
    vector<double> distVec;
    for(int i=0; i<dstPts.size(); ++i)
    {
        double tmpDist = ComputeDistance(srcPt, dstPts[i]);
        distVec.push_back(tmpDist);
    }
    return distVec;
}
std::vector<std::vector<double> > DistanceComparer::ComputeDistance(const std::vector<cv::Point>& srcPts, const std::vector<cv::Point>& dstPts)
{
    vector<vector<double> > distVecs;
    for(int i=0; i<srcPts.size(); ++i)
    {
        vector<double> tmpVec;
        for(int j=0; j<dstPts.size(); ++j)
        {
            double tmpDist = ComputeDistance(srcPts[i], dstPts[j]);
            tmpVec.push_back(tmpDist);
        }
        distVecs.push_back(tmpVec);
    }
    return distVecs;
}
//矩形
double DistanceComparer::ComputeDistance(const cv::Rect& srcRt, const cv::Rect& dstRt, ReferPoint rp)
{
    double dist = INVALID_VALUE;
    if(rp == CENTER)
    {
        Point srcCenPt = computeCenter(srcRt);
        Point dstCenPt = computeCenter(dstRt);
        dist = ComputeDistance(srcCenPt, dstCenPt);
    }
    else if(rp == LEFTTOP)
    {
        dist = ComputeDistance(Point(srcRt.x, srcRt.y), Point(dstRt.x, dstRt.y));
    }
    return dist;
}
std::vector<double> DistanceComparer::ComputeDistance(const cv::Rect& srcRt, const std::vector<cv::Rect>& dstRts, ReferPoint rp)
{
//    double dist = INVALID_VALUE;
    vector<double> distVec;
    if(rp == CENTER)
    {
        Point srcCenPt = computeCenter(srcRt);
        for(int i=0; i<dstRts.size(); ++i)
        {
            Point dstCenPt = computeCenter(dstRts[i]);
            distVec.push_back(ComputeDistance(srcCenPt, dstCenPt));
        }
    }
    else if(rp == LEFTTOP)
    {
        for(int i=0; i<dstRts.size(); ++i)
        {
            distVec.push_back(ComputeDistance(Point(srcRt.x, srcRt.y), Point(dstRts[i].x, dstRts[i].y)));
        }
    }
    return distVec;
}
std::vector<std::vector<double> > DistanceComparer::ComputeDistance(const std::vector<cv::Rect>& srcRts, const std::vector<cv::Rect>& dstRts, ReferPoint rp) {
    vector<vector<double> > distVecs;
    if (rp == CENTER)
    {
        for(int j=0; j<srcRts.size(); ++j)
        {
            vector<double> tmpVec;
            Point srcCenPt = computeCenter(srcRts[j]);
            for(int i=0; i<dstRts.size(); ++i)
            {
                Point dstCenPt = computeCenter(dstRts[i]);
                tmpVec.push_back(ComputeDistance(srcCenPt, dstCenPt));
            }
            distVecs.push_back(tmpVec);
        }
    }
    else if (rp == LEFTTOP)
    {
        for(int j=0; j<srcRts.size(); ++j)
        {
            vector<double> tmpVec;
            for(int i=0; i<dstRts.size(); ++i)
            {
                tmpVec.push_back(ComputeDistance(Point(srcRts[j].x, srcRts[j].y), Point(dstRts[i].x, dstRts[i].y)));
            }
            distVecs.push_back(tmpVec);
        }
    }
    return distVecs;
}

double DistanceComparer::Normalize(double dist, double thresh, int noramlizeMode)
{
    double normDist = INVALID_VALUE;
    if(m_distMode == 1)
    {
        if(noramlizeMode == 1)
            normDist = dist/thresh;
        else if(noramlizeMode == 2)
        {
            normDist = std::sqrt(dist*dist/(2*thresh*thresh));;
        }
    }
    return normDist; //目前只定义了m_dist一种模式
}
std::vector<double> DistanceComparer::Normalize(const std::vector<double>& dists, double thresh, int noramlizeMode)
{
    vector<double> normDistVec;
    if(m_distMode == 1)
    {
        for(int i=0; i<dists.size(); ++i)
        {
            double normDist = INVALID_VALUE;
            if(noramlizeMode == 1)
                normDist = dists[i]/thresh;
            else if(noramlizeMode == 2)
            {
                normDist = std::sqrt(dists[i]*dists[i]/(2*thresh*thresh));
            }
            normDistVec.push_back(normDist);
        }
    }
    return normDistVec;
}
std::vector<std::vector<double> > DistanceComparer::Normalize(const std::vector<std::vector<double> >& distVecs, double thresh, int noramlizeMode)
{
    vector<vector<double> > normDistVecs;
    if(m_distMode == 1)
    {
        for(int i=0; i<distVecs.size(); ++i)
        {
            vector<double> tmpNormDistVec;
            for(int j=0; j<distVecs[i].size(); ++j)
            {
                double normDist = INVALID_VALUE;
                if(noramlizeMode == 1)
                    normDist = distVecs[i][j]/thresh;
                else if(noramlizeMode == 2)
                    normDist = std::sqrt(distVecs[i][j]*distVecs[i][j]/(2*thresh*thresh));
                tmpNormDistVec.push_back(normDist);
            }
            normDistVecs.push_back(tmpNormDistVec);
        }
    }
    return normDistVecs;
}


//private
cv::Point DistanceComparer::computeCenter(cv::Rect rect)
{
    int cen_x = rect.x + rect.width/2;
    int cen_y = rect.y + rect.height/2;
    return Point(cen_x, cen_y);
}