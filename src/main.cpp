#include "core.hpp"

int main(){
    // test();
    // build_or_inferOnePicture_FT32("attn_unet_sas_fresh.onnx", "engine_ft32_sas.trtmodel", 1);
    // build_or_inferOnePicture_INT8("attn_unet_fake_fresh.onnx", "engine_int8_fake.trtmodel", 0);

    // performance_test("engine_ft32_sas.trtmodel", "valid_dataset.txt", "valid_label.txt");
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