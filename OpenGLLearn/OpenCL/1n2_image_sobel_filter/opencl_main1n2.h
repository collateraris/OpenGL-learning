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

	cl_context CreateContext();

	cl_command_queue CreateCommandQueue(cl_context context, cl_device_id* device);

	cl_program CreateProgram(cl_context context, cl_device_id device, const std::string& fileName);

	cl_mem LoadImage(cl_context context, const std::string& fileName, int& width, int& height);

	bool SaveImage(const std::string& fileName, char* buffer, int width, int height);

	void Cleanup(cl_context& context, cl_command_queue& commandQueue, cl_program& program, cl_kernel& kernel, cl_mem image[2]);

	int RoundUp(std::size_t groupSize, std::size_t globalSize);

	int lesson_main()
	{
		cl_context context = 0;
		cl_command_queue commandQueue = 0;
		cl_program program = 0;
		cl_device_id device = 0;
		cl_kernel kernel = 0;
		cl_mem images[2] = {0 , 0};
		cl_int errNum;

		context = CreateContext();
		if (context == nullptr)
		{
			std::cerr << "Failed to create OpenCL context." << std::endl;
			return 1;
		}

		commandQueue = CreateCommandQueue(context, &device);
		if (commandQueue == nullptr)
		{
			Cleanup(context, commandQueue, program, kernel, images);
			return 1;
		}

		program = CreateProgram(context, device, "OpenCL/1n2_image_sobel_filter/kernels/sobel_filter.cl");
		if (program == nullptr)
		{
			Cleanup(context, commandQueue, program, kernel, images);
			return 1;
		}

		kernel = clCreateKernel(program, "sobel_rgb", nullptr);
		if (kernel == nullptr)
		{
			std::cerr << "Failed to create kernel" << std::endl;
			Cleanup(context, commandQueue, program, kernel, images);
			return 1;
		}

		int width, height;
		std::string imagePath = "content/tex/awesomeface2.png";
		images[0] = LoadImage(context, imagePath, width, height);

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

		images[1] = clCreateImage(context,
			CL_MEM_WRITE_ONLY,
			&clImageFormat,
			&clImageDesc,
			nullptr,
			&errNum);

		if (errNum != CL_SUCCESS)
		{
			std::cerr << "Error creating CL image object" << std::endl;
			return 0;
		}

		errNum = clSetKernelArg(kernel, 0, sizeof(cl_mem), &images[0]);
		errNum |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &images[1]);

		if (errNum != CL_SUCCESS)
		{
			std::cerr << "Error setting kernel arguments." << std::endl;
			return 1;
		}

		size_t localWorkSize[2] = { 16, 16 };
		size_t globalWorkSize[2] = { RoundUp(localWorkSize[0], width), RoundUp(localWorkSize[1], height) };

		errNum = clEnqueueNDRangeKernel(commandQueue, kernel, 2, nullptr,
			globalWorkSize, localWorkSize,
			0, nullptr, nullptr);

		if (errNum != CL_SUCCESS)
		{
			std::cerr << "Error queuing kernel for execution." << std::endl;
			Cleanup(context, commandQueue, program, kernel, images);
			return 1;
		}

		char* buffer = new char[width * height * 4];
		size_t origin[3] = { 0, 0, 0 };
		size_t region[3] = { width, height, 1 };
		errNum = clEnqueueReadImage(commandQueue, images[1],
			CL_TRUE,
			origin, region, 0, 0, buffer,
			0, nullptr, nullptr);

		if (errNum != CL_SUCCESS)
		{
			std::cerr << "Error reading result buffer."
				<< std::endl;
			Cleanup(context, commandQueue, program, kernel, images);
			return 1;
		}

		if (!SaveImage("awersomeSobel.png", buffer, width, height))
		{
			std::cerr << "Error saving image."<< std::endl;
			Cleanup(context, commandQueue, program, kernel, images);
			return 1;
		}
		Cleanup(context, commandQueue, program, kernel, images);

		return 0;
	}

	cl_context CreateContext()
	{
		cl_int errNum;
		cl_uint numPlatforms;
		cl_platform_id firstPlatformId;
		cl_context context = nullptr;

		errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
		if (errNum != CL_SUCCESS || numPlatforms <= 0)
		{
			std::cerr << "Failed to find any OpenCL platforms." << std::endl;
			return nullptr;
		}

		cl_context_properties contextProperties[] =
		{
			CL_CONTEXT_PLATFORM,
			(cl_context_properties)firstPlatformId,
			0
		};

		context = clCreateContextFromType(contextProperties,
			CL_DEVICE_TYPE_GPU,
			nullptr, nullptr, &errNum);

		if (errNum != CL_SUCCESS)
		{
			std::cout << "Could not create GPU context, trying CPU..." << std::endl;
			context = clCreateContextFromType(contextProperties,
				CL_DEVICE_TYPE_CPU,
				nullptr, nullptr, &errNum);
			if (errNum != CL_SUCCESS)
			{
				std::cerr <<
					"Failed to create an OpenCL GPU or CPU context.";
				return nullptr;
			}
		}
		return context;
	}

	cl_command_queue CreateCommandQueue(cl_context context, cl_device_id* device)
	{
		cl_int errNum;
		cl_device_id* devices;
		cl_command_queue commandQueue = nullptr;
		size_t deviceBufferSize = -1;

		errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, nullptr,
			&deviceBufferSize);
		if (errNum != CL_SUCCESS)
		{
			std::cerr << "Failed call to clGetContextInfo(..., GL_CONTEXT_DEVICES, ...)";
			return nullptr;
		}

		if (deviceBufferSize <= 0)
		{
			std::cerr << "No devices available.";
			return nullptr;
		}

		devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];

		errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES,
			deviceBufferSize, devices, nullptr);
		if (errNum != CL_SUCCESS)
		{
			std::cerr << "Failed to get device IDs";
			return nullptr;
		}

		commandQueue = clCreateCommandQueue(context,
			devices[0], 0, nullptr);
		if (commandQueue == nullptr)
		{
			std::cerr << "Failed to create commandQueue for device 0";
			return nullptr;
		}

		*device = devices[0];
		delete[] devices;
		return commandQueue;
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

	bool SaveImage(const std::string& fileName, char* buffer, int width, int height)
	{
		FREE_IMAGE_FORMAT format =
			FreeImage_GetFIFFromFilename(fileName.c_str());
		FIBITMAP* image = FreeImage_ConvertFromRawBits((BYTE*)buffer,
			width, height, width * 4, 32,
			0xFF000000, 0x00FF0000, 0x0000FF00);
		return FreeImage_Save(format, image, fileName.c_str());
	}

	void Cleanup(cl_context& context, cl_command_queue& commandQueue, cl_program& program, cl_kernel& kernel, cl_mem images[2])
	{
		clReleaseContext(context);
		clReleaseCommandQueue(commandQueue);
		clReleaseProgram(program);
		clReleaseKernel(kernel);
		clReleaseMemObject(*images);
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