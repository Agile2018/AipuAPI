#include "GraphicProcessor.h"

GraphicProcessor::GraphicProcessor()
{
}

GraphicProcessor::~GraphicProcessor()
{
}

bool GraphicProcessor::ThereIsGraphicProcessor() {
	description = format->FormatString("-----------------------------------------\n");
	description += format->FormatString("CUDA HARDWARE REPORT\n");

	int deviceCount = 0;
	cudaError_t error_id = cudaGetDeviceCount(&deviceCount);

	if (error_id != cudaSuccess) {

		description += format->FormatString("cudaGetDeviceCount returned %d\n-> %s\n",
			static_cast<int>(error_id), cudaGetErrorString(error_id));
		return false;
	}

	// This function call returns 0 if there are no CUDA capable devices.
	if (deviceCount == 0) {
		description += format->FormatString("There are no available device(s) that support CUDA\n");

	}
	else {
		description += format->FormatString("Detected %d CUDA Capable device(s)\n", deviceCount);

	}

	int dev, driverVersion = 0, runtimeVersion = 0;

	for (dev = 0; dev < deviceCount; ++dev) {
		cudaSetDevice(dev);
		cudaDeviceProp deviceProp;
		cudaGetDeviceProperties(&deviceProp, dev);

		description += format->FormatString("\nDevice %d: \"%s\"\n", dev, deviceProp.name);
		cudaDriverGetVersion(&driverVersion);
		cudaRuntimeGetVersion(&runtimeVersion);
		description += format->FormatString("CUDA Driver Version / Runtime Version %d.%d / %d.%d\n",
			driverVersion / 1000, (driverVersion % 100) / 10,
			runtimeVersion / 1000, (runtimeVersion % 100) / 10);

		description += format->FormatString("CUDA Capability Major/Minor version number: %d.%d\n",
			deviceProp.major, deviceProp.minor);

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		description += format->FormatString("Total amount of global memory: %.0f MBytes (%llu bytes)\n",
			static_cast<float>(deviceProp.totalGlobalMem / 1048576.0f),
			(unsigned long long)deviceProp.totalGlobalMem);

#else
		description += format->FormatString("Total amount of global memory: %.0f MBytes (%llu bytes)\n",
			static_cast<float>(deviceProp.totalGlobalMem / 1048576.0f),
			(unsigned long long)deviceProp.totalGlobalMem);

#endif
		description += format->FormatString("(%2d) Multiprocessors, (%3d) CUDA Cores/MP: %d CUDA Cores\n",
			deviceProp.multiProcessorCount,
			_ConvertSMVer2Cores(deviceProp.major, deviceProp.minor),
			_ConvertSMVer2Cores(deviceProp.major, deviceProp.minor) *
			deviceProp.multiProcessorCount);

		description += format->FormatString("GPU Max Clock rate: %.0f MHz (%0.2f GHz)\n",
			deviceProp.clockRate * 1e-3f, deviceProp.clockRate * 1e-6f);

#if CUDART_VERSION >= 5000
		// This is supported in CUDA 5.0 (runtime API device properties)
		description += format->FormatString("Memory Clock rate:  %.0f Mhz\n",
			deviceProp.memoryClockRate * 1e-3f);

		description += format->FormatString("Memory Bus Width: %d-bit\n",
			deviceProp.memoryBusWidth);

		if (deviceProp.l2CacheSize) {
			description += format->FormatString("L2 Cache Size: %d bytes\n",
				deviceProp.l2CacheSize);
		}

#else
		// This only available in CUDA 4.0-4.2 (but these were only exposed in the
		// CUDA Driver API)
		int memoryClock;
		getCudaAttribute<int>(&memoryClock, CU_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE,
			dev);
		description += format->FormatString("Memory Clock rate: %.0f Mhz\n",
			memoryClock * 1e-3f);
		int memBusWidth;
		getCudaAttribute<int>(&memBusWidth,
			CU_DEVICE_ATTRIBUTE_GLOBAL_MEMORY_BUS_WIDTH, dev);
		description += format->FormatString("Memory Bus Width: %d-bit\n",
			memBusWidth);
		int L2CacheSize;
		getCudaAttribute<int>(&L2CacheSize, CU_DEVICE_ATTRIBUTE_L2_CACHE_SIZE, dev);

		if (L2CacheSize) {
			description += format->FormatString("L2 Cache Size: %d bytes\n",
				L2CacheSize);
		}

#endif
		description += format->FormatString("Total amount of constant memory: %lu bytes\n",
			deviceProp.totalConstMem);
		description += format->FormatString("Total amount of shared memory per block: %lu bytes\n",
			deviceProp.sharedMemPerBlock);
		description += format->FormatString("Total number of registers available per block: %d\n",
			deviceProp.regsPerBlock);
		description += format->FormatString("Warp size: %d\n",
			deviceProp.warpSize);

		description += format->FormatString("Maximum number of threads per multiprocessor:  %d\n",
			deviceProp.maxThreadsPerMultiProcessor);

		description += format->FormatString("Maximum number of threads per block: %d\n",
			deviceProp.maxThreadsPerBlock);

		description += format->FormatString("Max dimension size of a thread block (x,y,z): (%d, %d, %d)\n",
			deviceProp.maxThreadsDim[0], deviceProp.maxThreadsDim[1],
			deviceProp.maxThreadsDim[2]);

		description += format->FormatString("Run time limit on kernels: %s\n",
			deviceProp.kernelExecTimeoutEnabled ? "Yes" : "No");

		description += format->FormatString("Integrated GPU sharing Host Memory: %s\n",
			deviceProp.integrated ? "Yes" : "No");

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)

		description += format->FormatString("CUDA Device Driver Mode (TCC or WDDM): %s\n",
			deviceProp.tccDriver ? "TCC (Tesla Compute Cluster Driver)"
			: "WDDM (Windows Display Driver Model)");

#endif
	}

	// If there are 2 or more GPUs, query to determine whether RDMA is supported
	if (deviceCount >= 2) {
		cudaDeviceProp prop[64];
		int gpuid[64];  // we want to find the first two GPUs that can support P2P
		int gpu_p2p_count = 0;

		for (int i = 0; i < deviceCount; i++) {
			checkCudaErrors(cudaGetDeviceProperties(&prop[i], i));

			// Only boards based on Fermi or later can support P2P
			if ((prop[i].major >= 2)
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
				// on Windows (64-bit), the Tesla Compute Cluster driver for windows
				// must be enabled to support this
				&& prop[i].tccDriver
#endif
				) {
				// This is an array of P2P capable GPUs
				gpuid[gpu_p2p_count++] = i;
			}
		}

		// Show all the combinations of support P2P GPUs
		int can_access_peer;

		if (gpu_p2p_count >= 2) {
			for (int i = 0; i < gpu_p2p_count; i++) {
				for (int j = 0; j < gpu_p2p_count; j++) {
					if (gpuid[i] == gpuid[j]) {
						continue;
					}
					checkCudaErrors(
						cudaDeviceCanAccessPeer(&can_access_peer, gpuid[i], gpuid[j]));
					description += format->FormatString("> Peer access from %s (GPU%d) -> %s (GPU%d) : %s\n",
						prop[gpuid[i]].name, gpuid[i], prop[gpuid[j]].name, gpuid[j],
						can_access_peer ? "Yes" : "No");
				}
			}
		}
	}

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	description += format->FormatString("deviceQuery, CUDA Driver = CUDART, CUDA Driver Version = %d.%d\n",
		driverVersion / 1000, (driverVersion % 100) / 10);


#else
	description += format->FormatString("deviceQuery, CUDA Driver = CUDART, CUDA Driver Version = %d.%d\n",
		driverVersion / 1000, (driverVersion % 100) / 10);
#endif
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	description += format->FormatString("CUDA Runtime Version = %d.%d\n",
		runtimeVersion / 1000, (runtimeVersion % 100) / 10);

#else
	description += format->FormatString("CUDA Runtime Version = %d.%d\n",
		runtimeVersion / 1000, (runtimeVersion % 100) / 10);

#endif
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	description += format->FormatString("NumDevs  = %d\n",
		deviceCount);

#else
	description += format->FormatString("NumDevs = %d\n",
		deviceCount);

#endif

	return true;

}