#include "Vulkan/lesson1_triangle/vulkan1_triangle.h"

int main()
{
    vulkan_1_triangle::HelloTriangleApplication app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


