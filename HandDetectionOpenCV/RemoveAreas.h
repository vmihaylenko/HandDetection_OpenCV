//
//  RemoveAreas.h
//  HandDetectionOpenCV
//
//  Created by Vlad Mihaylenko on 01/04/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#ifndef __HandDetectionOpenCV__RemoveAreas__
#define __HandDetectionOpenCV__RemoveAreas__
#include <opencv2/opencv.hpp>

namespace hd_cv
{
    void remove_small_areas(cv::Mat& mat, const int minimum_area_size);
}

#endif /* defined(__HandDetectionOpenCV__RemoveAreas__) */
