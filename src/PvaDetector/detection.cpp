/**
* @Copyright (c) 2016 by Jp Hu
* @Date 2017-06-27
* @Institude College of Surveying & Geo-Informatics, Tongji University
* @Function
*/

#include "detection.h"

using namespace cv;
using namespace std;
// constructor
Detection::Detection()
{
}
Detection::Detection(const std::string& cls, const cv::Rect& rect, const float& score)
    :m_cls(cls), m_rect(rect), m_score(score)
{

}

// copy constructor
Detection::Detection(Detection const& detection)
{
    m_cls = detection.m_cls;
    m_rect = detection.m_rect;
    m_score = detection.m_score;
}
// set value constructor
Detection& Detection::operator=(const Detection &detection)
{
    m_cls = detection.m_cls;
    m_rect = detection.m_rect;
    m_score = detection.m_score;
    return *this;
}

Detection::~Detection()
{

}
//Interface
string Detection::getClass() const
{
    return m_cls;
}

Rect Detection::getRect() const
{
    return m_rect;
}

float Detection::getScore() const
{
    return m_score;
}

void Detection::setClass(const std::string& cls)
{
    m_cls = cls;
}

void Detection::setRect(const cv::Rect& rect)
{
    m_rect = rect;
}

void Detection::setScore(const float& score)
{
    m_score = score;
}
