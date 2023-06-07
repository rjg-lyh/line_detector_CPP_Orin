#pragma once

#include "preprocess.hpp"
#include <vector>

using namespace cv;
using namespace std;

float sigmoid(float x);

Point2i invertDot(const Point2i& point, int w, int h);

pair<Point2i, Point2i> computeEndDots(Mat& image, cv::Scalar dot_color, vector<Point2i> v);

pair<cv::Point2i, cv::Point2i> drawLine(const Mat& src, float* pdata);