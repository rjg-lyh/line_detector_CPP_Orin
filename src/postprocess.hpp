#pragma once

#include "preprocess.hpp"
#include <vector>

using namespace cv;
using namespace std;

float sigmoid(float x);

pair<Point2i, Point2i> computeEndDots(Mat& image, cv::Scalar dot_color, vector<Point2i> v);

Mat drawLine(const Mat& src, float* pdata);