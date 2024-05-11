#include "EdgeDetectionAlg.h"
#include <cmath>
#include <opencv2/videoio.hpp>
#include <cuda_runtime.h>
#include "cudaHeader.h"
#include <cstdio>
#include "gifenc.h"

cv::Mat EdgeDetectionAlg::EdgeDetectionOnCPU(cv::Mat& image)
{
	cv::Mat result;
	cv::GaussianBlur(image, result, cv::Size(5, 5), 1.3, 1.3, cv::BorderTypes::BORDER_CONSTANT);
	cv::Mat dx, dy;
	cv::Sobel(result, dx, -1, 1, 0, 3);
	cv::Sobel(result, dy, -1, 0, 1, 3);
	cv::Mat dir(result.rows, result.cols, CV_8UC1);
	image = result;
	for (int i = 0; i < result.cols; i++) {
		for (int j = 0; j < result.rows; j++) {
			result.at<uchar>(j, i) = static_cast<uchar>(std::clamp(hypot(static_cast<double>(dx.at<uchar>(j, i)), static_cast<double>(dy.at<uchar>(j, i))), 0.0, 255.0));
			double dirP = atan2(static_cast<double>(dy.at<uchar>(j, i)), static_cast<double>(dx.at<uchar>(j, i)));
			if ((dirP >= 0 && dirP < 22.5) || (dirP > 157.5 && dirP <= 180))
				dir.at<uchar>(j, i) = 0;
			else if (dirP >= 22.5 && dirP < 67.5)
				dir.at<uchar>(j, i) = 45;
			else if (dirP >= 67.5 && dirP < 112.5)
				dir.at<uchar>(j, i) = 90;
			else
				dir.at<uchar>(j, i) = 135;
		}
	}

	for (int i = 0; i < result.cols; i++) {
		for (int j = 0; j < result.rows; j++) {
			switch (dir.at<uchar>(j, i))
			{
			case 0:
				if (i - 1 >= 0 && i + 1 < result.cols)
					if (result.at<uchar>(j, i) < std::max(result.at<uchar>(j, i - 1), result.at<uchar>(j, i + 1)))
						result.at<uchar>(j, i) = 0;
					else if (i - 1 < 0 && i + 1 < result.cols)
						if (result.at<uchar>(j, i) < result.at<uchar>(j, i + 1))
							result.at<uchar>(j, i) = 0;
						else if (i - 1 >= 0 && i + 1 >= result.cols)
							if (result.at<uchar>(j, i) < result.at<uchar>(j, i - 1))
								result.at<uchar>(j, i) = 0;
				break;
			case 90:
				if (j - 1 >= 0 && j + 1 < result.rows)
					if (result.at<uchar>(j, i) < std::max(result.at<uchar>(j - 1, i), result.at<uchar>(j + 1, i)))
						result.at<uchar>(j, i) = 0;
					else if (j - 1 < 0 && j + 1 < result.rows)
						if (result.at<uchar>(j, i) < result.at<uchar>(j + 1, i))
							result.at<uchar>(j, i) = 0;
						else if (j - 1 >= 0 && j + 1 >= result.rows)
							if (result.at<uchar>(j, i) < result.at<uchar>(j - 1, i))
								result.at<uchar>(j, i) = 0;
				break;
			case 45:
				if (i - 1 >= 0 && j + 1 < result.rows && i + 1 < result.cols && j - 1 >= 0)
					if (result.at<uchar>(j, i) < std::max(result.at<uchar>(j + 1, i - 1), result.at<uchar>(j - 1, i + 1)))
						result.at<uchar>(j, i) = 0;
					else if (i - 1 >= 0 && j + 1 < result.rows && (i + 1 > result.cols || j - 1 < 0))
						if (result.at<uchar>(j, i) < result.at<uchar>(j + 1, i - 1))
							result.at<uchar>(j, i) = 0;
						else if ((i - 1 < 0 || j + 1 >= result.rows) && i + 1 < result.cols && j - 1 >= 0)
							if (result.at<uchar>(j, i) < result.at<uchar>(j - 1, i + 1))
								result.at<uchar>(j, i) = 0;
				break;
			case 135:
				if (i - 1 >= 0 && j - 1 >= 0 && i + 1 < result.cols && j + 1 < result.rows)
					if (result.at<uchar>(j, i) < std::max(result.at<uchar>(j + 1, i + 1), result.at<uchar>(j - 1, i - 1)))
						result.at<uchar>(j, i) = 0;
					else if (i - 1 >= 0 && j - 1 >= 0 && (i + 1 > result.cols || j + 1 > result.rows))
						if (result.at<uchar>(j, i) < result.at<uchar>(j - 1, i - 1))
							result.at<uchar>(j, i) = 0;
						else if ((i - 1 < 0 || j - 1 < 0) && i + 1 < result.cols && j + 1 < result.rows)
							if (result.at<uchar>(j, i) < result.at<uchar>(j + 1, i + 1))
								result.at<uchar>(j, i) = 0;
				break;
			}
		}
	}
	return result;
}

void EdgeDetectionAlg::RunEdgeDetectionOnCPU(const std::string &sourcefileName, const std::string &dstfileName)
{
	bool gif = false;
	int fps = 0, height = 0, width = 0;
	std::vector<cv::Mat> frames = ReadImage(sourcefileName, gif, height, width, fps);
	std::vector<cv::Mat> results;
	for (cv::Mat image : frames) {
		results.push_back(EdgeDetectionOnCPU(image));
	}
	WriteImage(dstfileName, results, gif, height, width, fps);
}

cv::Mat EdgeDetectionAlg::EdgeDetectionOnGPU(cv::Mat& image)
{
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

	return cv::Mat();
}

void EdgeDetectionAlg::RunEdgeDetectionOnGPU(const std::string &sourcefileName, const std::string &dstfileName)
{
	bool gif = false;
	int fps = 0, width = 0, height = 0;
	std::vector<cv::Mat> frames = ReadImage(sourcefileName, gif, height, width, fps);
	std::vector<cv::Mat> results;

	for (cv::Mat image : frames) {
		results.push_back(EdgeDetectionOnGPU(image));
	}
	WriteImage(dstfileName, results, gif, height, width, fps);
}

std::vector<cv::Mat> EdgeDetectionAlg::ReadImage(const std::string& fileName, bool &gif, int &height, int &width, int &fps)
{
	std::vector<cv::Mat> frames;
	if (fileName.substr(fileName.rfind(".") + 1) == "gif") {
		cv::VideoCapture capture(fileName);
		height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
		width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
		fps = capture.get(cv::CAP_PROP_FPS);
		cv::Mat frame;
		gif = true;
		while (capture.read(frame)) {
			cv::Mat gray;
			cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
			frames.push_back(gray);
		}
		capture.release();
	}
	else
		frames.push_back(cv::imread(fileName, cv::IMREAD_GRAYSCALE));
	return frames;
}

void EdgeDetectionAlg::WriteImage(const std::string& dstfileName, const std::vector<cv::Mat>& frames, const bool& gif, const int& height, const int& width, const int& fps)
{
	if (gif) {
		ge_GIF* new_gif = ge_new_gif(dstfileName.c_str(), width, height, palette, 8, -1, 0);

		for (cv::Mat frame : frames) {
			if (frame.isContinuous())
				new_gif->frame = frame.data;
			ge_add_frame(new_gif, 100 / fps);
		}
		ge_close_gif(new_gif);
	}
	else
		cv::imwrite(dstfileName, frames[0]);
}
