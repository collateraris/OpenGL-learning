#pragma once

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>

namespace vulkan_1_triangle
{
    class HelloTriangleApplication;

    class HelloTriangleApplication {
    public:
        void run() {
            initWindow();
            initVulkan();
            mainLoop();
            cleanup();
        }

    private:
        void initWindow();

        void initVulkan();

        void mainLoop();

        void cleanup();

        void createInstance();

        bool checkValidationLayerSupport();

        std::vector<const char*> getRequiredExtensions();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);

        void setupDebugMessenger();

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        GLFWwindow* mWindow = nullptr;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
    };

}