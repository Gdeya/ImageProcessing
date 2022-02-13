#include <opencv2/opencv.hpp>
#include <vector>

cv::Mat createMozaic(cv::Mat img) {
    std::vector<cv::Mat> rgbchannels(3);
    cv::split(img, rgbchannels);
    cv::Mat g, rimg, gimg, bimg;
    g = cv::Mat::zeros(cv::Size(img.cols, img.rows), CV_8UC1);
    std::vector<cv::Mat> rchannel(3), gchannel(3), bchannel(3);

    rchannel = { g, g, rgbchannels[2] };

    gchannel = { g, rgbchannels[1], g };

    bchannel = { rgbchannels[0], g, g };

    cv::merge(rchannel, rimg);
    cv::merge(gchannel, gimg);
    cv::merge(bchannel, bimg);

    cv::Mat Mozaic;
    cv::Mat horisontal1, horisontal2;
    cv::hconcat(img, rimg, horisontal1);
    cv::hconcat(gimg, bimg, horisontal2);
    cv::vconcat(horisontal1, horisontal2, Mozaic);
    return Mozaic;
}
cv::Mat createHist(cv::Mat src) {
    std::vector<cv::Mat> bgr_planes;
    split(src, bgr_planes);
    int histSize = 256;
    float range[] = { 0, 256 }; //the upper boundary is exclusive
    const float* histRange[] = { range };
    bool uniform = true, accumulate = false;
    cv::Mat b_hist, g_hist, r_hist;
    calcHist(&bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, histRange, uniform, accumulate);
    calcHist(&bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, histRange, uniform, accumulate);
    calcHist(&bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, histRange, uniform, accumulate);
    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));
    normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
    normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
    normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
    for (int i = 1; i < histSize; i++)
    {
        line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
            cv::Scalar(255, 0, 0), 2, 8, 0);
        line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
            cv::Scalar(0, 255, 0), 2, 8, 0);
        line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
            cv::Scalar(0, 0, 255), 2, 8, 0);
    }
    return histImage;
}

int main() {
	std::string path = cv::samples::findFile("cross_0256x0256.png");
    std::string path1 = cv::samples::findFile("cross_0256x0256_025.jpeg");
	cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);
    cv::Mat imgjpeg = cv::imread(path1, cv::IMREAD_COLOR);
    if (img.empty())
    {
        std::cout << "Could not read the image: " << path << std::endl;
        return 1;
    }
    if (imgjpeg.empty())
    {
        std::cout << "Could not read the image: " << path1 << std::endl;
        return 1;
    }
    std::vector<int> p(2);
    p[0] = cv::ImwriteFlags::IMWRITE_JPEG_QUALITY;
    p[1] = 25;

    cv::imwrite("cross_0256x0256_025.jpeg", img, p);
    cv::Mat hist;
    cv::hconcat(createHist(img), createHist(imgjpeg), hist);
    cv::imwrite("cross_0256x0256_png_channels.png", createMozaic(img));
    cv::imwrite("cross_0256x0256_jpg_channels.png", createMozaic(imgjpeg));
    cv::imwrite("cross_0256x0256_hists.png", hist);
    return 0;
}