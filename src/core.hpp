#pragma once

#include "build_infer.hpp"
#include "preprocess.hpp"
#include "postprocess.hpp"
#include "control.hpp"
#include "tools.hpp"

const cv::Mat mat_intri = ( cv::Mat_<float> ( 3,3 ) << 1.36348255e+03, 0.00000000e+00, 9.47132866e+02,
                                                         0.00000000e+00 , 1.38236930e+03 , 5.24419545e+02,
                                                         0.00000000e+00 , 0.00000000e+00 , 1.00000000e+00);
const cv::Mat coff_dis = ( cv::Mat_<float> ( 5,1 ) << -0.47869613 ,  0.4691494 ,  -0.00133405 ,  0.00117572 , -0.49861739);

int build_or_inferOnePicture(const char* onnx_name, const string& path, int state);

int runCamera(nvinfer1::IExecutionContext *model, cv::Size resize_scale, size_t input_data_size, size_t output_data_size,
              Camera& cam, float v_des, float L, float B);

int runRobot(const string& path, const cv::Size &resize_scale, const size_t &input_size, const size_t &output_size,
            Camera& cam, float v_des, float L, float B);