//
// Created by dongxia on 17-10-26.
//

#include "darkchannel.h"

using namespace cv;
using namespace std;


cv::Mat calcMinChannel(const cv::Mat& src)
{
    CV_Assert(!src.empty());

    vector<Mat> channels;
    split(src,channels);

    Mat minChannel = channels[0];
    for(size_t i = 1;i<channels.size();i++){
        minChannel = min(minChannel,channels[i]);
    }
    return minChannel;
}

cv::Mat calcDarkChannel(const cv::Mat& src, int r)
{
    Mat minChannel = calcMinChannel(src);

    Mat kernel = getStructuringElement(MORPH_RECT,Size(2*r+1,2*r+1));

    Mat darkChannel;
    erode(minChannel,darkChannel,kernel);

    return darkChannel;
}

cv::Vec3f estimateAtmosphericLight(const Mat& source, int r)
{
//    double start = clock();

    Mat minChannel = calcMinChannel(source);

    double maxValue = 0;
    Mat aimRoi;

    for (int i = 0; i < minChannel.rows; i+=r) {
        for (int j = 0; j < minChannel.cols; j+=r) {
            int w = (j+r < minChannel.cols) ? r : minChannel.cols-j;
            int h = (i+r < minChannel.rows) ? r : minChannel.rows-i;
            Mat roi(minChannel,Rect(j,i,w,h));
            cv::Scalar mean, std, score;
            meanStdDev(roi,mean,std);
            score = mean -std;
            if (score.val[0] > maxValue){
                maxValue = score.val[0];
                aimRoi = Mat(source,Rect(j,i,w,h));
            }
        }
    }

    Scalar mean,std;
    meanStdDev(aimRoi,mean,std);

    cv::Vec3f atmosphericLight;

    atmosphericLight[0] = static_cast<float> (mean.val[0]);
    atmosphericLight[1] = static_cast<float> (mean.val[1]);
    atmosphericLight[2] = static_cast<float> (mean.val[2]);

//    double stop = clock();

//    cout << "atmosphericLight" << atmosphericLight << endl;
//    qDebug() <<"估计大气光耗时："<<(stop - start)/CLOCKS_PER_SEC*1000<<"ms"<<endl;

    return atmosphericLight;
}
