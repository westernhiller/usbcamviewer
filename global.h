#ifndef GLOBAL_H
#define GLOBAL_H

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <QDebug>
#include <QImage>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/videoio/videoio.hpp"

#define SCREENWIDTH     1280
#define SCREENHEIGHT    800
#define IMAGEWIDTH      1920
#define IMAGEHEIGHT     1080

#define SHMEMKEY        "USBCAM"

typedef enum
{
    WIDGET_INVALID,
    WIDGET_MAIN,
    WIDGET_REPLAY,
    WIDGET_BROWSER,
    WIDGET_SETTINGS,
} WIDGET_ID;

#define time_diff(ts, te)     ((te - ts)/getTickFrequency())

void msleep(unsigned int msec);

#endif // GLOBAL_H
