#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.hpp>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include "ProcessHelper.h"
#include "CameraCapturer.h"
#include "AreasFinder.h"
#include <unordered_map>

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

typedef std::pair<int, int> pair_of_int;
typedef std::vector<pair_of_int> contour_type;
typedef std::vector<contour_type> vector_of_contour;





using namespace std;
using namespace cv;
using namespace hd_cv;

pixelType label(const cv::Mat& image, int i, int j) {
    const bool firstIsValid = i >= 0 && i < image.size().height;
    const bool secondIsValid = j >= 0 && j < image.size().width;
    const bool all_is_valid = firstIsValid && secondIsValid;
    if (all_is_valid) {
        return image.at<pixelType>(i, j);
    }
    return 0;
}
vector<pair_of_int> neighbours(const cv::Mat& image, int i, int j) {
    return {make_pair(i, j - 1), make_pair(i-1, j-1), make_pair(i-1, j), make_pair(i - 1, j + 1),  make_pair(i, j + 1), make_pair(i + 1, j + 1), make_pair(i + 1, j), make_pair(i + 1, j - 1),};
//    vector<pair_of_int> result {};
//    for (auto &x : ns) {
//        const bool firstIsValid = x.first >= 0 && x.first < image.size().height;
//        const bool secondIsValid = x.second >= 0 && x.second < image.size().width;
//        const bool pixelIsValid = image.at<pixelType>(i,j) > 0;
//        const bool allIsValid = firstIsValid && secondIsValid && pixelIsValid;
//        if (allIsValid) {
//            result.push_back(x);
//        }
//    }
//    return result;
}

contour_type contour(const Mat& image, const pair_of_int& start, hd_cv::pixelType const current_label) {
    contour_type v {start};
    int i = start.first;
    int j = start.second;
    pair_of_int enter_from = std::make_pair(i - 1, j);
    pair_of_int next_pixel = std::make_pair(- 1, -1);
    while (next_pixel != start) {
        pair_of_int p = v[v.size() - 1];
        auto ns = neighbours(image, p.first, p.second);
        auto e_f = find(ns.begin(), ns.end(), enter_from);
        vector<pair_of_int> tempV (ns.size());
//        move(e_f, ns.end(), tempV);
//        move(ns.begin(), e_f, tempV);
        vector<pixelType> ns_labels (ns.size());
        auto iter = 0;
//        for (auto i = 0; i < ns_labels.size(); i++) {
        for (auto& x :ns_labels) {
            ns_labels[iter] = label(image, ns[iter].first, ns[iter].second);
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
        for (auto& x : v) {
            swap(x.first, x.second);
        }
    }
    return v;
}

vector_of_contour countour_moore(const cv::Mat& mat) {
    vector_of_contour v {};
    vector<pixelType> labels;
    ProcessHelper::process_mat(mat, [&mat, &labels, &v](int i, int j) {
        pixelType current_label = mat.at<pixelType>(i, j);
        if (find(labels.begin(), labels.end(), current_label) == labels.end()) {
            v.push_back(contour(mat, make_pair(i, j), current_label));
            labels.push_back(current_label);
        }
    });
    return v;
}

void custom_main() {
    hd_cv::CameraCapturer cc = hd_cv::CameraCapturer();
    cc.start_capturing_with_mog();
    cv::Mat src = cc.output();
    
    cv::Mat connected = hd_cv::AreasFinder::areas_two_pass(src);
    
    static const int minimum_size = 500;
    remove_small_areas(connected, minimum_size);
    
    auto v = countour_moore(connected);
    drawContours(connected, v, -1, Scalar(255, 0, 0));
    imshow("connected", connected * 1000);
    imshow("src", src * 40);
    cv::waitKey(0);
}


int main() {
    auto test_pair = make_pair(3, 4);
    vector<pair<int, int>> v = {make_pair(1, 2), make_pair(1, 2), test_pair, make_pair(5, 6), make_pair(7, 8)};
    auto e = find(v.begin(), v.end(), test_pair);
    
    iter_swap(e - 1, e);
//    swap_ranges(e, e + v.size() - 1, e - 1);
//    vector<pair<int, int>> v1 (v.size());
//    move(e, v.end(), v1.begin());
//    move(v.begin(), e, v1.end() - e);
    
    for (auto& x : v) {
        cout << x.first << " " << x.second << "\n";
    }
//    custom_main();
    return 0;
}


//old impliantation
using namespace cv;

void main_openCV() {
    hd_cv::CameraCapturer cc = hd_cv::CameraCapturer();
    cc.start_capturing_with_mog();
    Mat src = cc.output();
    if (!src.data)
        return;
    
    imshow("src", src);
    
    // Create binary image from source image
    Mat bw;
    cvtColor(src, bw, CV_GRAY2BGR);
    //    threshold(bw, bw, 40, 255, CV_THRESH_BINARY);
    //    imshow("bw", bw);
    
    // Perform the distance transform algorithm
    Mat dist;
    distanceTransform(src, dist, CV_DIST_L2, 3);
    
    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    normalize(dist, dist, 0, 1., NORM_MINMAX);
    //    imshow("dist", dist);
    
    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
    threshold(dist, dist, .5, 1., CV_THRESH_BINARY);
    //    imshow("dist2", dist);
    
    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);
    
    // Find total markers
    std::vector<std::vector<Point> > contours;
    findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    auto ncomp = contours.size();
    
    // Create the marker image for the watershed algorithm
    Mat markers = Mat::zeros(dist.size(), CV_32SC1);
    
    // Draw the foreground markers
    for (auto i = 0; i < ncomp; i++)
        drawContours(markers, contours, i, Scalar::all(i+1), -1);
    
    // Draw the background marker
    circle(markers, Point(5,5), 3, CV_RGB(255,255,255), -1);
    //    imshow("markers", markers*10000);
    
    // Perform the watershed algorithm
    watershed(bw, markers);
    
    // Generate random colors
    std::vector<Vec3b> colors;
    for (auto i = 0; i < ncomp; i++) {
        auto b = theRNG().uniform(0, 255);
        auto g = theRNG().uniform(0, 255);
        auto r = theRNG().uniform(0, 255);
        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }
    
    // Create the result image
    Mat dst = Mat::zeros(markers.size(), CV_8UC3);
    
    // Fill labeled objects with random colors
    std::vector<int> sizesOfAreas {static_cast<int>(ncomp)};
    
    hd_cv::ProcessHelper::process_mat(markers, [&markers, &sizesOfAreas](int i, int j) {
        auto index = markers.at<int>(i,j);
        sizesOfAreas[index]++;
    });
    
    hd_cv::ProcessHelper::process_mat(markers, [&markers, &sizesOfAreas, &dst, &colors](int i, int j) {
        int index = markers.at<int>(i,j);
        if (sizesOfAreas[index] < 1000) {
            markers.at<int>(i,j) = 0;
            dst.at<Vec3b>(i,j) = Vec3b(0,0,0);
        } else {
            dst.at<Vec3b>(i,j) = colors[index-1];
        }
    });
    
    
    imshow("dst", dst);
    
    waitKey(0);
}

