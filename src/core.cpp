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
    OutInfo* outinfo = postprocess(src, output_data_pin);
    printInfo(clock.getTimeMilliSec(), 6, "后处理总时长", 0);

    cv::namedWindow("out", 0);
    cv::resizeWindow("out", cv::Size(1422, 800));
    cv::imshow("out", src);
	cv::waitKey(0);
    cv::destroyAllWindows();

    cudaFreeHost(output_data_pin);
    delete outinfo;
    delete params;

    return 0;
}

int runCamera(nvinfer1::IExecutionContext *model, SerialPort* serialPort,
              cv::Size resize_scale, size_t input_data_size, size_t output_data_size,
              Camera& cam, float v_des, float L, float B){
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
    double frame_W = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    double frame_H = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    cout << "w: " << frame_W << endl;
    cout << "h: " << frame_H << endl;

	namedWindow("camera-frame", 0);
    resizeWindow("camera-frame", 1422, 800); 

    Mat frame_naive, frame;
    while (capture.read(frame_naive)){
        // undistort(frame_naive, frame, mat_intri, coff_dis, noArray());//去畸变
        frame = frame_naive;
        float* pdst_pin = warpaffine_and_normalize_best(frame, resize_scale); //预处理
        float* output_data_pin = inference(model, pdst_pin, input_data_size, output_data_size); //模型预测结果
        OutInfo* outinfo = postprocess(frame, output_data_pin); //后处理
        
        float wheelAngle = control_unit(cam, L, B, frame_H, v_des, outinfo->ex, outinfo->e_angle);    // control, wheelAngle区间范围[-90, 90]
        // float wheelAngle = control_unit(cam, L, B, 512, v_des, 10, -30);

        string signal = angle2signal(serialPort, wheelAngle);      //车轮角度转化为对应的Hex信号
        serialPort->Write(signal);
        // serialPort->Write(string_to_hex(signal));    //串口输出控制信号，下位机转动车轮

        imshow("camera-frame", frame);
        cout << "wheelAngle: " << wheelAngle << endl;

        cudaFreeHost(output_data_pin);
        delete outinfo;
        char c = waitKey(1);
		if (c == 27) {
			break;
		}
    }
    
    cv::destroyAllWindows();
    capture.release();
    return 0;
}

int runRobot(const string& path, const cv::Size &resize_scale, const size_t &input_size, const size_t &output_size,
            Camera& cam, float v_des, float L, float B){

    PairThree* params = load_model(path);
    nvinfer1::IExecutionContext* model = params->model;

    SerialPort* serialPort = new SerialPort("/dev/pts/5", BaudRate::B_9600);
    serialPort->SetTimeout(0); //无阻塞， -1：阻塞接受
    serialPort->Open(); //打开串口
    
    serialPort->Write("FF 01 01 30 80 00 00 00 FF\n");
    // serialPort->Write(string_to_hex("FF 01 01 30 80 00 00 00 FF")); // 启动农机，速度30，正方向 

    runCamera(model, serialPort, resize_scale, input_size, output_size, cam, v_des, L, B); //运行相机，开始无人导航

    serialPort->Write("FF 00 00 00 80 00 00 00 FF\n");
    // serialPort->Write(string_to_hex("FF 00 00 00 80 00 00 00 FF")); // 刹车复位，车轮摆正

    serialPort->Close();

    delete serialPort;
    delete params;
    return 0;
}