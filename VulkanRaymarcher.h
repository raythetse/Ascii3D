#pragma once

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.h>
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

// Structured pixel output mapped from VRAM
struct PixelOutput {
    float r, g, b;
    float depth;
};

// Packed hardware registers for per-frame data (Must be a multiple of 16 bytes total)
struct ShaderInputs {
    float time;
    float customFloat;
    float width; // explicit 16-byte structural alignment boundary block
    float height;

    float camPos[3];   // X, Y, Z raw components
    float pad2;        // Pad out to 16 bytes boundary

    float camForward[3];  // X, Y, Z raw components
    float pad3;

    float camUp[3];  // X, Y, Z raw components
    float pad4;

    float camRight[3];  // X, Y, Z raw components
    float pad5;
};

class VulkanRaymarcher {
public:
    VulkanRaymarcher();
    ~VulkanRaymarcher() { cleanup(); }

    // Prevents accidental copying of hardware resources
    VulkanRaymarcher(const VulkanRaymarcher&) = delete;
    VulkanRaymarcher& operator=(const VulkanRaymarcher&) = delete;

    bool selectDevice();
    void setShaderSource(std::string& shaderCode);
    void setCanvasSize(uint32_t width, uint32_t height);
    PixelOutput* renderFrame(const ShaderInputs& pushData);

private:
    // Core persistent objects (Never touched on resize)
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_LogicalDevice = VK_NULL_HANDLE;
    VkQueue m_ComputeQueue = VK_NULL_HANDLE;
    uint32_t m_QueueFamilyIndex = UINT32_MAX;

    VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    VkShaderModule m_ComputeShaderModule = VK_NULL_HANDLE;
    VkPipeline m_ComputePipeline = VK_NULL_HANDLE;
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
    std::string m_shaderSourceCode{};

    // Dynamic resolution-dependent objects
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
    VkDeviceSize m_BufferSize = 0;
    VkBuffer m_InfoBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
    VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
    PixelOutput* m_MappedData = nullptr; // Kept persistently mapped!

    // Helper functions
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void cleanupResolutionResources();
    void cleanup();
    std::vector<uint32_t> compileDynamicGlslToSpirV(const std::string& sourceCode);
};
