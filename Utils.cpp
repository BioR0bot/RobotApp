#include "Utils.h"
#include <QDebug>

QImage CvMatToQImage(const cv::Mat &inMat)
{
    switch (inMat.type())
    {
    case CV_8UC4: // 8-bit, 4 channel
    {
        QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGBA8888);

        return image;
    }
    case CV_8UC3: // 8-bit, 3 channel
    {
        QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888);

        return image;
    }
    case CV_8UC1: // 8-bit, 1 channel
    {
        static QVector<QRgb>  sColorTable;

        // only create our color table once
        if (sColorTable.isEmpty())
        {
            for (int i = 0; i < 256; ++i)
                sColorTable.push_back(qRgb(i, i, i));
        }

        QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8);

        image.setColorTable(sColorTable);

        return image;
    }

    default:
        qDebug() << "cv::Mat image type not handled in switch:" << inMat.type();
        break;
    }

    return QImage();
}

QPixmap CvMatToQPixmap(const cv::Mat& inMat)
{
    return QPixmap::fromImage(CvMatToQImage(inMat));
}
