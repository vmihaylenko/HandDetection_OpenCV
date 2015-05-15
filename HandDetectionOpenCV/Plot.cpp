//
//  Plot.cpp
//  HandDetectionOpenCV
//
//  Created by Vlad Mihaylenko on 05/04/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#include "Plot.h"
#include <opencv2/opencv.hpp>

namespace hd_cv
{
  void plot(const std::vector<double>& v, const std::string& name)
  {
    const auto step = 3;
    auto length = v.size();
    std::cout<<"contour length "<<length<<std::endl;
    auto max = *std::max_element(v.begin(), v.end());
    if (max == 0) {max = 1;};
    cv::Mat mat = cv::Mat::zeros(200, static_cast<int>(step * length), CV_8UC3);
    std::vector<cv::Point> result (length + 2);
    result[0] = cv::Point(0, 200);
    result[result.size()-1] = cv::Point(length * step, 200);
    
    
    for (auto i = 1; i < result.size() - 1; i++) {
      result[i] = cv::Point(i * step, static_cast<int>(200 - ((150/max)*v[i])));
      //std::cout<<result[i].x<<" "<<result[i].y<<" "<<v[i]<<std::endl;
    }
    
    std::vector<std::vector<cv::Point>> res {result};
    cv::drawContours(mat, res, -1, cv::Scalar(255, 0, 0), 2);
    cv::imshow(name, mat);
  }
}
