//
//  HandDetector.h
//  HandDetectionOpenCV
//
//  Created by Vlad Mihaylenko on 01/04/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#ifndef __HandDetectionOpenCV__HandDetector__
#define __HandDetectionOpenCV__HandDetector__
#include <opencv2/opencv.hpp>

namespace hd_cv
{
    
class HandDetector
{
public:
    static void detect_hand_on_image(const cv::Mat& im);
};
    
}

#endif /* defined(__HandDetectionOpenCV__HandDetector__) */
