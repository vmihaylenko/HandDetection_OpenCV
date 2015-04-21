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
    auto max = *std::max_element(v.begin(), v.end());
    cv::Mat mat = cv::Mat::zeros(200, static_cast<int>(step * length), CV_8UC3);
    std::vector<cv::Point> result (length + 2);
    result[0] = cv::Point(0, 200);
    
    for (auto i = 1; i < result.size() - 1; i++)
      result[i] = cv::Point(i * step, static_cast<int>(200 - ((150/max)*v[i])));
    
    result[result.size() - 1] = cv::Point(0, 200);
    
    std::vector<std::vector<cv::Point>> res {result};
    cv::drawContours(mat, res, -1, cv::Scalar(255, 0, 0), 2);
    cv::imshow(name, mat);
  }
}
