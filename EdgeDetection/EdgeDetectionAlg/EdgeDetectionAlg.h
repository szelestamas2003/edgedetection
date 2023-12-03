#ifndef EDGEDETECTIONALG_H
#define EDGEDETECTIONALG_H

//#include <opencv2/core/core.hpp>
//#include <opencv2/imgcodecs.hpp>

namespace EdgeDetectionAlg
{
	void EdgeDetectionOnCPU();

	extern "C" void EdgeDetectionOnGPU();
};

#endif EDGEDETECTIONALG_H