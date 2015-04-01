//
//  Contour.cpp
//  HandDetectionOpenCV
//
//  Created by Vlad Mihaylenko on 01/04/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#include "Contour.h"
#include "ProcessHelper.h"
#include "FFT.h"


extern std::vector<cv::Point> palm_controur;

namespace hd_cv
{
    
using namespace std;
using namespace cv;
using namespace FFT;
    
pixelType label(const Mat& image, int i, int j) {
    const bool firstIsValid = i >= 0 && i < image.size().height;
    const bool secondIsValid = j >= 0 && j < image.size().width;
    const bool all_is_valid = firstIsValid && secondIsValid;
    if (all_is_valid) {
        return image.at<pixelType>(i, j);
    }
    return 0;
}
    
vector<Point> neighbours(const Mat& image, int i, int j) {
    return {Point(i, j - 1), Point(i-1, j-1), Point(i-1, j), Point(i - 1, j + 1), Point(i, j + 1), Point(i + 1, j + 1), Point(i + 1, j), Point(i + 1, j - 1),};
}

contour_type contour(const Mat& image, const Point& start, pixelType const current_label) {
    contour_type v {start};
    int i = start.x;
    int j = start.y;
    Point enter_from = Point(i - 1, j);
    Point next_pixel = Point(- 1, -1);
    while (next_pixel != start) {
        auto p = v[v.size() - 1];
        auto ns = neighbours(image, p.x, p.y);
        auto e_f = find(ns.begin(), ns.end(), enter_from);
        rotate(ns.begin(), e_f, ns.end());
        vector<pixelType> ns_labels (ns.size());
        auto iter = 0;
        for (auto& x :ns_labels) {
            ns_labels[iter] = label(image, ns[iter].x, ns[iter].y);
            iter++;
        }
        auto it = find(ns_labels.begin(), ns_labels.end(), current_label) ;
        long pixel_with_label_index;
        if (it != ns_labels.end()) {
            pixel_with_label_index = it - ns_labels.begin();
        } else {
            pixel_with_label_index = -666;
            break;
        }
        next_pixel = ns[pixel_with_label_index];
        if (pixel_with_label_index == 0) {
            enter_from = ns[ns.size() - 1];
        } else {
            enter_from = ns[pixel_with_label_index - 1];
        }
        v.push_back(next_pixel);
    }
    return v;
}

vector_of_contour countour_moore(const Mat& mat) {
    vector_of_contour v;
    vector<pixelType> labels;
    ProcessHelper::process_mat(mat, [&mat, &labels, &v](int i, int j) {
        pixelType current_label = mat.at<pixelType>(i, j);
        if (find(labels.begin(), labels.end(), current_label) == labels.end()) {
            v.push_back(contour(mat, Point(i, j), current_label));
            labels.push_back(current_label);
        }
    });
    return v;
}

CArray contour_coors_to_complex(const contour_type& v) {
    CArray r (v.size());
    auto i = 0;
    for (auto& x : v) {
        auto j = (i + 1) % v.size();
        auto p = v[j] - x;
        r[i] = complex<double>(p.x, p.y);
        i++;
    }
    return r;
}

void contour_reduce(contour_type& v, const int length) {
    if (!length || v.size() < length)
        return;
    
    auto step = ceil(v.size() / length);
    contour_type temp_v;
    for (auto i = v.begin(); i - v.begin() < v.size(); i += step)
        temp_v.push_back(*i);
    
    
    auto elements_to_delete = temp_v.size() - length;
    if (!elements_to_delete)
        return;
    
    step = ceil(temp_v.size()/elements_to_delete);
    auto iterator = temp_v.begin();
    
    while (iterator - v.begin() < v.size()) {
        iterator = temp_v.erase(iterator);
        iterator += step - 1;
    }
    
    v = temp_v;
}

void contour_extend(contour_type& v, const int length) {
    if (!length || v.size() > length)
        return;
    
    vector<pixelType> edges(v.size());
    auto i = 0;
    for (auto& x : v) {
        Point p;
        if (i > 0)
            p = x - v[i-1];
        else
            p = x - v[v.size() - 1];
        auto p_norm = sqrt(p.x * p.x + p.y * p.y);
        edges[i] = p_norm;
        i++;
    }
    
    while (v.size() < length) {
        auto max_edges_index = max_element(edges.begin(), edges.end()) - edges.begin();
        Point new_vertice;
        if (max_edges_index > 0)
            new_vertice = v[max_edges_index - 1] + Point((v[max_edges_index] - v[max_edges_index - 1]).x/2, (v[max_edges_index] - v[max_edges_index - 1]).y/2);
        else
            new_vertice = v[v.size() - 1] + Point((v[max_edges_index] - v[v.size() - 1]).x/2, (v[max_edges_index] - v[v.size() - 1]).y/2);
        v.insert(v.begin() + max_edges_index, new_vertice);
        edges[max_edges_index] /= 2;
        edges.insert(edges.begin() + max_edges_index, edges[max_edges_index]);
    }
}

void contour_change_length(contour_type& v, const int length) {
    if (v.size() > length)
        contour_reduce(v, length);
    else
        contour_extend(v, length);
}

double contours_compare(contour_type& a, contour_type& b) {
    auto length = MIN(100, ceil((a.size() + b.size())/2));
    contour_change_length(a, length);
    contour_change_length(b, length);
    auto first = contour_coors_to_complex(a);
    auto second = contour_coors_to_complex(b);
    
    first /= abs(first);
    second /= abs(second);
    
    fft(first);
    fft(second);
    second = second.apply(conj);
    CArray multiplication_result = first * second;
    ifft(multiplication_result);
    auto final = multiplication_result.apply(real);
    auto result_vector = vallaray_to_vector(final);
    auto max = *max_element(result_vector.begin(), result_vector.end());
    return max/length;
}

bool is_not_palm(contour_type& c) {
    static constexpr auto threshold = 0.1f;
    auto compare = contours_compare(c, palm_controur);
    return compare < threshold;
}
    
}
