//
//  CameraCapturer.cpp
//  HandDetectionOpenCV
//
//  Created by Vlad Mihaylenko on 24/03/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#include "CameraCapturer.h"
#include "ProcessHelper.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.hpp>

namespace hd_cv {
    void CameraCapturer::start_capturing_with_mog() {
        cvNamedWindow("capture", CV_WINDOW_NORMAL);
        while (true) {
            if (!_capture.read(_frame)) {
                exit(EXIT_FAILURE);
            }
            (*_mog)(_frame, _output, 0.01f);
            imshow("FG Mask MOG", _output);
            
            char c = cvWaitKey(33);
            if (c == 27) {
                break;
            }
        }
        _output.convertTo(_output, CV_16UC1);
        
        ProcessHelper::process_mat(_output, [this](int i, int j) {
            auto p = _output.at<pixelType>(i,j);
            if (p > 0) {
                _output.at<pixelType>(i,j) = 255;
            }
        });
        _capture.release();
    }
    
    void CameraCapturer::foo(std::function<void(cv::Mat&)>f) {
        cvNamedWindow("capture", CV_WINDOW_NORMAL);
        while (true) {
            if (!_capture.read(_frame)) {
                exit(EXIT_FAILURE);
            }
            (*_mog)(_frame, _output, 0.01f);
            _output.convertTo(_output, CV_16UC1);
            
            ProcessHelper::process_mat(_output, [this](int i, int j) {
                auto p = _output.at<pixelType>(i,j);
                if (p > 0) {
                    _output.at<pixelType>(i,j) = 255;
                }
            });
            char c = cvWaitKey(33);
            if (c == 27) {
                break;
            }
            f(_output);
        }
        _capture.release();
    }
}
