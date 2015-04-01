//
//  AreasFinder.h
//  HandDetectionOpenCV
//
//  Created by Vlad Mihaylenko on 24/03/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#ifndef __HandDetectionOpenCV__AreasFinder__
#define __HandDetectionOpenCV__AreasFinder__
#include <opencv2/opencv.hpp>

namespace hd_cv
{
    
class AreasFinder
{
public:
    static cv::Mat areas_two_pass(const cv::Mat& image);
    
private:
    template <typename T>
    static void custom_merge(std::vector<std::set<T>>& v);
    
    template <typename T>
    static std::set<T> custom_set_union(const std::set<T>& lhs, const std::set<T>& rhs);
    
    template <typename T>
    static bool all_distinct(const std::vector<std::set<T>>& v);
    
    template <typename T>
    static bool intersection(const std::set<T>& lhs, const std::set<T>& rhs);
};
    
}

#endif /* defined(__HandDetectionOpenCV__AreasFinder__) */
