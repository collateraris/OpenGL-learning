#pragma once

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/cl.h>
#include <FreeImage/FreeImage.h>

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>

namespace opencl_1n2
{
	int lesson_main();

	cl_program CreateProgram(cl_context context, cl_device_id device, const std::string& fileName);

	cl_mem LoadImage(cl_context context, const std::string& fileName, int& width, int& height);

	int RoundUp(std::size_t groupSize, std::size_t globalSize);

	int lesson_main()
	{
		cl_platform_id platform_id;
		cl_uint ret_num_platforms;
		auto ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

		cl_device_id device_id;
		cl_uint ret_num_devices;
		ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);

		auto context = clCreateContext(nullptr, 1, &device_id, nullptr, nullptr, &ret);

		cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

		std::string kernel_path = "OpenCL/1n2_image_sobel_filter/kernels/sobel_filter.cl";
		cl_program program = CreateProgram(context, device_id, kernel_path);

		int width, height;
		std::string imagePath = "content/tex/awesomeface2.png";
		cl_mem image = LoadImage(context, imagePath, width, height);

		cl_kernel kernel = clCreateKernel(program, "test", &ret);

		cl_mem memobj = nullptr;
		int memLenth = 10;
		cl_int* mem = (cl_int*)malloc(sizeof(cl_int) * memLenth);

		memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, memLenth * sizeof(cl_int), nullptr, &ret);

		ret = clEnqueueWriteBuffer(command_queue, memobj, CL_TRUE, 0, memLenth * sizeof(cl_int), mem, 0, nullptr, nullptr);

		ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &image);
		ret |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &image);

		if (ret != CL_SUCCESS)
		{
			std::cerr << "Error setting kernel arguments." << std::endl;
			return 1;
		}

		size_t localWorkSize[2] = { 16, 16 };
		size_t globalWorkSize[2] = { RoundUp(localWorkSize[0], width),
		
			RoundUp(localWorkSize[1], height) };

		ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, nullptr, localWorkSize, localWorkSize, 0, nullptr, nullptr);

		ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0, memLenth * sizeof(float), mem, 0, nullptr, nullptr);


		return 0;
	}

	cl_program CreateProgram(cl_context context, cl_device_id device, const std::string& fileName)
	{
		cl_int errNum;
		cl_program program;
		std::ifstream kernelFile(fileName, std::ios::in);
		if (!kernelFile.is_open())
		{
			std::cerr << "Failed to open file for reading: " << fileName << std::endl;
			return nullptr;
		}

		std::ostringstream oss;
		oss << kernelFile.rdbuf();
		std::string srcStdStr = oss.str();
		const char* srcStr = srcStdStr.c_str();
		program = clCreateProgramWithSource(context, 1, (const char**)&srcStr, nullptr, nullptr);
		if (program == nullptr)
		{
			std::cerr << "Failed to create CL program from source." << std::endl;
			return nullptr;
		}

		errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
		if (errNum != CL_SUCCESS)
		{
			// Determine the reason for the error
			char buildLog[16384];
			clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
				sizeof(buildLog), buildLog, NULL);
			std::cerr << "Error in kernel: " << std::endl;
			std::cerr << buildLog;
			clReleaseProgram(program);
			return nullptr;
		}

		return program;
	}

	cl_mem LoadImage(cl_context context, const std::string& filePath, int& width, int& height)
	{
		const char* fileName = filePath.c_str();
		FREE_IMAGE_FORMAT format = FreeImage_GetFileType(fileName, 0);
		FIBITMAP* image = FreeImage_Load(format, fileName);

		FIBITMAP* temp = image;
		image = FreeImage_ConvertTo32Bits(image);
		FreeImage_Unload(temp);

		width = FreeImage_GetWidth(image);
		height = FreeImage_GetHeight(image);
		char* buffer = new char[width * height * 4];
		memcpy(buffer, FreeImage_GetBits(image), width * height * 4);
		FreeImage_Unload(image);

		cl_image_format clImageFormat;
		clImageFormat.image_channel_order = CL_RGBA;
		clImageFormat.image_channel_data_type = CL_UNORM_INT8;

		cl_image_desc clImageDesc;
		clImageDesc.image_type = CL_MEM_OBJECT_IMAGE2D;
		clImageDesc.image_width = width;
		clImageDesc.image_height = height;
		clImageDesc.image_depth = 0;
		clImageDesc.image_array_size = 0;
		clImageDesc.image_row_pitch = 0;
		clImageDesc.image_slice_pitch = 0;
		clImageDesc.num_mip_levels = 0;
		clImageDesc.num_samples = 0;
		clImageDesc.buffer = nullptr;

		cl_int errNum;
		cl_mem clImage;
		clImage = clCreateImage(context,
			CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			&clImageFormat,
			&clImageDesc,
			buffer,
			&errNum);

		if (errNum != CL_SUCCESS)
		{
			std::cerr << "Error creating CL image object" << std::endl;
			return 0;
		}

		return clImage;
	}

	int RoundUp(std::size_t groupSize, std::size_t globalSize)
	{
		int r;
		if ((r = globalSize % groupSize) == 0)
		{
			return globalSize;
		}
		return globalSize + groupSize - r;
	}
}