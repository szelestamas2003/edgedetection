#ifndef EDGEDETECTIONALG_H
#define EDGEDETECTIONALG_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

namespace EdgeDetectionAlg
{
	cv::Mat EdgeDetectionOnCPU(const std::string &fileName);

	cv::Mat EdgeDetectionOnGPU(const std::string &fileName);
};

#endif EDGEDETECTIONALG_H