#pragma once

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/cl.h>

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

namespace opencl_1n1
{
	const std::size_t ARRAY_SIZE = 10;

	int lesson_main();

	cl_context CreateContext();

	cl_command_queue CreateCommandQueue(cl_context context, cl_device_id* device);

	cl_program CreateProgram(cl_context context, cl_device_id device, const std::string& fileName);

	void Cleanup(cl_context& context, cl_command_queue& commandQueue, cl_program& program, cl_kernel& kernel, cl_mem memObjects[3]);

	bool CreateMemObjects(cl_context context, cl_mem memObjects[3], float* a, float* b);

	int lesson_main()
	{
		cl_context context = 0;
		cl_command_queue commandQueue = 0;
		cl_program program = 0;
		cl_device_id device = 0;
		cl_kernel kernel = 0;
		cl_mem memObjects[3] = { 0, 0, 0 };
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
			Cleanup(context, commandQueue, program, kernel, memObjects);
			return 1;
		}

		program = CreateProgram(context, device, "OpenCL/1n1_init/kernels/HelloWorld.cl");
		if (program == nullptr)
		{
			Cleanup(context, commandQueue, program, kernel, memObjects);
			return 1;
		}

		kernel = clCreateKernel(program, "hello_kernel", nullptr);
		if (kernel == nullptr)
		{
			std::cerr << "Failed to create kernel" << std::endl;
			Cleanup(context, commandQueue, program, kernel, memObjects);
			return 1;
		}

		float result[ARRAY_SIZE];
		float a[ARRAY_SIZE];
		float b[ARRAY_SIZE];
		for (int i = 0; i < ARRAY_SIZE; i++)
		{
			a[i] = static_cast<float>(i);
			b[i] = static_cast<float>(i * 2);
		}

		if (!CreateMemObjects(context, memObjects, a, b))
		{
			Cleanup(context, commandQueue, program, kernel, memObjects);
			return 1;
		}

		errNum = clSetKernelArg(kernel, 0, sizeof(cl_mem), &memObjects[0]);
		errNum |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &memObjects[1]);
		errNum |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &memObjects[2]);
		if (errNum != CL_SUCCESS)
		{
			std::cerr << "Error setting kernel arguments." << std::endl;
			Cleanup(context, commandQueue, program, kernel, memObjects);
			return 1;
		}

		size_t globalWorkSize[1] = { ARRAY_SIZE };
		size_t localWorkSize[1] = { 1 };

		errNum = clEnqueueNDRangeKernel(commandQueue, kernel, 1, nullptr,
			globalWorkSize, localWorkSize,
			0, nullptr, nullptr);
		if (errNum != CL_SUCCESS)
		{
			std::cerr << "Error queuing kernel for execution." << std::endl;
			Cleanup(context, commandQueue, program, kernel, memObjects);
			return 1;
		}

		errNum = clEnqueueReadBuffer(commandQueue, memObjects[2],
			CL_TRUE, 0,
			ARRAY_SIZE * sizeof(float), result,
			0, nullptr, nullptr);
		if (errNum != CL_SUCCESS)
		{
			std::cerr << "Error reading result buffer." << std::endl;
			Cleanup(context, commandQueue, program, kernel, memObjects);
			return 1;
		}

		for (int i = 0; i < ARRAY_SIZE; i++)
		{
			std::cout << result[i] << " ";
		}

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

		errNum = clBuildProgram(program, 0, nullptr, nullptr, nullptr, nullptr);
		if (errNum != CL_SUCCESS)
		{
			// Determine the reason for the error
			char buildLog[16384];
			clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
				sizeof(buildLog), buildLog, nullptr);
			std::cerr << "Error in kernel: " << std::endl;
			std::cerr << buildLog;
			clReleaseProgram(program);
			return nullptr;
		}

		return program;
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

	bool CreateMemObjects(cl_context context, cl_mem memObjects[3], float* a, float* b)
	{
		memObjects[0] = clCreateBuffer(context, CL_MEM_READ_ONLY |
			CL_MEM_COPY_HOST_PTR,
			sizeof(float) * ARRAY_SIZE, a,
			nullptr);
		memObjects[1] = clCreateBuffer(context, CL_MEM_READ_ONLY |
			CL_MEM_COPY_HOST_PTR,
			sizeof(float) * ARRAY_SIZE, b,
			nullptr);
		memObjects[2] = clCreateBuffer(context, CL_MEM_READ_WRITE,
			sizeof(float) * ARRAY_SIZE,
			nullptr, nullptr);

		if (memObjects[0] == nullptr || memObjects[1] == nullptr ||
			memObjects[2] == nullptr)
		{
			std::cerr << "Error creating memory objects." << std::endl;
			return false;
		}
		return true;
	}

	void Cleanup(cl_context& context, cl_command_queue& commandQueue, cl_program& program, cl_kernel& kernel, cl_mem memObjects[3])
	{
		clReleaseContext(context);
		clReleaseCommandQueue(commandQueue);
		clReleaseProgram(program);
		clReleaseKernel(kernel);
		clReleaseMemObject(*memObjects);
	}
}