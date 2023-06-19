#include "core.hpp"

void test(){
    cout<<setfill('-')<<setiosflags(ios::right);
    cout << endl;
    printInfo(14.318126, 10, "前处理：高斯模糊时长", 0);
    printInfo(9.318624, 5, "前处理: resize时长", 0);
    printInfo(24.512364, 6, "前处理总时长", 0);
    cout << endl;
    printInfo(8.703112, 6, "后处理：mask时长", 0);
    printInfo(16.156384, 10, "后处理：直线拟合时长", 0);
    printInfo(26.135687, 6, "后处理总时长", 0);

    cout << endl << endl;
    printInfo(6.187617, 10, "前处理：高斯模糊时长", 0);
    printInfo(7.638154, 5, "前处理: resize时长", 0);
    printInfo(14.115134, 6, "前处理总时长", 0);
    cout << endl;
    printInfo(6.318724, 6, "后处理：mask时长", 0);
    printInfo(6.716491, 10, "后处理：直线拟合时长", 0);
    printInfo(13.721534, 6, "后处理总时长", 0);

}

int main(){
    // test();
    // build_or_inferOnePicture_FT32("attn_unet_fresh2.onnx", "engine_ft32.trtmodel", 1);
    // build_or_inferOnePicture_INT8("attn_unet_fresh2.onnx", "engine_int8.trtmodel", 1);

    // performance_test("engine_ft32.trtmodel", "valid_dataset.txt", "valid_label.txt");
    // performance_test("engine_int8.trtmodel", "valid_dataset.txt", "valid_label.txt");

    string path = "engine_int8.trtmodel";      // 推理模型源文件
    Size resize_scale =  Size(256, 256);  // resize大小
    size_t input_size = 1*3*256*256;      // 推理模型的输入大小
    size_t output_size = 1*4*256*256;     // 推理模型的输出大小

    string port = "/dev/pts/2";
    BaudRate rate = BaudRate::B_9600;

    Camera cam(0, 1.2, 1, deg2rad(-80));  // 相机位置dx, dy, dz, camera_degree

    float v_des = 0.2;                    // 车轮转速
    float L = 1.2;                        // 车身长度
    float B = 0.4;                        // 轮间半轴长度

    runRobot(path, resize_scale, input_size, output_size, port, rate, cam, v_des, L, B); // Firing ! !

    return 0;
}