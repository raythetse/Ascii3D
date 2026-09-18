#pragma once
#include <vulkan/vulkan.hpp>

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <iostream>

class VKHandle {
public:
    // Delete copy constructor and assignment operator to prevent duplicates
    VKHandle(const VKHandle&) = delete;
    void operator=(const VKHandle&) = delete;

    // Static access point
    static VKHandle& Get() {
        static VKHandle contextInstance; // Thread-safe lazy initialization
        return contextInstance;
    }

    // Public getter for the handle
    VkInstance GetInstance() const { return m_Instance; }

private:
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE; // Optional but recommended

    // Private constructor ensures instantiation only happens via Get()
    VKHandle() {
        InitInstance();
    }

    // Private destructor handles clean teardown at program exit
    ~VKHandle() {
        // Destroy debug messenger first if you created one
        if (m_DebugMessenger != VK_NULL_HANDLE) {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr) func(m_Instance, m_DebugMessenger, nullptr);
        }

        // Always destroy the instance last
        if (m_Instance != VK_NULL_HANDLE) {
            vkDestroyInstance(m_Instance, nullptr);
        }
    }

    void InitInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Headless Console App";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        std::vector<const char*> extensions;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.empty() ? nullptr : extensions.data();

        // 👉 NEW: Enable standard validation layers for debugging
        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        // To be safe, we will verify the layer is available on your machine
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp("VK_LAYER_KHRONOS_validation", layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (layerFound) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
            std::cout << "Vulkan Validation Layers enabled successfully!\n";
        }
        else {
            std::cerr << "Warning: VK_LAYER_KHRONOS_validation not found. Install Vulkan SDK components.\n";
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan Instance Singleton!");
        }
    }
};
