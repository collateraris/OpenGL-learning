#pragma once

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <optional>
#include <array>
#include <chrono>

namespace vulkan_2_load_model
{
    class ModelViewApplication;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value()
                && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats = {};
        std::vector<VkPresentModeKHR> presentModes = {};
    };

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

            return attributeDescriptions;
        }
    };

    struct UniformBufferObject {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    class ModelViewApplication {
    public:

        static int main();

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

        void DestroyDebugUtilsMessengerEXT(VkInstance instance,
            VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
            const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

        void setupDebugMessenger();

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        void pickPhysicalDevice();

        bool isDeviceSuitable(VkPhysicalDevice device);

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        void createLogicalDevice();

        void createSurface();

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        void createSwapChain();

        void createImageViews();

        void createGraphicsPipeline();
        VkShaderModule createShaderModule(const std::vector<char>& code);
        void createRenderPass();
        void createFramebuffers();
        void createCommandPool();
        void createCommandBuffers();

        void drawFrame();
        void createSyncObjects();

        void recreateSwapChain();
        void cleanupSwapChain();

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

        void createVertexBuffer();
        void createIndexBuffer();
        void createUniformBuffers();
        void updateUniformBuffer(uint32_t currentImage);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void createDescriptorSetLayout();
        void createDescriptorPool();
        void createDescriptorSets();
        void createTextureImage();
        void createImage(uint32_t width, uint32_t height,
            VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
            VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        void createTextureImageView();
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        void createTextureSampler();

        void createDepthResources();
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkFormat findDepthFormat();
        bool hasStencilComponent(VkFormat format);

        void loadModel();

        GLFWwindow* mWindow = nullptr;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;
        VkQueue graphicsQueue;

        VkSurfaceKHR surface;
        VkQueue presentQueue;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages = {};
        std::vector<VkImageView> swapChainImageViews = {};
        std::vector<VkFramebuffer> swapChainFramebuffers = {};
        std::vector<VkCommandBuffer> commandBuffers = {};
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        VkRenderPass renderPass;
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;
        VkCommandPool commandPool;

        std::vector<VkSemaphore> imageAvailableSemaphores = {};
        std::vector<VkSemaphore> renderFinishedSemaphores = {};
        std::vector<VkFence> inFlightFences = {};
        std::vector<VkFence> imagesInFlight = {};
        size_t currentFrame = 0;

        bool framebufferResized = false;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;

        std::vector<VkBuffer> uniformBuffers = {};
        std::vector<VkDeviceMemory> uniformBuffersMemory = {};

        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;

        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;
    };

}