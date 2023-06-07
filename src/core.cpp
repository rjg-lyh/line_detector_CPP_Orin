#include "core.hpp"

int build_or_inferOnePicture(const char* onnx_name, const string& path, int state){
    if(state == 0){
        if(!build_model(onnx_name)){
            return -1;
        }
        return 0;
    }

    TimerClock clock;
    cout<<setfill('-')<<setiosflags(ios::right);

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
    float* pdst_pin = warpaffine_and_normalize_best_info(src, resize_scale);
    printInfo(clock.getTimeMilliSec(), 6, "前处理总时长", 0);
    
    clock.update();
    PairThree* params = load_model(path);
    nvinfer1::IExecutionContext* model = params->model;
    printInfo(clock.getTimeMilliSec(), 7, "模型加载总时长", 0);

    clock.update();
    float* output_data_pin = inference_info(model, pdst_pin, input_size, output_size);
    printInfo(clock.getTimeMilliSec(), 5, "推理总时长", 0);
    // save_data_CHW<float*>(output_data_pin, "output_data_host.txt", 256*256); //没打印全

    clock.update();
    drawLine(src, output_data_pin);
    printInfo(clock.getTimeMilliSec(), 6, "后处理总时长", 0);

    cv::namedWindow("out", 0);
    cv::resizeWindow("out", cv::Size(1422, 800));
    cv::imshow("out", src);
	cv::waitKey(0);
    cv::destroyAllWindows();

    cudaFreeHost(output_data_pin);
    delete params;

    return 0;
}

int runCamera(nvinfer1::IExecutionContext *model, cv::Size resize_scale, size_t input_data_size, size_t output_data_size){
    VideoCapture capture(0);
    // string pipeline = "v4l2src device=/dev/video4 ! video/x-raw,format=UYVY,width=1920,height=1080, \
    // framerate=30/1! videoconvert ! appsink video-sink=xvimagesink sync=false";

    // capture.open(pipeline, cv::CAP_GSTREAMER);
    if(capture.isOpened()){
        cout << "成功开启摄像头" << endl;
    }
    else{
        cout << "打开摄像头失败..." << endl;
        return -1;
    }

    Mat frame;
	namedWindow("camera-frame", 0);
    resizeWindow("camera-frame", 1422, 800); 
    while (capture.read(frame)){
        //去畸变
        float* pdst_pin = warpaffine_and_normalize_best(frame, resize_scale); //预处理
        float* output_data_pin = inference(model, pdst_pin, input_data_size, output_data_size); //模型预测结果
        auto pair_dots = drawLine(frame, output_data_pin); //后处理, 目前是在frame上画出导航线, 并返回两端点值
        cudaFreeHost(output_data_pin);
        //********************
        // control code
        //********************
        imshow("camera-frame", frame);
        printf("dot1: (%.1f, %.1f)  dot2: (%.1f, %.1f)\n\n",
               pair_dots.first.x, pair_dots.first.y, 
               pair_dots.second.x, pair_dots.second.y);
        char c = waitKey(1);
		if (c == 27) {
			break;
		}
    }
    cv::destroyAllWindows();
    capture.release();
    return 0;
}

int runRobot(const string& path, const cv::Size &resize_scale, const size_t &input_size, const size_t &output_size){

    PairThree* params = load_model(path);
    nvinfer1::IExecutionContext* model = params->model;
    runCamera(model, resize_scale, input_size, output_size);

    delete params;
    return 0;
}