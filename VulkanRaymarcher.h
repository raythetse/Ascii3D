#pragma once

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.h>
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

// Structured pixel output mapped from VRAM
struct PixelOutput {
    float r, g, b, l;
};

// Packed hardware registers for per-frame data (Must be a multiple of 16 bytes total)
struct ShaderInputs {

    float camPos[3];
    float time;

    float camForward[3];
    float width;

    float camUp[3];
    float height;

    float camRight[3];
    float sceneDepth;

    float surfTolerance;
    float viewportSize[2];
    float pad2;
};

class VulkanRaymarcher {
public:
    VulkanRaymarcher();
    ~VulkanRaymarcher();

    // Prevents accidental copying of hardware resources
    VulkanRaymarcher(const VulkanRaymarcher&) = delete;
    VulkanRaymarcher& operator=(const VulkanRaymarcher&) = delete;

    const std::vector<VkPhysicalDeviceProperties>& enumerateDevices();
    bool selectDevice(size_t selection);
    void setShaderSource(std::string& shaderCode);
    void setCanvasSize(uint32_t width, uint32_t height);
    const PixelOutput* renderFrame(const ShaderInputs& pushData);

private:
    std::vector<VkPhysicalDevice> m_physicalDevices{};
    std::vector<VkPhysicalDeviceProperties> m_pysicalDeviceProps{};
    size_t m_deviceIndex{};

    // Core persistent objects (Never touched on resize)
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkDevice m_LogicalDevice = VK_NULL_HANDLE;
    VkQueue m_ComputeQueue = VK_NULL_HANDLE;
    uint32_t m_QueueFamilyIndex = UINT32_MAX;
    VkFence m_RenderFence = VK_NULL_HANDLE;
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
    VkBuffer m_HostStagingBuffer = VK_NULL_HANDLE; // Cached System RAM for CPU reading
    VkDeviceMemory m_HostStagingMemory = VK_NULL_HANDLE;
    VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
    VkBuffer m_DeviceLocalBuffer = VK_NULL_HANDLE; // Pure VRAM for Compute Shader writes
    VkDeviceMemory m_DeviceLocalMemory = VK_NULL_HANDLE;
    PixelOutput* m_MappedStagingData = nullptr;

    // Helper functions
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void cleanupResolutionResources();
    void cleanupGeneralResources();
    void cleanup();
    std::vector<uint32_t> compileDynamicGlslToSpirV(const std::string& sourceCode);
};
