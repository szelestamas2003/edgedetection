#ifndef EDGEDETECTIONALG_H
#define EDGEDETECTIONALG_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

namespace EdgeDetectionAlg
{
	cv::Mat EdgeDetectionOnCPU(std::string fileName);

	std::vector<cv::Mat> EdgeDetectionOnGPU(std::vector<std::string> fileNames);
};

#endif EDGEDETECTIONALG_H