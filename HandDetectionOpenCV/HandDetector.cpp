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
#include "Plot.h"


namespace hd_cv
{
    
void HandDetector::detect_hand_on_image(const cv::Mat &im)
{
    std::cout<<im.size()<<std::endl;
    cv::imshow("image", im);
    std::cout<<im.size()<<std::endl;
    
    auto connected = AreasFinder::areas_two_pass(im);
    static const int minimum_size = 10;
    remove_small_areas(connected, minimum_size);
    
    auto v = countour_moore(connected);
    for (auto& c : v) {
        for (auto& p : c) {
            std::swap(p.x, p.y);
        }
    }
    std::cout<<connected.size()<<std::endl;
    cv::drawContours(connected, v, -1, cv::Scalar(255, 0, 0), 5);
    cv::imshow("connected", connected * 10);
    //v.erase(remove_if(v.begin(), v.end(), is_not_palm), v.end());
    cv::Mat result = cv::Mat::zeros(im.size(), CV_8UC1);
    cv::drawContours(result, v, -1, cv::Scalar(255, 0, 0), 5);
    std::cout<<connected.size()<<std::endl;    
    imshow("src", result);
    auto i = 1;
    for (auto& x : v)
    {
        const std::string s (i, 'x');
        plot(contour_coors_to_curvature(x), s);
        i++;
    }
    cv::waitKey(0);
}
    
}
