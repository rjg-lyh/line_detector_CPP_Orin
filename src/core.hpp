#pragma once

#include "build_infer.hpp"
#include "preprocess.hpp"
#include "postprocess.hpp"
#include "tools.hpp"

int build_or_inferOnePicture(const char* onnx_name, const string& path, int state);

int runCamera(nvinfer1::IExecutionContext *model, cv::Size resize_scale, size_t input_data_size, size_t output_data_size);

int runRobot(const string& path, const cv::Size &resize_scale, const size_t &input_size, const size_t &output_size);