#include "dehaze.h"
#include <QTimer>
#include <QDebug>

#include "darkchannel.h"
#include "fastguidedfilter.h"

using namespace std;
using namespace cv;

Dehaze::Dehaze(int r, double t0, double omega, double eps)
    : r(r)
    , t0(t0)
    , omega(omega)
    , eps(eps)
{
    initGammaLookUpTable(0.7);
}

Dehaze::~Dehaze()
{

}

Mat Dehaze::removeHaze(const Mat& I)
{
    CV_Assert(I.channels() == 3);
    if (I.depth() != CV_32F){
        I.convertTo(this->I, CV_32F);
    }

    cvtColor(this->I,this->I_YUV,COLOR_BGR2YUV);

//    estimateAtmosphericLight(I, r);

    estimateTransmission();
    return recover();
}

void Dehaze::updateAtmosphericLight(float fr, float fg, float fb)
{
    atmosphericLight[0] = fr;
    atmosphericLight[1] = fg;
    atmosphericLight[1] = fb;
}


void Dehaze::initGammaLookUpTable(double gamma)
{
    mGammaLookUpTable = Mat(1, 256, CV_8U);
    uchar* p = mGammaLookUpTable.ptr();
    for( int i = 0; i < 256; ++i)
    {
        p[i] = saturate_cast<uchar>(pow((i/255.0), gamma) * 255.0);
    }
}

cv::Mat Dehaze::estimateTransmission(){

//    double start = clock();

    CV_Assert(I.channels() == 3);
    vector<Mat> channels;
    split(I_YUV,channels);

    channels[0] = channels[0]/(atmosphericLight[0]*0.114 + atmosphericLight[1]*0.587 + atmosphericLight[2]*0.299);
    Mat darkChannel = calcDarkChannel(channels[0],r);

    transmission = 1 - omega * darkChannel;

    //导向滤波耗时30ms左右
//    transmission = guidedFilter(src, transmission, 8*r, eps);

    //透射率修正
    float k = 0.3;
    transmission = min(max(k/abs(1-darkChannel),1).mul(transmission),1);

    //10ms左右
    transmission = fastGuidedFilter(channels[0], transmission, 8*r, 4, eps);

//    double stop = clock();

//    qDebug() <<"估计透射率耗时："<<(stop - start)/CLOCKS_PER_SEC*1000<<"ms"<<endl;

    return transmission;
}

cv::Mat Dehaze::recover(){

//    double start = clock();

    CV_Assert(I.channels() == 3 && I.depth() == CV_32F);
    vector<Mat> channels;
    split(I,channels);

    Mat t = transmission;

    channels[0] = (channels[0]-atmosphericLight[0])/t + atmosphericLight[0];
    channels[1] = (channels[1]-atmosphericLight[1])/t + atmosphericLight[1];
    channels[2] = (channels[2]-atmosphericLight[2])/t + atmosphericLight[2];

    Mat recover;
    merge(channels,recover);

    recover.convertTo(recover,CV_8U);

    //
    LUT(recover,mGammaLookUpTable,recover);

//    double stop = clock();

//    qDebug() <<"恢复图像耗时："<<(stop - start)/CLOCKS_PER_SEC*1000<<"ms"<<endl;

    return recover;
}
