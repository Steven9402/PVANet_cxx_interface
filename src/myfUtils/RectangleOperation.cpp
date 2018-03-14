//
// Created by cuizhou on 18-2-25.
//

#include "RectangleOperation.h"

RectangleOperation::RectangleOperation(int i){

}
double RectangleOperation::calcIouRatio(const Rect& rect1,const Rect& rect2){

    //''' Rect = [x1, y1, x2, y2] '''
    int x1 = rect1.x;
    int y1 = rect1.y;
    int width1 = rect1.width;
    int height1 = rect1.height;

    int x2 = rect2.x;
    int y2 = rect2.y;
    int width2 = rect2.width;
    int height2 = rect2.height;

    int endx = max(x1+width1,x2+width2);
    int startx = min(x1,x2);
    int width = width1+width2-(endx-startx);

    int endy = max(y1+height1,y2+height2);
    int starty = min(y1,y2);
    int height = height1+height2-(endy-starty);

    double ratio=0;
    if(width <=0 || height <= 0) {
        ratio = 0;
    }else{
        double area = width*height;
        double area1 = width1*height1;
        double area2 = width2*height2;
        ratio = area*1.0/(area1+area2-area);
    }
    return ratio;
}
double RectangleOperation::calcOverlapRatio(const Rect& rect1,const Rect& rect2){

    //''' Rect = [x1, y1, x2, y2] '''
    int x1 = rect1.x;
    int y1 = rect1.y;
    int width1 = rect1.width;
    int height1 = rect1.height;

    int x2 = rect2.x;
    int y2 = rect2.y;
    int width2 = rect2.width;
    int height2 = rect2.height;

    int endx = max(x1+width1,x2+width2);
    int startx = min(x1,x2);
    int width = width1+width2-(endx-startx);

    int endy = max(y1+height1,y2+height2);
    int starty = min(y1,y2);
    int height = height1+height2-(endy-starty);

    double ratio=0;
    if(width <=0 || height <= 0) {
        ratio = 0;
    }else{
        double area = width*height;
        double area1 = width1*height1;
        double area2 = width2*height2;
        ratio = (double)area/min(area1,area2);
    }
    return ratio;
}