#pragma once

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/cl.h>

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

namespace opencl_1n1
{
	int lesson_main();

	cl_program CreateProgram(cl_context context, cl_device_id device, const std::string& fileName);

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

		std::string kernel_path = "OpenCL/1n1_init/kernels/get_global_id.cl";
		cl_program program = CreateProgram(context, device_id, kernel_path);

		cl_kernel kernel = clCreateKernel(program, "test", &ret);

		cl_mem memobj = NULL;
		int memLenth = 10;
		cl_int* mem = (cl_int*)malloc(sizeof(cl_int) * memLenth);

		memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, memLenth * sizeof(cl_int), NULL, &ret);

		ret = clEnqueueWriteBuffer(command_queue, memobj, CL_TRUE, 0, memLenth * sizeof(cl_int), mem, 0, NULL, NULL);

		ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&memobj);

		size_t global_work_size[1] = { 10 };

		ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);

		ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0, memLenth * sizeof(float), mem, 0, NULL, NULL);

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
			return NULL;
		}

		return program;
	}
}