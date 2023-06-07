#pragma once

// tensorRT include
// 编译用的头文件
#include <NvInfer.h>

// onnx解析器的头文件
#include <onnx-tensorrt-release-8.0/NvOnnxParser.h>

// 推理用的运行时头文件
#include <NvInferRuntime.h>

// cuda include
#include <cuda_runtime.h>

// system include
#include <stdio.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <preprocess.hpp>

using namespace std;

struct PairThree{
    nvinfer1::IRuntime* runtime = nullptr;
    nvinfer1::ICudaEngine* engine = nullptr;
    nvinfer1::IExecutionContext* model = nullptr;
    PairThree(){}
    PairThree(nvinfer1::IRuntime* r, 
              nvinfer1::ICudaEngine* e, 
              nvinfer1::IExecutionContext* m):runtime(r), engine(e), model(m){}
    ~PairThree(){
        model->destroy();
        engine->destroy();
        runtime->destroy();
    }
};

class TRTLogger : public nvinfer1::ILogger{
public:
    virtual void log(Severity severity, nvinfer1::AsciiChar const* msg) noexcept override;
};

inline const char* severity_string(nvinfer1::ILogger::Severity t);

vector<unsigned char> load_file(const string& file);

bool build_model(const char* model_name);

PairThree* load_model(const string& path);

float* inference_info(nvinfer1::IExecutionContext* execution_context, float* input_data_pin, size_t input_data_size, size_t output_data_size);

float* inference(nvinfer1::IExecutionContext* execution_context, float* input_data_pin, size_t input_data_size, size_t output_data_size);