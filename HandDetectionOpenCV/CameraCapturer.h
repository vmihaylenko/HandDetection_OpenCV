//
//  CameraCapturer.h
//  HandDetectionOpenCV
//
//  Created by Vlad Mihaylenko on 24/03/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#ifndef __HandDetectionOpenCV__CameraCapturer__
#define __HandDetectionOpenCV__CameraCapturer__

#include <opencv2/opencv.hpp>
#include "ProcessHelper.h"

namespace hd_cv {
    class CameraCapturer {
    public:
        CameraCapturer() : _capture(cv::VideoCapture(0)), _mog(new cv::BackgroundSubtractorMOG(10, 2, 0.1f)), _frame(cv::Mat(defaultSize.width, defaultSize.height, CV_16UC1)), _output(cv::Mat(defaultSize.width, defaultSize.height, CV_16UC1)) {
            _capture.set(CV_CAP_PROP_FRAME_WIDTH, defaultSize.width);
            _capture.set(CV_CAP_PROP_FRAME_HEIGHT, defaultSize.height);
            _capture.set(CV_CAP_PROP_FPS, 30);
        }
        
//        const cv::VideoCapture& capture() const{
//            return _capture;
//        }
//        
//        const cv::Mat& frame() const{
//            return _frame;
//        }
        
        const cv::Mat& output() const{
            return _output;
        }
        
//        cv::Mat& output() {
//            return _output;
//        }
        
//        const cv::BackgroundSubtractorMOG& mog() const{
//            return (*_mog);
//        }
        void foo(std::function<void(cv::Mat&)>f);
        void start_capturing_with_mog();
        
    private:
        cv::VideoCapture _capture;
        cv::Mat _frame;
        cv::Mat _output;
        cv::Ptr<cv::BackgroundSubtractorMOG> _mog;
    };
}

#endif /* defined(__HandDetectionOpenCV__CameraCapturer__) */
