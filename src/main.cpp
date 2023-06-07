#include "build_infer.hpp"
#include "preprocess.hpp"
#include "postprocess.hpp"
#include "tools.hpp"

int main(){
    // char model_name[] = "attn_unet_fresh2.onnx";
    // build_model(model_name);
    // build_model("attn_unet_fresh2.onnx");
    TimerClock clock;
    cout<<setfill('-')<<setiosflags(ios::right);

    string path = "engine.trtmodel";
    Size resize_scale =  Size(256, 256);
    size_t input_size = 1*3*256*256;
    size_t output_size = 1*4*256*256;

    Mat src = imread("ceshi.jpg");

    // cuda预热 setfill('&')
    clock.update();
    warm_up_cuda(src, resize_scale);
    printInfo(clock.getTimeMilliSec(), 5, "cuda预热总时长", 0);
    // cout << PURPLE << left << setw(70)<< "|---------总预热时长: " + to_string(clock.getTimeMilliSec()) + "ms" << "|" << NONE << endl << endl;

    clock.update();
    float* pdst_pin = warpaffine_and_normalize_best(src, resize_scale);
    printInfo(clock.getTimeMilliSec(), 6, "前处理总时长", 0);
    
    clock.update();
    PairThree* params = load_model(path);
    nvinfer1::IExecutionContext* model = params->model;
    printInfo(clock.getTimeMilliSec(), 7, "模型加载总时长", 0);

    clock.update();
    float* output_data_pin = inference(model, pdst_pin, 1*3*256*256, 1*4*256*256);
    printInfo(clock.getTimeMilliSec(), 5, "推理总时长", 0);
    // save_data_CHW<float*>(output_data_pin, "output_data_host.txt", 256*256); //没打印全

    clock.update();
    Mat out = drawLine(src, output_data_pin);
    printInfo(clock.getTimeMilliSec(), 6, "后处理总时长", 0);

    cv::namedWindow("out", 0);
    cv::resizeWindow("out", cv::Size(1422, 800));
    cv::imshow("out", out);
	cv::waitKey(0);
    cv::destroyAllWindows();

    cudaFreeHost(output_data_pin);
    delete params;

    return 0;
}