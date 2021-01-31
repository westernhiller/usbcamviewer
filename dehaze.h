#ifndef DEHAZE_H
#define DEHAZE_H

#include "global.h"

class Dehaze : public QObject
{
    Q_OBJECT
public:
    explicit Dehaze(int r = 7, double t0 = 0.1, double omega = 0.95, double eps = 10E-6);
    ~Dehaze() override;

    cv::Mat removeHaze(const cv::Mat& I);

signals:

private:
    //common
    int r;
    double t0;
    double omega;
    double eps;

    cv::Mat mGammaLookUpTable;
    cv::Mat I;
    cv::Mat I_YUV;
    cv::Vec3f atmosphericLight = cv::Vec3f(255,255,255);
    cv::Mat transmission;

//    cv::Vec3f estimateAtmosphericLight();
    cv::Mat estimateTransmission();
    cv::Mat recover();

    void initGammaLookUpTable(double gamma);

public slots:
    void updateAtmosphericLight(float, float, float);
};

#endif

