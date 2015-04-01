#include "CameraCapturer.h"
#include "HandDetector.h"

void custom_main();

int main() {
    custom_main();
    return 0;
}

void custom_main() {
    hd_cv::CameraCapturer cc = hd_cv::CameraCapturer();
    cc.start_capturing_with_mog();
    cv::Mat src = cc.output();
    hd_cv::HandDetector::detect_hand_on_image(src);
}

