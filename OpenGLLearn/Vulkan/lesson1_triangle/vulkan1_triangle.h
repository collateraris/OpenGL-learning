#pragma once

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <optional>

namespace vulkan_1_triangle
{
    class HelloTriangleApplication;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value()
                && presentFamily.has_value();
        }
    };

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

        void pickPhysicalDevice();

        bool isDeviceSuitable(VkPhysicalDevice& device);

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        void createLogicalDevice();

        void createSurface();

        GLFWwindow* mWindow = nullptr;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;
        VkQueue graphicsQueue;

        VkSurfaceKHR surface;
        VkQueue presentQueue;
    };

}