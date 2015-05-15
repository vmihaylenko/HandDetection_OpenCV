#include "CameraCapturer.h"
#include "HandDetector.h"

void custom_main();

int main() {
    custom_main();
    return 0;
}

void custom_main() {
    //hd_cv::CameraCapturer cc = hd_cv::CameraCapturer();
    //cc.start_capturing_with_mog();
    cv::Mat src = cv::imread("/home/mikhail/1.bmp", CV_LOAD_IMAGE_GRAYSCALE);
    hd_cv::HandDetector::detect_hand_on_image(src);
}

