#include "core.hpp"

int main(){
    // build_or_inferOnePicture("attn_unet_fresh2.onnx", "engine.trtmodel", 1);
    
    string path = "engine.trtmodel";
    Size resize_scale =  Size(256, 256);
    size_t input_size = 1*3*256*256;
    size_t output_size = 1*4*256*256;

    runRobot(path, resize_scale, input_size, output_size);

    return 0;
}