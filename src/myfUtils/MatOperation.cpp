//
// Created by cuizhou on 18-2-24.
//

#include "MatOperation.h"

Mat composeGrayVisMat(vector<Mat> matpool){
    int cols=20;
    int rows=matpool.size()/20+1;

    int size=75;

    Mat vismat=Mat::zeros(size*rows,size*cols,CV_8UC1);
    for(int ind=0;ind<matpool.size();ind++){
        int row=ind/cols;
        int col=ind%cols;
        int x=col*size;
        int y=row*size;
        Rect pasteroi(x,y,size-5,size-5);

        Mat pastemat;
        resize(matpool[ind],pastemat,Size(size-5,size-5),0,0);
        pastemat.copyTo(vismat(pasteroi));
    }

    return vismat;
}