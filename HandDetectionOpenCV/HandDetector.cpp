//
//  HandDetector.cpp
//  HandDetectionOpenCV
//
//  Created by Vlad Mihaylenko on 01/04/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#include "HandDetector.h"
#include "AreasFinder.h"
#include "RemoveAreas.h"
#include "Contour.h"


namespace hd_cv
{
    
void HandDetector::detect_hand_on_image(const cv::Mat &im)
{
    auto connected = AreasFinder::areas_two_pass(im);
    static const int minimum_size = 500;
    remove_small_areas(connected, minimum_size);
    
    auto v = countour_moore(connected);
    for (auto& c : v) {
        for (auto& p : c) {
            std::swap(p.x, p.y);
        }
    }
    cv::drawContours(connected, v, -1, cv::Scalar(255, 0, 0), 5);
    cv::imshow("connected", connected * 1000);
    v.erase(remove_if(v.begin(), v.end(), is_not_palm), v.end());
    cv::Mat result = cv::Mat::zeros(im.size(), CV_8UC3);
    cv::drawContours(result, v, -1, cv::Scalar(255, 0, 0), 5);
    imshow("src", result);
    cv::waitKey(0);
}
    
}