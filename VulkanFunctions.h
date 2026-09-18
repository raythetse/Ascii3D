#pragma once
#include "VKHandle.h"
#include <shaderc/shaderc.hpp>

namespace VF
{
    inline VkPhysicalDevice selectDeviceFromPrompt() {
        auto instance = VKHandle::Get().GetInstance();
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            std::cout << "No Vulkan-compatible hardware found. Defaulting to Software CPU implementation.\n";
            return VK_NULL_HANDLE;
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        // Filter devices to ensure they support Vulkan 1.3
        std::vector<VkPhysicalDevice> validGpus;
        std::vector<VkPhysicalDeviceProperties> validProps;

        for (const auto& device : devices) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(device, &props);

            // Enforce Vulkan 1.3 requirement for modern dynamic rendering features
            if (props.apiVersion >= VK_API_VERSION_1_3) {
                validGpus.push_back(device);
                validProps.push_back(props);
            }
        }

        // Display the menu to the user
        std::cout << "==========================================\n";
        std::cout << "      ASCII Ray-Marcher - Ray Young       \n";
        std::cout << "==========================================\n";
        std::cout << "      Select your prefered renderer       \n";
        std::cout << "==========================================\n";
        std::cout << "[1] Software (multi-thread CPU)\n";

        for (size_t i = 0; i < validGpus.size(); ++i) {
            std::cout << "[" << (i + 2) << "] ";

            // Print device type cleanly
            switch (validProps[i].deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   std::cout << "(Discrete GPU)   "; break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: std::cout << "(Integrated GPU) "; break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    std::cout << "(Virtual GPU)    "; break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU:            std::cout << "(Vulkan CPU)     "; break;
            default:                                     std::cout << "(Unknown Device) "; break;
            }
            std::cout << validProps[i].deviceName << "\n";
        }
        std::cout << "==========================================\n";

        // Prompt user input
        int choice = -1;
        while (choice < 1 || choice > static_cast<int>(validGpus.size() + 1)) {
            std::cout << "Enter your choice (1-" << (validGpus.size() + 1) << "): ";
            std::cin >> choice;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(INT_MAX, '\n');
                choice = -1;
            }
        }

        // Handle user selection
        if (choice == 1) {
            std::cout << "\nSelected: Software CPU Implementation.\n";
            return VK_NULL_HANDLE;
        }
        else {
            VkPhysicalDevice chosenDevice = validGpus[choice - 2];
            std::cout << "Selected Hardware: " << validProps[choice - 2].deviceName << "\n";
            return chosenDevice;
        }
    }

    inline uint32_t getQueueFamilyIndex(VkPhysicalDevice physicalDevice)
    {
        uint32_t queueFamilyIndex = UINT32_MAX;
        uint32_t queueFamilyCount = 0;

        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            // Check for the COMPUTE bit instead of GRAPHICS
            if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                queueFamilyIndex = i;
                break;
            }
        }

        if (queueFamilyIndex == UINT32_MAX) {
            throw std::runtime_error("The selected GPU does not support Compute operations!");
        }

        std::cout << "Found compatible Compute Queue Family at index: " << queueFamilyIndex << "\n";

        return queueFamilyIndex;
    }

    inline VkDevice getLogicalDevice(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
    {
        // 1. Specify the queue configuration
        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        // 2. Request Vulkan 1.3 Core Features
        VkPhysicalDeviceVulkan13Features features13 = {};
        features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        // Dynamic rendering isn't strictly required for raw compute buffers, 
        // but we keep Vulkan 1.3 core active for modern synchronization features.

        VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
        deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures2.pNext = &features13;

        // 3. Assemble the Logical Device Creation Info
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.pNext = &deviceFeatures2;

        // Headless compute requires zero window extensions, so ppEnabledExtensionNames remains empty.
        createInfo.enabledExtensionCount = 0;
        createInfo.ppEnabledExtensionNames = nullptr;

        // 4. Instantiate the logical device
        VkDevice device;
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device!");
        }

        return device;
    }

    inline uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        throw std::runtime_error("Failed to find suitable memory type!");
    }

    inline std::vector<uint32_t> compileDynamicGlslToSpirV(const std::string& sourceCode) {
        // Initialize the C compiler handle
        shaderc_compiler_t compiler = shaderc_compiler_initialize();
        shaderc_compile_options_t options = shaderc_compile_options_initialize();

        // Set optimizations
        shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_performance);

        // Compile GLSL string directly to SPIR-V
        shaderc_compilation_result_t result = shaderc_compile_into_spv(
            compiler,
            sourceCode.c_str(),
            sourceCode.length(),
            shaderc_glsl_compute_shader,
            "dynamic_raymarcher.comp",
            "main",
            options
        );

        // Error handling
        if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
            std::cerr << "Dynamic Shader Error:\n" << shaderc_result_get_error_message(result) << "\n";

            // Clean up handles before failing
            shaderc_result_release(result);
            shaderc_compile_options_release(options);
            shaderc_compiler_release(compiler);
            throw std::runtime_error("Failed to compile dynamic GLSL!");
        }

        // Pull the raw bytes out of the result handle
        const uint32_t* bytes = reinterpret_cast<const uint32_t*>(shaderc_result_get_bytes(result));
        size_t lengthInBytes = shaderc_result_get_length(result);

        std::vector<uint32_t> spirvBinary(bytes, bytes + (lengthInBytes / sizeof(uint32_t)));

        // Clean up allocated C tracking structures safely
        shaderc_result_release(result);
        shaderc_compile_options_release(options);
        shaderc_compiler_release(compiler);

        return spirvBinary;
    }
}