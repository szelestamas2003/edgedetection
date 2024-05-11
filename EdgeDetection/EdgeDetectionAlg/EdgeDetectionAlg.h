#ifndef EDGEDETECTIONALG_H
#define EDGEDETECTIONALG_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

class EdgeDetectionAlg
{
public:
	EdgeDetectionAlg() {
		int j = 0;
		for (int i = 0; i < 256; i++) {
			palette[j++] = i;
			palette[j++] = i;
			palette[j++] = i;
		}
	}

	cv::Mat EdgeDetectionOnCPU(cv::Mat &image);

	void RunEdgeDetectionOnCPU(const std::string& sourcefileName, const std::string& dstfileName);

	cv::Mat EdgeDetectionOnGPU(cv::Mat& image);

	void RunEdgeDetectionOnGPU(const std::string& sourcefileName, const std::string& dstfileName);
private:
	uint8_t palette[256 * 3];

	std::vector<cv::Mat> ReadImage(const std::string& fileName, bool &gif, int &height, int &width, int& fps);

	void WriteImage(const std::string& dstfileName, const std::vector<cv::Mat> &frames, const bool& gif, const int& height, const int& width, const int& fps);
};

#endif EDGEDETECTIONALG_H