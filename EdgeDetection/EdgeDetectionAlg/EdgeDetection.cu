#define _USE_MATH_DEFINES
#include <cuda_runtime.h>
#include <math.h>
#include "cudaHeader.h"
#include "device_launch_parameters.h"

__global__ void gaussianKernel(float* kernel, float sigma, int size) {
	int i = blockIdx.x;
	int j = threadIdx.x;
	float k = (size - 1) / 2;

	int idx = i * blockDim.x + j;
	kernel[idx] = 1.0f / (2 * M_PI * pow(sigma, 2)) * exp(-1 * (pow(i - (k + 1), 2) + pow(j + (k + 1), 2)) / 2 * pow(sigma, 2));
}

__global__ void edgedetection(unsigned char* src, unsigned char* dst, int width, int height, float* kernel) {
	int currY = blockIdx.y * blockDim.y + threadIdx.y;
	int currX = blockIdx.x * blockDim.x + threadIdx.x;


}


unsigned char* launchKernels(unsigned char* pixels, int width, int height) {

	unsigned char* resultPixels = new unsigned char[width * height];
	int kernelSize = 5;
	float sigma = 1.0;

	float* GaussianKernel;
	size_t cudaKernelSize = kernelSize * kernelSize * sizeof(float);

	cudaMalloc((void**)&GaussianKernel, cudaKernelSize);

	gaussianKernel << <kernelSize, kernelSize >> > (GaussianKernel, sigma, kernelSize);
	cudaDeviceSynchronize();

	unsigned char* cudaSrcPixels;
	unsigned char* cudaDstPixels;

	size_t size = height * width * sizeof(unsigned char);

	cudaMalloc((void**)&cudaSrcPixels, size);
	cudaMalloc((void**)&cudaDstPixels, size);
	cudaMemcpy(cudaSrcPixels, pixels, size, cudaMemcpyHostToDevice);

	dim3 block_size(32, 32);
	dim3 grid_size((width + block_size.x - 1) / block_size.x, (height + block_size.y - 1) / block_size.y);

	edgedetection << <block_size, grid_size >> > (cudaSrcPixels, cudaDstPixels, width, height, GaussianKernel);
	cudaDeviceSynchronize();

	cudaMemcpy(resultPixels, cudaDstPixels, size, cudaMemcpyDeviceToHost);

	cudaFree(cudaSrcPixels);
	cudaFree(cudaDstPixels);
	cudaFree(GaussianKernel);

	return resultPixels;
}