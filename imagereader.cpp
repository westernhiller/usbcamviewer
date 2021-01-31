#include "imagereader.h"
#include "global.h"
#include "processthread.h"

ImageReader::ImageReader(QString imgfile, QObject* parent)
    : QThread(parent)
    , m_imgfile(imgfile)
{
}

ImageReader::~ImageReader()
{
    requestInterruption();
    quit();
    wait();
}

bool ImageReader::isFileVideo(QString filename)
{
    QString ext = filename.right(4);
    if(ext.compare(".avi") == 0)
        return true;

    if(ext.compare(".mp4") == 0)
        return true;

    return false;
}

void ImageReader::run()
{
    if(isFileVideo(m_imgfile))
    {
        cv::VideoCapture vc(m_imgfile.toStdString());
        if(vc.isOpened())
        {
            cv::Mat frame;
            vc >> frame;
            if(frame.data)
            {
                int x1 = frame.cols * 1 / 3;
                int x2 = frame.cols * 2 / 3;
                int y1 = frame.rows / 3;
                int y2 = frame.rows * 2 / 3;

                cv::Point pts[4] = {cv::Point(x1, y1), cv::Point(x1, y2), cv::Point(x2, frame.rows / 2), cv::Point(x1, y1)};
                cv::fillConvexPoly(frame, pts, 4, cv::Scalar(0, 255, 0));

                emit gotImage(m_imgfile, mat2qimage(frame));
            }
        }
    }
    else {
        QImage image(m_imgfile);
        if(!image.isNull())
            emit gotImage(m_imgfile, image.copy());
    }
    emit killMe();
}

