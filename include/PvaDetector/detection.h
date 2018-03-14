#ifndef DETECTION_H
#define DETECTION_H

#include <string>
#include <opencv2/core/core.hpp>

class Detection
{
public:
    Detection();
    Detection(const std::string& cls, const cv::Rect& rect, const float& score);
    Detection(Detection const& detection);
    Detection& operator=(const Detection &detection);

    ~Detection();

    //Interface
    std::string getClass() const;
    cv::Rect getRect() const;
    float getScore() const;

    void setClass(const std::string& cls);
    void setRect(const cv::Rect& rect);
    void setScore(const float& score);

private:
    std::string m_cls;
    cv::Rect m_rect;
    float m_score;

};

#endif // DETECTION_H
