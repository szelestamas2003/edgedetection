#include "EdgeDetectionAlg.h"
#include <cmath>
#include <cuda_runtime.h>
#include "cudaHeader.h"

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

cv::Mat EdgeDetectionAlg::EdgeDetectionOnCPU(const std::string &fileName)
{
	cv::Mat image;
	image = cv::imread(fileName, cv::IMREAD_GRAYSCALE);

	cv::Mat result;
	cv::GaussianBlur(image, result, cv::Size(5, 5), 1.3, 1.3, cv::BorderTypes::BORDER_CONSTANT);
	cv::Mat dx, dy;
	cv::Sobel(result, dx, -1, 1, 0, 3);
	cv::Sobel(result, dy, -1, 0, 1, 3);
	cv::Mat dir(result.rows, result.cols, CV_8UC1);
	image = result;
	for (int i = 0; i < result.cols; i++) {
		for (int j = 0; j < result.rows; j++) {
			result.at<double>(j, i) = hypot(dx.at<double>(j, i), dy.at<double>(j, i));
			double dirP = atan2(dy.at<double>(j, i), dx.at<double>(j, i));
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
				if (i - 1 >= 0 && i + 1 < result.cols)
					if (result.at<double>(j, i) < std::max(result.at<double>(j, i - 1), result.at<double>(j, i + 1)))
						result.at<double>(j, i) = 0;
				else if (i - 1 < 0 && i + 1 < result.cols)
					if (result.at<double>(j, i) < result.at<double>(j, i + 1))
						result.at<double>(j, i) = 0;
				else if (i - 1 >= 0 && i + 1 >= result.cols)
					if (result.at<double>(j, i) < result.at<double>(j, i - 1))
						result.at<double>(j, i) = 0;
				break;
			case 90:
				if (j - 1 >= 0 && j + 1 < result.rows)
					if (result.at<double>(j, i) < std::max(result.at<double>(j - 1, i), result.at<double>(j + 1, i)))
						result.at<double>(j, i) = 0;
				else if (j - 1 < 0 && j + 1 < result.rows)
					if (result.at<double>(j, i) < result.at<double>(j + 1, i))
						result.at<double>(j, i) = 0;
				else if (j - 1 >= 0 && j + 1 >= result.rows)
					if (result.at<double>(j, i) < result.at<double>(j - 1, i))
						result.at<double>(j, i) = 0;
				break;
			case 45:
				if (i - 1 >= 0 && j + 1 < result.rows && i + 1 < result.cols && j - 1 >= 0)
					if (result.at<double>(j, i) < std::max(result.at<double>(j + 1, i - 1), result.at<double>(j - 1, i + 1)))
						result.at<double>(j, i) = 0;
				else if (i - 1 >= 0 && j + 1 < result.rows && (i + 1 > result.cols || j - 1 < 0))
					if (result.at<double>(j, i) < result.at<double>(j + 1, i - 1))
						result.at<double>(j, i) = 0;
				else if ((i - 1 < 0 || j + 1 >= result.rows) && i + 1 < result.cols && j - 1 >= 0)
					if (result.at<double>(j, i) < result.at<double>(j - 1, i + 1))
						result.at<double>(j, i) = 0;
				break;
			case 135:
				if (i - 1 >= 0 && j - 1 >= 0 && i + 1 < result.cols && j + 1 < result.rows)
					if (result.at<double>(j, i) < std::max(result.at<double>(j + 1, i + 1), result.at<double>(j - 1, i - 1)))
						result.at<double>(j, i) = 0;
				else if (i - 1 >= 0 && j - 1 >= 0 && (i + 1 > result.cols || j + 1 > result.rows))
					if (result.at<double>(j, i) < result.at<double>(j - 1, i - 1))
						result.at<double>(j, i) = 0;
				else if ((i - 1 < 0 || j - 1 < 0) && i + 1 < result.cols && j + 1 < result.rows)
					if (result.at<double>(j, i) < result.at<double>(j + 1, i + 1))
						result.at<double>(j, i) = 0;
				break;
			}
		}
	}
	image = result;

	return image;
}

cv::Mat EdgeDetectionAlg::EdgeDetectionOnGPU(const std::string &fileName)
{
	cv::Mat image;
	image = cv::imread(fileName, cv::IMREAD_GRAYSCALE);

	int width = image.cols;
	int height = image.rows;

	unsigned char* pixels = new unsigned char[width * height];
	unsigned char* resultPixels = new unsigned char[width * height];

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			uchar pixel = image.at<uchar>(y, x);

			pixels[width * y + x] = pixel;
		}
	}

	resultPixels = launchKernels(pixels, width, height);

	
	return image;
}
