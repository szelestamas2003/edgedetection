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
			dir.at<uchar>(j, i) = static_cast<uchar>(std::clamp(dirP, 0.0, 255.0));
		}
	}

	for (int i = 1; i < result.cols - 1; i++) {
		for (int j = 1; j < result.rows - 1; j++) {
			double weight, magnitude1, magnitude2;
			uchar angle = dir.at<uchar>(j, i);
			if (angle >= 0 && angle < 45) {
				weight = std::tan(angle);
				magnitude1 = result.at<uchar>(j, i + 1) * (1 - weight) + result.at<uchar>(j - 1, i + 1) * weight;
				magnitude2 = result.at<uchar>(j, i - 1) * (1 - weight) + result.at<uchar>(j + 1, i - 1) * weight;
			} else if (angle >= 45 && angle < 90) {
				weight = std::tan(angle);
				magnitude1 = result.at<uchar>(j - 1, i + 1) * weight + result.at<uchar>(j - 1, i) * (1 - weight);
				magnitude2 = result.at<uchar>(j + 1, i - 1) * weight + result.at<uchar>(j + 1, i) * (1 - weight);
			} else if (angle >= 90 && angle < 135) {
				weight = std::tan(angle);
				magnitude1 = result.at<uchar>(j - 1, i - 1) * weight + result.at<uchar>(j - 1, i) * (1 - weight);
				magnitude2 = result.at<uchar>(j + 1, i + 1) * weight + result.at<uchar>(j + 1, i) * (1 - weight);
			} else {
				weight = std::tan(angle);
				magnitude1 = result.at<uchar>(j, i + 1) * (1 - weight) + result.at<uchar>(j + 1, i + 1) * weight;
				magnitude2 = result.at<uchar>(j, i - 1) * (1 - weight) + result.at<uchar>(j - 1, i - 1) * weight;
				
			}
			if (result.at<uchar>(j, i) < std::max(magnitude1, magnitude2))
				result.at<uchar>(j, i) = 0;
		}
	}

	float max = 0;
	uchar lowTreshold = 45, highTreshold = 130;

	for (int i = 0; i < result.cols; i++) {
		for (int j = 0; j < result.rows; j++) {
			if (result.at<uchar>(j, i) >= highTreshold)
				result.at<uchar>(j, i) = 255;
			else if (result.at<uchar>(j, i) >= lowTreshold)
				result.at<uchar>(j, i) = highTreshold;
			else
				result.at<uchar>(j, i) = 0;
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
