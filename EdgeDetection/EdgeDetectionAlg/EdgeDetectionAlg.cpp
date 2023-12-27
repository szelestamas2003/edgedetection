#include "EdgeDetectionAlg.h"
#include <cmath>
#include <opencv2/highgui/highgui.hpp>

namespace {
	float** CreatingGaussFilter(int size = 5, float omega = 1) {
		float** matrix = new float* [size];
		float k = (size - 1) / 2.0f;

		for (int i = 1; i <= size; i++) {
			matrix[i - 1] = new float[size];
			for (int j = 1; j <= size; j++) {
				matrix[i - 1][j - 1] = 1.0f / (2 * CV_PI * omega * omega) * exp(-1 * (pow(i - (k + 1), 2) + pow(j - (k + 1), 2)) / 2 * omega * omega);
			}
		}
		return matrix;
	}
}

cv::Mat EdgeDetectionAlg::EdgeDetectionOnCPU(std::string fileName)
{
	float** gaussFilter = CreatingGaussFilter(5, 1);
	cv::Mat image = cv::imread(fileName, cv::IMREAD_GRAYSCALE);

	cv::Mat result;
	cv::GaussianBlur(image, result, cv::Size(5, 5), 1.3, 1.3, cv::BorderTypes::BORDER_CONSTANT);
	cv::Mat dx, dy;
	cv::Sobel(result, dx, -1, 1, 0, 3);
	cv::Sobel(result, dy, -1, 0, 1, 3);
	cv::Mat dir(result.rows, result.cols, CV_8UC1);
	image = result;
	for (int i = 0; i < result.cols; i++) {
		for (int j = 0; j < result.rows; j++) {
			result.at<uchar>(j, i) = hypot(dx.at<uchar>(j, i), dy.at<uchar>(j, i));
			double dirP = atan2(dy.at<uchar>(j, i), dx.at<uchar>(j, i));
			if ((dirP >= 0 && dirP < 22.5) || (dirP > 157.5 && dirP <= 180))
				dir.at<uint8_t>(j, i) = 0;
			else if (dirP >= 22.5 && dirP < 67.5)
				dir.at<uint8_t>(j, i) = 45;
			else if (dirP >= 67.5 && dirP < 112.5)
				dir.at<uint8_t>(j, i) = 90;
			else
				dir.at<uint8_t>(j, i) = 135;
		}
	}

	for (int i = 0; i < result.cols; i++) {
		for (int j = 0; j < result.rows; j++) {
			switch (dir.at<uint8_t>(j, i))
			{
			case 0:
				if (i - 1 >= 0 && i + 1 < result.cols) {
					if (result.at<uchar>(j, i) < std::max(result.at<uchar>(j, i - 1), result.at<uchar>(j, i + 1)))
						result.at<uchar>(j, i) = 0;
				}
				else if (i - 1 < 0 && i + 1 < result.cols) {
					if (result.at<uchar>(j, i) < result.at<uchar>(j, i + 1))
						result.at<uchar>(j, i) = 0;
				}
				else if (i - 1 >= 0 && i + 1 >= result.cols) {
					if (result.at<uchar>(j, i) < result.at<uchar>(j, i - 1))
						result.at<uchar>(j, i) = 0;
				}
				break;
			case 90:
				if (j - 1 >= 0 && j + 1 < result.rows) {
					if (result.at<uchar>(j, i) < std::max(result.at<uchar>(j - 1, i), result.at<uchar>(j + 1, i)))
						result.at<uchar>(j, i) = 0;
				}
				else if (j - 1 < 0 && j + 1 < result.rows) {
					if (result.at<uchar>(j, i) < result.at<uchar>(j + 1, i))
						result.at<uchar>(j, i) = 0;
				}
				else if (j - 1 >= 0 && j + 1 >= result.rows) {
					if (result.at<uchar>(j, i) < result.at<uchar>(j - 1, i))
						result.at<uchar>(j, i) = 0;
				}
				break;
			case 45:
				if (i - 1 >= 0 && j + 1 < result.rows && i + 1 < result.cols && j - 1 >= 0) {
					if (result.at<uchar>(j, i) < std::max(result.at<uchar>(j + 1, i - 1), result.at<uchar>(j - 1, i + 1)))
						result.at<uchar>(j, i) = 0;
				}
				else if (i - 1 >= 0 && j + 1 < result.rows && (i + 1 > result.cols || j - 1 < 0)) {
					if (result.at<uchar>(j, i) < result.at<uchar>(j + 1, i - 1))
						result.at<uchar>(j, i) = 0;
				}
				else if ((i - 1 < 0 || j + 1 >= result.rows) && i + 1 < result.cols && j - 1 >= 0) {
					if (result.at<uchar>(j, i) < result.at<uchar>(j - 1, i + 1))
						result.at<uchar>(j, i) = 0;
				}
				break;
			case 135:
				if (i - 1 >= 0 && j - 1 >= 0 && i + 1 < result.cols && j + 1 < result.rows) {
					if (result.at<uchar>(j, i) < std::max(result.at<uchar>(j + 1, i + 1), result.at<uchar>(j - 1, i - 1)))
						result.at<uchar>(j, i) = 0;
				}
				else if (i - 1 >= 0 && j - 1 >= 0 && (i + 1 > result.cols || j + 1 > result.rows)) {
					if (result.at<uchar>(j, i) < result.at<uchar>(j - 1, i - 1))
						result.at<uchar>(j, i) = 0;
				}
				else if ((i - 1 < 0 || j - 1 < 0) && i + 1 < result.cols && j + 1 < result.rows) {
					if (result.at<uchar>(j, i) < result.at<uchar>(j + 1, i + 1))
						result.at<uchar>(j, i) = 0;
				}
				break;
			}
		}
	}

	free(gaussFilter);

	return result;
}

std::vector<cv::Mat> EdgeDetectionAlg::EdgeDetectionOnGPU(std::vector<std::string> fileNames)
{
	return std::vector<cv::Mat>();
}
