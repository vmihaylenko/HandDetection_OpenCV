#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.hpp>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <set>

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
    CameraCapturer() : _capture(VideoCapture(0)), _mog(new BackgroundSubtractorMOG()), _frame(Mat()), _output(Mat()) {
        
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
        _capture.release();
    }
    
private:
    VideoCapture _capture;
    Mat _frame;
    Mat _output;
    Ptr<BackgroundSubtractorMOG> _mog;
};

typedef uint8_t pixelType;
typedef std::pair<pixelType, pixelType> pixelPair;

using namespace std;

Mat areas_two_pass(const Mat& image, int minpixels = 10) {
    typedef uint16_t pixelType;
    typedef std::pair<pixelType, pixelType> pair;
    Mat connected = Mat::zeros(image.size(), CV_8UC1);
    int objects = 0;
    std::set<pair> equivalences {};
//    process_mat_invert(image, [&image, &connected, &objects, &equivalences](int i, int j) {
    for (auto i = 0; i < image.rows; i++) {
        for (auto j = 0; j < image.cols; j++) {
            
            auto currentValue = image.at<pixelType>(i, j);
            auto currentLabel = connected.at<pixelType>(i, j);
            if (currentValue) {
                pixelType westValue = 0;
                pixelType westLabel = 0;
                if (i - 1 >= 0) {
                    westValue = image.at<pixelType>(i - 1, j);
                    westLabel = connected.at<pixelType>(i - 1, j);
                }
                pixelType northValue = 0;
                pixelType northLabel = 0;
                if (j - 1 >= 0) {
                    northValue = image.at<pixelType>(i, j - 1);
                    northLabel = connected.at<pixelType>(i, j - 1);
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
                    equivalences.insert(std::make_pair(westLabel, northLabel));
                    
    //                bool flag = false;
    //                westLabel = northLabel;
    //                currentLabel = northLabel;
    //                for (auto &currentSet : equivalences) {
    //                    bool setHasWest = std::find(currentSet.begin(), currentSet.end(), westLabel) != currentSet.end();
    //                    bool setHasNorth = std::find(currentSet.begin(), currentSet.end(), northLabel) != currentSet.end();
    //                    if (setHasNorth || setHasWest) {
    //                        std::set<pixelType> tempSet = currentSet;
    //                        equivalences.erase(currentSet);
    //                        tempSet.insert(westLabel);
    //                        tempSet.insert(northLabel);
    //                        equivalences.insert(tempSet);
    //                        flag = true;
    //                        break;
    //                    }
    //                }
    //                if (!flag)
    //                    equivalences.insert(std::set<pixelType> {westLabel, northLabel});
                    
                }
                connected.at<pixelType>(i,j) = currentLabel;
            }
        }
    }
    typedef std::set<std::set<pixelType>>::iterator pixelsIterator;
    
    std::set<pair> tempSet = equivalences;
    for (const auto& x : tempSet) {
        equivalences.insert(std::make_pair(x.second, x.first));
    }
   
    std::vector<pixelType> labelMap (objects + 1);
    
    std::vector<pixelType> js {};
    for (auto label = 1; label < objects + 1; label++ ) {
        for (auto& x : equivalences) {
            if (x.first == label) {
                js.push_back(x.second);
            }
        }
        auto minElemet = std::min_element(js.begin(), js.end());
        if (minElemet == js.end()) {
            labelMap[label] = label;
        } else {
            labelMap[label] = *minElemet;
        }
        
    }
    
//    for (auto &currentSet : equivalences) {
//        auto minElemet = *std::min_element(currentSet.begin(), currentSet.end());
//        for (auto &label : currentSet) {
//            if (minElemet < labelMap[label])
//                labelMap[label] = minElemet;
//        }
//    }
    
    for (auto i = 0; i < connected.rows; i++) {
        for (auto j = 0; j < connected.cols; j++) {
            connected.at<pixelType>(i, j) = labelMap[connected.at<pixelType>(i, j)];
        }
    }
//    process_mat_invert(connected, [&image, &connected, &labelMap](int i, int j) {
//        connected.at<pixelType>(i, j) = labelMap[connected.at<pixelType>(i, j)];
////        connected.at<pixelType>(i, j) = 255;
//        
////        connected.at<pixelType>(i, j) = i + j;
////        std::cout << (i + j) << std::endl ;
////        connected.at<pixelType>(i, j) = i;
//
//    });
    
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

vector<pixelPair> neighbours(const Mat& image, const Mat& connected, int i, int j) {
    vector<pixelPair> ns {make_pair(i, j - 1), make_pair(i, j + 1), make_pair(i + 1, j), make_pair(i - 1, j), make_pair(i + 1, j + 1), make_pair(i - 1, j - 1), make_pair(i + 1, j - 1), make_pair(i - 1, j + 1)};
    vector<pixelPair> result {};
    for (auto &x : ns) {
        bool firstIsValid = x.first >= 0 && x.first < image.size().height;
        bool secondIsValid = x.second >= 0 && x.second < image.size().width;
        bool pixelIsValid = image.at<pixelType>(i,j) > 0;
        bool connectedIsValid = connected.at<pixelType>(i, j) == 0;
        bool allIsValid = firstIsValid && secondIsValid && pixelIsValid && connectedIsValid;
        if (allIsValid) {
            result.push_back(x);
        }
        
    }
    return result;
}

int fill_neighbours(Mat& image, Mat& connected, int i, int j, int label) {
    auto pixelsCount = 0;
    vector<pixelPair> pixels {std::make_pair(i, j)};
    while (!pixels.empty()) {
        pixelPair currentPixel = pixels.back();
        pixels.pop_back();
        pixelsCount++;
        vector<pixelPair> n = neighbours(image, connected, currentPixel.first, currentPixel.second);
        pixels.insert(pixels.end(), n.begin(), n.end());
        connected.at<pixelType>(currentPixel.first, currentPixel.second) = label;
        image.at<pixelType>(currentPixel.first, currentPixel.second) = 0;
      
    }
    
    
    return pixelsCount;
}

Mat areas_depth_first(const Mat& image, int minpixels = 10) {
    Mat connected = Mat::zeros(image.size(), CV_8UC1);
    int objects = 0;
    Mat tmpImage = image;
    auto pixelsCount = 0;
    process_mat(tmpImage, [&tmpImage, &connected, &objects, &pixelsCount] (int i, int j) {
        if (tmpImage.at<pixelType>(i, j) > 0) {
            objects += 10;
            pixelsCount = fill_neighbours(tmpImage, connected, i, j, objects);
            cout<<pixelsCount<<"\n";
            
        }
    });
    return connected;
}

void custom_main() {
//    IplImage *image = nullptr;
//    image = cvLoadImage("/Users/vladmihaylenko/Study/CV/HandDetectionOpenCV/HandDetectionOpenCV/test.jpg");
//
    CameraCapturer cc = CameraCapturer();
    cc.start_capturing_with_mog();
    Mat src = cc.output();
    
//    resize(src, src, Size(256, 128), CV_INTER_NN);
    
//    Mat src = cc.output();
    if (!src.data)
        return;
    
    imshow("src", src);
    Mat connected = areas_depth_first(src);
//    resize(connected, connected, Size(1024, 512), CV_INTER_NN);
    imshow("connected", connected );
    waitKey(0);
}


int main() {
    custom_main();
//    main_openCV();
    return 0;
}


