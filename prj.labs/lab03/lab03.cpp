#include <opencv2/opencv.hpp>
#include <vector>

double brightnessTransform(double x) {
    return x*sin(x);
}

int main() {
    std::string path = cv::samples::findFile("cross_0256x0256.png");
    cv::Mat img = cv::imread(path);
    cv::Mat img_grey = cv::imread(path, cv::IMREAD_GRAYSCALE);
    if (img.empty())
    {
        std::cout << "Could not read the image: " << path << std::endl;
        return 1;
    }
    cv::imwrite("lab03_rgb.png", img);
    cv::imwrite("lab03_gre.png", img_grey);

    /*cv::Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();*/
    /*for (int i = 0; i < 256; ++i)
        p[i] = brightnessTransform(i);*/
    std::vector<int> lookUpTable(256);
    for (int i = 0; i < 256; i += 1) {
        lookUpTable.at(i) = (int)(brightnessTransform(i / 255.0) * 255);
    }


    cv::Mat img_grey_tr;
    cv::LUT(img_grey, lookUpTable, img_grey_tr);
    cv::imwrite("lab03_gre_res.png", img_grey_tr);

    cv::Mat img_tr;
    cv::LUT(img, lookUpTable, img_tr);
    cv::imwrite("lab03_rgb_res.png", img_tr);

    int func_hist_w = 512, func_hist_h = 512;
    cv::Mat func_hist(func_hist_w, func_hist_h, CV_8UC1, cv::Scalar(255, 255, 255));
    cv::line(func_hist, cv::Point(0, 0), cv::Point(0, 512), cv::Scalar(0, 0, 0), 2, 0);
    cv::line(func_hist, cv::Point(0, 512), cv::Point(512, 512), cv::Scalar(0, 0, 0), 2, 0);
    for (int x = 0; x < func_hist.cols; x += 1) {
        int y = 511 - brightnessTransform(x / 511.0) * 511;
        func_hist.at<uchar>(y, x) = 0;
    }

    cv::imwrite("lab03_viz_func.png", func_hist);

    return 0;
};