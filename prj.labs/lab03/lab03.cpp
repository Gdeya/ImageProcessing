#include <opencv2/opencv.hpp>
#include <vector>

double brightnessTransform(int x) {
    return x*3*log(x);
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

    cv::Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    for (int i = 0; i < 256; ++i)
        p[i] = brightnessTransform(i);

    cv::Mat img_grey_tr;
    cv::LUT(img_grey, lookUpTable, img_grey_tr);
    cv::imwrite("lab03_gre_res.png", img_grey_tr);

    cv::Mat img_tr;
    cv::LUT(img, lookUpTable, img_tr);
    cv::imwrite("lab03_rgb_res.png", img_tr);

    int func_hist_w = 512, func_hist_h = 512;
    cv::Mat func_hist(func_hist_w, func_hist_h, CV_8UC1, cv::Scalar(255, 255, 255));
    for (int i = 0; i < 256; ++i) {
        cv::line(
            func_hist,
            cv::Point((i - 1) * 2, func_hist_h - cvRound(p[i - 1]) * 2),
            cv::Point((i) * 2, func_hist_h - cvRound(p[i]) * 2),
            cv::Scalar(0, 0, 0),
            1,
            0
        );
    }
    cv::imwrite("lab03_viz_func.png", func_hist);

    return 0;
};