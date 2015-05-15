//
//  Contour.h
//  HandDetectionOpenCV
//
//  Created by Vlad Mihaylenko on 01/04/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#ifndef __HandDetectionOpenCV__Contour__
#define __HandDetectionOpenCV__Contour__
#include <opencv2/opencv.hpp>

typedef std::vector<cv::Point> contour_type;
typedef std::vector<contour_type> vector_of_contour;

namespace hd_cv
{
    vector_of_contour countour_moore(const cv::Mat& mat);
    bool is_not_palm(contour_type& c);
    std::vector<double> contour_coors_to_curvature(const contour_type& contours);
}

#endif /* defined(__HandDetectionOpenCV__Contour__) */
