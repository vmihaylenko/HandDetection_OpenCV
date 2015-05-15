//
//  AreasFinder.cpp
//  HandDetectionOpenCV
//
//  Created by Vlad Mihaylenko on 24/03/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#include "AreasFinder.h"
#include "ProcessHelper.h"

namespace hd_cv
{
    
using namespace std;
using namespace cv;

template <typename T>
bool AreasFinder::intersection(const set<T>& lhs, const set<T>& rhs) {
    for (auto& x : lhs) {
        for (auto& y : rhs) {
            if (x == y) {
                return true;
            }
        }
    }
    return false;
}

template <typename T>
bool AreasFinder::all_distinct(const vector<set<T>>& v) {
    for (auto i = v.begin(); i != v.end(); ++i) {
        for (auto j = i + 1; j != v.end(); ++j) {
            if (intersection(*i, *j)) {
                return false;
            }
        }
    }
    return true;
}

template <typename T>
set<T> AreasFinder::custom_set_union(const set<T>& lhs, const set<T>& rhs) {
    set<T> result = lhs;
    for (auto& x : rhs) {
        result.insert(x);
    }
    return result;
}

template <typename T>
void AreasFinder::custom_merge(vector<set<T>>& v) {
    vector<set<T>> tmp = v;
    while (!all_distinct(v)) {
        for (auto i = v.begin(); i != v.end(); ++i) {
            for (auto j = i + 1; j != v.end(); ++j) {
                if (intersection(*i, *j)) {
                    set<T> s = custom_set_union(*i, *j);
                    if (find(tmp.begin(), tmp.end(), *i) != tmp.end())
                        tmp.erase(remove(tmp.begin(), tmp.end(), *i), tmp.end());
                    if (find(tmp.begin(), tmp.end(), *j) != tmp.end())
                        tmp.erase(remove(tmp.begin(), tmp.end(), *j), tmp.end());
                    tmp.push_back(s);
                }
            }
        }
        v = tmp;
    }
}

Mat AreasFinder::areas_two_pass(const Mat& image) {
    typedef vector<set<pixelType>> vectorOfSets;
    Mat connected = Mat::zeros(image.rows, image.cols, CV_8UC1);
    /*
    for (auto i=0; i<connected.rows; i++)
        for(auto j=0; j<connected.cols; j++)
            connected.at<pixelType>(i,j) = 0;
    */    
    int objects = 0;
    vectorOfSets equivalences {};
    ProcessHelper::process_mat(image, [&image, &connected, &objects, &equivalences](int i, int j) {
        auto currentValue = image.at<pixelType>(i, j);
        auto currentLabel = connected.at<pixelType>(i, j);
        if (currentValue) {
            pixelType westValue = 0;
            pixelType westLabel = 0;
            if (j - 1 >= 0) {
                westValue = image.at<pixelType>(i, j - 1);
                westLabel = connected.at<pixelType>(i, j - 1);
            }
            pixelType northValue = 0;
            pixelType northLabel = 0;
            if (i - 1 >= 0) {
                northValue = image.at<pixelType>(i - 1, j);
                northLabel = connected.at<pixelType>(i - 1, j);
            }
            if (westValue != currentValue && northValue != currentValue) {
                objects++;
                currentLabel = objects;
                
            } else if (westValue == currentValue && northValue != currentValue) {
                currentLabel = westLabel;
            } else if (westValue != currentValue && northValue == currentValue) {
                currentLabel = northLabel;
            } else if (westValue == currentValue && northValue == currentValue) {
                currentLabel = westLabel;
                if (westLabel != northLabel) {
                    set<pixelType> s {westLabel, northLabel};
                    connected.at<pixelType>(i, j - 1) = northLabel;
                    equivalences.push_back(s);
                }
                custom_merge(equivalences);
                
            }
            
            connected.at<pixelType>(i,j) = currentLabel;
            
        }
    });
    
    custom_merge(equivalences);
    
    std::vector<pixelType> labelMap (objects + 1);
    auto iterator = 0;
    for (auto& x : labelMap) {
        x = iterator;
        iterator++;
    }
    
    iterator = 1;
    for (auto& x : equivalences) {
        //        auto m = *min_element(x.begin(), x.end());
        for (auto& y : x) {
            labelMap[y] = iterator;
        }
        iterator++;
    }
    
    ProcessHelper::process_mat(connected, [&connected, &labelMap](int i, int j) {
        connected.at<pixelType>(i, j) = labelMap[connected.at<pixelType>(i, j)];
    });
    std::cout<<connected.size()<<std::endl;
    return connected;
}
    
}
