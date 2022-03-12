#include <opencv2/opencv.hpp>
#include <iostream>

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
    std::string path = cv::samples::findFile(filename + ".mp4");
    cv::VideoCapture video(path);
    if (!video.isOpened()) {
        std::cerr << "Failed to open the video device, video file or image sequence!\n";
        return 1;
    }
    cv::Mat frame;
    auto frames = splitVideo(video);
    cv::Mat frame_quantized, frame_binar, frame_morph, finalimg;
    cv::Rect2i rect;
    for (int i = 0; i < 3; ++i) {
        frame = frames.at((int)frames.size()*(i+2)/5);
        frame_quantized = quantize_frame(frame);
        frame_binar = binarize_frame(frame);
        frame_morph = morph(frame_binar);
        rect = connected_components(frame_morph);
        cv::cvtColor(frame_morph, finalimg, cv::COLOR_RGB2BGR);
        cv::rectangle(finalimg, rect, cv::Scalar(0, 0, 255), 4);
        cv::hconcat(frame, finalimg, finalimg);
        cv::imwrite(filename + "_" + std::to_string(i+2) + "_5.png", finalimg);
    }
    //cv::imwrite(filename + "quantized.png", frame_quantized);
    //cv::imwrite(filename + "binar.png", frame_binar);
    //cv::imwrite(filename + "morph.png", frame_morph);
}

int main() {
    lab4("lab4_1");
    lab4("lab4_2");
    lab4("lab4_3");
    lab4("lab4_4");
    lab4("lab4_5");
}
