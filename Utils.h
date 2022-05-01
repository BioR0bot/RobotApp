#pragma once

#include <memory>
#include <QImage>
#include <QPixmap>
#include <opencv2/core.hpp>

QImage CvMatToQImage(const cv::Mat &inMat);

QPixmap CvMatToQPixmap(const cv::Mat& inMat);

template <class T> std::weak_ptr<T> make_weak_ptr(std::shared_ptr<T> ptr)
{
    return ptr;
}

