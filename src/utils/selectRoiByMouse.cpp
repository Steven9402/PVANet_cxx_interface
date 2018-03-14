#include "selectRoiByMouse.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

std::vector<cv::Point> draw_polygon, final_polygon;
bool drawing_poly = false, gotPoly = false;

void drawPolygon(cv::Mat& img, std::vector<cv::Point> polygon)
{
    polylines(img, polygon, true, Scalar(0, 255, 0), 2);
    //多边形填充，制作mask
    Point *points[1];
    points[0] = new Point[polygon.size()];
    for (int i = 0; i < polygon.size(); ++i)
    {
        points[0][i] = polygon.at(i);
    }
    const Point* ppt[1] = { points[0] };
    const int npt[] = { polygon.size() };
}
void drawLine(Mat& img, vector<Point> polygon)
{
    line(img, polygon[0], polygon[1], Scalar(0, 255, 0), 2);
}

void mouseHandler(int event, int x, int y, int flags, void *param)
{
    switch (event){
        case CV_EVENT_MOUSEMOVE:
            if (drawing_poly){
                draw_polygon = final_polygon;
                draw_polygon.push_back(Point(x, y));
            }
            break;
        case CV_EVENT_LBUTTONDOWN:
            drawing_poly = true;
            final_polygon.push_back(Point(x, y));
            if (final_polygon.size() >= 6)
                gotPoly = true;
            break;
    }
}
cv::Rect selectRectByMouse(const cv::Mat& img, double scale)
{
    final_polygon.clear();
    draw_polygon.clear();
    vector<pair<int, int> >roi;
    vector<cv::Point> polygon;
    namedWindow("img", CV_WINDOW_AUTOSIZE);
    setMouseCallback("img", mouseHandler, NULL);
    Mat _img = img.clone();
    while (!gotPoly)
    {
        Mat showImg;
        cv::resize(_img, showImg, cv::Size(int(_img.cols / scale), int(_img.rows / scale)));
        if (draw_polygon.size() > 2)
            drawPolygon(showImg, draw_polygon);
        else if (draw_polygon.size() == 2)
            drawLine(showImg, draw_polygon);
        imshow("img", showImg);
        waitKey(40);
    }
    setMouseCallback("img", NULL, NULL);
    cv::destroyWindow("img");
    for (int i = 0; i < final_polygon.size(); ++i)
    {
        roi.push_back(pair<int,int>(int(final_polygon[i].x * scale),int(final_polygon[i].y * scale)));
    }
    drawing_poly = false;
    gotPoly = false;
    return boundingRect(final_polygon);
}

void selectPolygonByMouse(const cv::Mat& img, double scale, std::vector<cv::Point>& polygon, cv::Rect& rect)
{
    final_polygon.clear();
    draw_polygon.clear();
    vector<pair<int, int> >roi;
    namedWindow("img", CV_WINDOW_AUTOSIZE);
    setMouseCallback("img", mouseHandler, NULL);
    Mat _img = img.clone();
    while (!gotPoly)
    {
        Mat showImg;
        cv::resize(_img, showImg, cv::Size(int(_img.cols / scale), int(_img.rows / scale)));
        if (draw_polygon.size() > 2)
            drawPolygon(showImg, draw_polygon);
        else if (draw_polygon.size() == 2)
            drawLine(showImg, draw_polygon);
        imshow("img", showImg);
        waitKey(40);
    }
    setMouseCallback("img", NULL, NULL);
    cv::destroyWindow("img");
    for (int i = 0; i < final_polygon.size(); ++i)
    {
        roi.push_back(pair<int,int>(int(final_polygon[i].x * scale),int(final_polygon[i].y * scale)));
    }
    drawing_poly = false;
    gotPoly = false;
    polygon.insert(polygon.begin(), final_polygon.begin(), final_polygon.end());
    rect = boundingRect(final_polygon);
    return ;
}

//获取多边形兴趣区
void getPolyRoi(Mat src, Mat& dst, vector<Point> polygon)
{
    Mat mask = Mat::zeros(src.size(), CV_8UC1);
    //多边形填充，制作mask
    Point *points[1];
    points[0] = new Point[polygon.size()];
    for (int i = 0; i < polygon.size(); ++i)
    {
        points[0][i] = polygon.at(i);
    }
    const Point* ppt[1] = { points[0] };
    const int npt[] = { polygon.size() };
    fillPoly(mask, ppt, npt, 1, Scalar(255));
    //二值化
    threshold(mask, mask, 128, 255, THRESH_BINARY);
    //根据mask绘制
    src.copyTo(dst, mask);
    delete[] points[0]; //删除动态分配内存的点
}


void mouseHandler_polyline(int event, int x, int y, int flags, void *param)
{
    switch (event){
        case CV_EVENT_MOUSEMOVE:
            if (drawing_poly){
                draw_polygon = final_polygon;
                draw_polygon.push_back(Point(x, y));
            }
            break;
        case CV_EVENT_LBUTTONDOWN:
            drawing_poly = true;
            final_polygon.push_back(Point(x, y));
            if (final_polygon.size() >= 3)
                gotPoly = true;
            break;
    }
}

void selectPolylineByMouse(const cv::Mat& img, double scale, std::vector<cv::Point>& polyline)
{
    final_polygon.clear();
    draw_polygon.clear();
    vector<pair<int, int> >roi;
    namedWindow("img", CV_WINDOW_AUTOSIZE);
    setMouseCallback("img", mouseHandler_polyline, NULL);
    Mat _img = img.clone();
    while (!gotPoly)
    {
        Mat showImg;
        cv::resize(_img, showImg, cv::Size(int(_img.cols / scale), int(_img.rows / scale)));
        if (draw_polygon.size() >= 2)
            drawPolyline(showImg, draw_polygon);
        imshow("img", showImg);
        waitKey(40);
    }
    setMouseCallback("img", NULL, NULL);
    cv::destroyWindow("img");
    for (int i = 0; i < final_polygon.size(); ++i)
    {
        roi.push_back(pair<int,int>(int(final_polygon[i].x * scale),int(final_polygon[i].y * scale)));
    }
    drawing_poly = false;
    gotPoly = false;
    polyline.insert(polyline.begin(), final_polygon.begin(), final_polygon.end());
    return ;
}

void drawPolyline(cv::Mat& img, std::vector<cv::Point> polyline)
{
    for(int i=0; i<polyline.size()-1; ++i)
    {
        line(img, polyline[i], polyline[i+1], Scalar(0, 255, 0), 2);
    }
}