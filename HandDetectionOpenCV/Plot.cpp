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
        const auto step = 10;
        auto length = v.size();
        cv::Mat mat = cv::Mat::zeros(200, static_cast<int>(step * length), CV_8UC3);
        std::vector<cv::Point> result (length);
        for (auto i = 0; i < length; i++)
        {
            result[i] = cv::Point(i * step, 200 - static_cast<int>(200 * v[i]));
        }
        std::vector<std::vector<cv::Point>> res {result};
        cv::drawContours(mat, res, -1, cv::Scalar(255, 0, 0), 5);
        cv::imshow(name, mat);
    }
}
