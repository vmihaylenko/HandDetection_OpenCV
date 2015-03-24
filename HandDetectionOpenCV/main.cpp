#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.hpp>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include "ProcessHelper.h"
#include "CameraCapturer.h"
#include "AreasFinder.h"

void custom_main() {
    hd_cv::CameraCapturer cc = hd_cv::CameraCapturer();
    cc.start_capturing_with_mog();
    cv::Mat src = cc.output();
    
    cv::Mat connected = hd_cv::AreasFinder::areas_two_pass(src);
    imshow("connected", connected * 1000);
    imshow("src", src * 40);
    cv::waitKey(0);
}


int main() {
    custom_main();
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

