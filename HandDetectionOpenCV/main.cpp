#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.hpp>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>

void process_mat(const cv::Mat& mat, std::function<void(int, int)>process) {
    for (auto i = 0; i < mat.rows; i++) {
        for (auto j = 0; j < mat.cols; j++) {
            process(i, j);
        }
    }
}

void process_mat_invert(const cv::Mat& mat, std::function<void(int, int)>process) {
    for (auto i = 0; i < mat.cols; i++) {
        for (auto j = 0; j < mat.rows; j++) {
            process(j, i);
        }
    }
}

using namespace cv;

class CameraCapturer {
public:
    CameraCapturer() : _capture(VideoCapture(0)), _mog(new BackgroundSubtractorMOG()), _frame(Mat(512, 256, CV_16UC1)), _output(Mat(512, 256, CV_16UC1)) {
        
    }
    
    const VideoCapture& capture() const{
        return _capture;
    }
    
    const Mat& frame() const{
        return _frame;
    }
    
    const Mat& output() const{
        return _output;
    }
    
    Mat& output() {
        return _output;
    }
    
    const BackgroundSubtractorMOG& mog() const{
        return (*_mog);
    }
    
    void start_capturing_with_mog() {
        cvNamedWindow("capture", CV_WINDOW_NORMAL);
        while (true) {
            if (!_capture.read(_frame)) {
                exit(EXIT_FAILURE);
            }
            (*_mog)(_frame, _output, 0.01f);
            imshow("FG Mask MOG", _output);
            
            char c = cvWaitKey(33);
            if (c == 27) {
                break;
            }
        }
        _output.convertTo(_output, CV_16UC1);
        _capture.release();
    }
    
private:
    VideoCapture _capture;
    Mat _frame;
    Mat _output;
    Ptr<BackgroundSubtractorMOG> _mog;
};

typedef uint16_t pixelType;
typedef std::pair<pixelType, pixelType> pixelPair;

using namespace std;


template <typename T>
bool intersection(const set<T>& lhs, const set<T>& rhs) {
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
bool all_distinct(const vector<set<T>>& v) {
    typedef typename vector<set<T>>::const_iterator constVectorIterator;
    for (constVectorIterator i = v.begin(); i != v.end(); ++i) {
        for (constVectorIterator j = i + 1; j != v.end(); ++j) {
            if (intersection(*i, *j)) {
                return false;
            }
        }
    }
    return true;
}

template <typename T>
set<T> custom_set_union(const set<T>& lhs, const set<T>& rhs) {
    set<T> result = lhs;
    for (auto& x : rhs) {
        result.insert(x);
    }
    return result;
}

template <typename T>
void custom_merge(vector<set<T>>& v) {
    vector<set<T>> tmp = v;
    while (!all_distinct(v)) {
        typedef typename vector<set<T>>::const_iterator constVectorIterator;
        for (constVectorIterator i = v.begin(); i != v.end(); ++i) {
            for (constVectorIterator j = i + 1; j != v.end(); ++j) {
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

Mat areas_two_pass(const Mat& image, int minpixels = 10) {
    typedef vector<set<pixelType>> vectorOfSets;
    typedef std::pair<pixelType, pixelType> pair;
    Mat connected = Mat::zeros(image.rows, image.cols, CV_16UC1);
    int objects = 0;
    process_mat(connected, [&connected](int i, int j) {
        
    });
//    imshow("hui", connected *20);
//    cvWaitKey(0);

    
    
    vectorOfSets equivalences {};
//    process_mat_invert(image, [&image, &connected, &objects, &equivalences](int i, int j) {
    for (auto i = 0; i < image.rows; i++) {
        for (auto j = 0; j < image.cols; j++) {
            
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
                    set<pixelType> s {westLabel, northLabel};
                    equivalences.push_back(s);
                    custom_merge(equivalences);
                    
                }
                connected.at<pixelType>(i,j) = currentLabel;
            }
        }
//        if (i%16 == 0) {
//            imshow("hui", connected *20);
//            cvWaitKey(0);
//        }
    }
    
    
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
   
    for (auto i = 0; i < connected.rows; i++) {
        for (auto j = 0; j < connected.cols; j++) {
            connected.at<pixelType>(i, j) = labelMap[connected.at<pixelType>(i, j)];
        }
    }
    
    return connected;
}

void main_openCV() {
    CameraCapturer cc = CameraCapturer();
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
    
    process_mat(markers, [&markers, &sizesOfAreas](int i, int j) {
        auto index = markers.at<int>(i,j);
        sizesOfAreas[index]++;
    });
    
    process_mat(markers, [&markers, &sizesOfAreas, &dst, &colors](int i, int j) {
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

void custom_main() {
//
    CameraCapturer cc = CameraCapturer();
    cc.start_capturing_with_mog();
    Mat src = cc.output();
//
    resize(src, src, Size(512, 256), CV_INTER_NN);
    
    process_mat(src, [&src](int i, int j) {
        auto p = src.at<pixelType>(i,j);
        if (p > 0) {
            src.at<pixelType>(i,j) = 255;
        }
    });
//
////    Mat src = cc.output();
//    if (!src.data)
//        return;
//    
    Mat connected = areas_two_pass(src);
//    resize(connected, connected, Size(1024, 512), CV_INTER_NN);
    imshow("connected", connected * 1000);
    imshow("src", src * 40);
    waitKey(0);
    string filename = "/Users/vladmihaylenko/Study/CV/HandDetectionOpenCV/HandDetectionOpenCV/connection.jpg";
    imwrite(filename, connected * 1000);
}


int main() {
//    Mat src = Mat::zeros(5, 6, CV_16UC1);
//    vector<vector<pixelType>> v = {{0,1,1,0,0,1}, {1,1,1,1,0,1}, {0,1,0,0,0,1}, {0,0,0,1,1,1}, {0,0,0,1,1,1}};
//    for (auto i = 0; i < 5; i++) {
//        for (auto j = 0; j < 6; j++) {
//            cout<<v.at(i).at(j)<<"\n";
//            src.at<pixelType>(i, j) = v.at(i).at(j);
//        }
//    }
//    cout<<"\n";
//    Mat src = imread("/Users/vladmihaylenko/Study/CV/HandDetectionOpenCV/HandDetectionOpenCV/test.tiff");
////
//    Mat connected = areas_two_pass(src);
//    imshow("hui", connected * 1000);
//    imshow("src", src);
//    waitKey(0);
//    vector<set<pixelType>> v {{1}, {1}, {2}, {2, 3}};
//    custom_merge(v);
//    for (auto& x : v) {
//        for (auto& y : x) {
//            cout<<y;
//        }
//        cout<<"\n";
//    }
    custom_main();
//    main_openCV();
    return 0;
}


