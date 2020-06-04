#pragma once

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/cl.h>

#include <string>
#include <cstdio>

namespace opencl_1n1
{
	int lesson_main();

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
		size_t source_size;
		char* source_str;
		#pragma warning(suppress : 4996)
		auto fp = fopen(kernel_path.c_str(), "r");
		if (!fp) {
			fprintf(stderr, "Failed to load kernel.\n");
			exit(1);
		}
		// obtain file size:
		fseek(fp, 0, SEEK_END);
		auto lSize = ftell(fp);
		rewind(fp);

		source_str = (char*)malloc(sizeof(char) * lSize);
		source_size = fread(source_str, 1, lSize, fp);
		fclose(fp);

		cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &ret);

		ret = clBuildProgram(program, 1, &device_id, nullptr, nullptr, nullptr);

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
}