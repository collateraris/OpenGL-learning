#pragma once

#include <vulkan/vulkan.h>

#include "vulkan1_triangle.h"


using namespace vulkan_1_triangle;

void HelloTriangleApplication::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	mWindow = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
}

void HelloTriangleApplication::initVulkan()
{

}

void HelloTriangleApplication::mainLoop()
{
	while (!glfwWindowShouldClose(mWindow)) {
		glfwPollEvents();
	}
}

void HelloTriangleApplication::cleanup()
{
	glfwDestroyWindow(mWindow);

	glfwTerminate();
}
