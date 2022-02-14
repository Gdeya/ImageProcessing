#include <opencv2/opencv.hpp>
#include <time.h>
#include <iostream>

int main() {
  cv::Mat I_1(60, 768, CV_8UC1);
  // draw dummy image
  I_1 = 0;
  for (auto i = 0; i < I_1.cols; i++) {
	  for (auto j = 0; j < I_1.rows; j++) {
		  I_1.at<uchar>(j, i) += i/3;
	  }
  }
  // Конвертим для pow
  cv::Mat I_1CONV;
  I_1.convertTo(I_1CONV, CV_32FC1, (1.0/255));
  // Создаем результат
  cv::Mat G_1;
  clock_t start = clock();
  cv::pow(I_1CONV, 2.2, G_1);
  clock_t end = clock();
  double seconds = (double)(end - start) / CLOCKS_PER_SEC;
  //
  cv::Mat G_2;
  I_1.convertTo(G_2, CV_32FC1, 1.0/255);
  start = clock();
  for (auto i = 0; i < G_2.cols; i++) {
	  for (auto j = 0; j < G_2.rows; j++) {
		  G_2.at<float>(j, i) = pow(G_2.at<float>(j, i), 2.2);
	  }
  }
  end = clock();
  double seconds1 = (double)(end - start) / CLOCKS_PER_SEC;
  cv::Mat G_3(180, 768, CV_8UC1);
  G_1.convertTo(G_1, CV_8UC1, 255);
  G_2.convertTo(G_2, CV_8UC1, 255);
  cv::vconcat(I_1, G_1, G_3);;
  cv::vconcat(G_3, G_2, G_3);
  cv::imwrite("lab01_1.png", I_1);
  cv::imwrite("lab01_2.png", G_1);
  cv::imwrite("lab01_3.png", G_2);
  cv::imwrite("lab01_4.png", G_3);
  std::cout << "Time for pow :" << seconds << std::endl;
  std::cout << "Time for direct address :" << seconds1 << std::endl;
}
