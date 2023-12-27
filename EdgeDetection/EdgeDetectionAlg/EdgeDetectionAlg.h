#ifndef EDGEDETECTIONALG_H
#define EDGEDETECTIONALG_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

namespace EdgeDetectionAlg
{
	std::vector<cv::Mat> EdgeDetectionOnCPU(std::vector<std::string> fileNames);

	std::vector<cv::Mat> EdgeDetectionOnGPU(std::vector<std::string> fileNames);
};

#endif EDGEDETECTIONALG_H