#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

std::vector<cv::Rect2i> markedRect = { cv::Rect2i( 57, 190, 548, 518 ), cv::Rect2i( 145, 167, 381, 590 ), cv::Rect2i( 218, 325, 257, 544 ),
                                       cv::Rect2i( 62, 260, 596, 596 ), cv::Rect2i( 115, 235, 455, 660 ), cv::Rect2i( 200, 220, 322, 621 ),
                                       cv::Rect2i( 425, 60, 632, 531 ), cv::Rect2i( 400, 170, 622, 322 ), cv::Rect2i( 423, 240, 612, 282 ),
                                       cv::Rect2i( 0, 268, 704, 730 ),  cv::Rect2i( 0, 330, 693, 536 ),   cv::Rect2i( 38, 367, 647, 378 ),
                                       cv::Rect2i( 11, 265, 660, 648 ), cv::Rect2i( 12, 340, 637, 443 ),  cv::Rect2i( 55, 415, 592, 312 )};

cv::Point find_center(cv::Rect2i rect) {
    return cv::Point(rect.width / 2 + rect.x, rect.height / 2 + rect.y);
}

double centreDist(cv::Point a, cv::Point b) {
    return std::abs((std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y))));
}

double rectDiff(cv::Rect2i recta, cv::Rect2i rectb) {
    return(centreDist(find_center(recta), find_center(rectb)));
}

double errorCheck(cv::Mat frame, cv::Rect a, cv::Rect b) {
    double error;
    error = std::max(a.width * a.height, b.width * b.height) / std::min(a.width * a.height, b.width * b.height) * (rectDiff(a,b)/std::sqrt((int)(frame.cols * frame.cols) + (int)(frame.rows * frame.rows)))*100;
    return error;
}

std::vector<cv::Mat> splitVideo(cv::VideoCapture& capture) {
    std::vector<cv::Mat> result;
    result.reserve(500);
    while (1) {
        cv::Mat frame;
        capture >> frame;
        if (frame.empty())
            break;
        result.push_back(frame);
    }
    capture.release();
    return result;
}

cv::Mat quantize_frame(cv::Mat& frame) {
    cv::Mat quant;
    cv::cvtColor(frame, quant, cv::COLOR_BGRA2GRAY, 0);
    return quant;
}

cv::Mat morph(cv::Mat& frame) {
    cv::Mat Morph;
    cv::morphologyEx(frame, Morph, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));
    cv::morphologyEx(Morph, Morph, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));
    /*cv::dilate(Morph, Morph, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(100, 100)));
    cv::erode(Morph, Morph, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(100, 100)));*/
    return Morph;
}

cv::Mat binarize_frame(cv::Mat& frame) {

    // watch https://docs.opencv.org/4.x/db/d8e/tutorial_threshold.html
    int threshold_value = 170;  // below this value is black and above this value is white 
    int threshold_type = 0;     // binary type (divided into black & white)
    int max_binary_value = 255;
    cv::Mat binar;
    cv::threshold(quantize_frame(frame), binar, threshold_value, max_binary_value, threshold_type);
    /*cv::adaptiveThreshold(quantize_frame(frame), binar, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 13, 0);*/
    /*cv::threshold(quantize_frame(frame), binar, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);*/
    return binar;
}

cv::Rect2i connected_components(const cv::Mat& image) {
    cv::Mat labels, stats, centroids;
    cv::connectedComponentsWithStats(image, labels, stats, centroids);

    uint max_i = 0;
    uint max_s = 0;

    for (auto i = 1; i < stats.rows; ++i) {

        if (stats.at<int>(i, 4) > max_s) {
            max_i = i;
            max_s = stats.at<int>(i, 4);

        }
    }

    auto x = stats.at<int>(max_i, 0), y = stats.at<int>(max_i, 1);
    auto width = stats.at<int>(max_i, 2), height = stats.at<int>(max_i, 3);

    cv::Rect rect = { x, y, width, height };
    return rect;
}

int lab4(const std::string filename) {
    std::string path = "../data/" + filename + ".mp4";
    cv::VideoCapture video(path);
    if (!video.isOpened()) {
        std::cerr << "Failed to open the video device, video file or image sequence!\n";
        return 1;
    }
    cv::Mat frame;
    auto frames = splitVideo(video);
    cv::Mat frame_quantized, frame_binar, frame_morph, finalimg;
    cv::Rect2i rectalg, rectest;
    for (int i = 0; i < 3; ++i) {
        frame = frames.at((int)frames.size()*(i+2)/5);
        frame_quantized = quantize_frame(frame);
        frame_binar = binarize_frame(frame);
        frame_morph = morph(frame_binar);
        rectalg = connected_components(frame_morph);
        rectest = markedRect.at((((filename[5]) - '0') - 1) * 3 + i);
        cv::cvtColor(frame_morph, finalimg, cv::COLOR_RGB2BGR);
        cv::rectangle(finalimg, rectalg, cv::Scalar(0, 0, 255), 4);
        cv::rectangle(frame, rectest, cv::Scalar(0, 255, 0), 4);
        cv::hconcat(frame, finalimg, finalimg);
        cv::imwrite(filename + "_" + std::to_string(i+2) + "_5.png", finalimg);
        /*std::cout << rectDiff(rectalg, rectest) << std::endl;*/
        /*std::cout << cv::norm(find_center(rectalg) - find_center(rectest)) << std::endl;*/
        std::cout << errorCheck(frame, rectalg, rectest) << std::endl;
    }
    /*cv::imwrite(filename + "quantized.png", frame_quantized);
    cv::imwrite(filename + "binar.png", frame_binar);
    cv::imwrite(filename + "morph.png", frame_morph);*/
}

int main() {
    lab4("lab4_1");
    lab4("lab4_2");
    lab4("lab4_3");
    lab4("lab4_4");
    lab4("lab4_5");
}
