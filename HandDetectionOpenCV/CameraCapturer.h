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
        CameraCapturer() : _capture(cv::VideoCapture(0)), _mog(new cv::BackgroundSubtractorMOG()), _frame(cv::Mat(defaultSize.width, defaultSize.height, CV_16UC1)), _output(cv::Mat(defaultSize.width, defaultSize.height, CV_16UC1)) {
            
        }
        
        const cv::VideoCapture& capture() const{
            return _capture;
        }
        
        const cv::Mat& frame() const{
            return _frame;
        }
        
        const cv::Mat& output() const{
            return _output;
        }
        
        cv::Mat& output() {
            return _output;
        }
        
        const cv::BackgroundSubtractorMOG& mog() const{
            return (*_mog);
        }
        
        void start_capturing_with_mog();
        
    private:
        cv::VideoCapture _capture;
        cv::Mat _frame;
        cv::Mat _output;
        cv::Ptr<cv::BackgroundSubtractorMOG> _mog;
    };
}

#endif /* defined(__HandDetectionOpenCV__CameraCapturer__) */
