#ifndef SELECTROIBYMOUSE_H
#define SELECTROIBYMOUSE_H

#include <opencv2/core/core.hpp>
#include <vector>

void mouseHandler(int event, int x, int y, int flags, void *param);
cv::Rect selectRectByMouse(const cv::Mat& img, double scale);
void selectPolygonByMouse(const cv::Mat& img, double scale, std::vector<cv::Point>& polygon, cv::Rect& rect);

void drawPolygon(cv::Mat& img, std::vector<cv::Point> polygon);
void drawLine(cv::Mat& img, std::vector<cv::Point> polygon);

void getPolyRoi(cv::Mat src, cv::Mat& dst, std::vector<cv::Point> polygon);

//
void selectPolylineByMouse(const cv::Mat& img, double scale, std::vector<cv::Point>& polyline);
void mouseHandler_polyline(int event, int x, int y, int flags, void *param);
void drawPolyline(cv::Mat& img, std::vector<cv::Point> polyline);

#endif //SELECTROIBYMOUSE_H