//
//  RemoveAreas.cpp
//  HandDetectionOpenCV
//
//  Created by Vlad Mihaylenko on 01/04/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#include "RemoveAreas.h"
#include "ProcessHelper.h"
#include <unordered_map>
#include <vector>
#include <utility>

namespace hd_cv
{
    void remove_small_areas(cv::Mat& mat, const int minimum_area_size) {
        std::unordered_map<hd_cv::pixelType, std::vector<std::pair<int, int>>> map;
        hd_cv::ProcessHelper::process_mat(mat, [&mat, &map, minimum_area_size](int i, int j) {
            auto current_pixel = mat.at<hd_cv::pixelType>(i, j);
            if (current_pixel > 0) {
                auto iter = map.find(current_pixel);
                if (iter == map.end()) {
                    std::vector<std::pair<int, int>> v {std::make_pair(i, j)};
                    map.emplace(current_pixel, v);
                } else {
                    if (iter->second.size() < minimum_area_size) {
                        iter->second.push_back(std::make_pair(i, j));
                    }
                }
            }
        });
        
        for (auto& x : map) {
            std::vector<std::pair<int, int>> v = x.second;
            if (v.size() < minimum_area_size) {
                for (auto& y : v) {
                    mat.at<hd_cv::pixelType>(y.first, y.second) = 0;
                }
            }
        }
    }
    

}