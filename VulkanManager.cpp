#include "VulkanManager.h"

VulkanManager::VulkanManager()
{
    // 1. Initialize Vulkan 1.3 Instance
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Headless Console Raymarcher";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    instanceInfo.ppEnabledLayerNames = validationLayers.data();

    if (vkCreateInstance(&instanceInfo, nullptr, &m_Instance) != VK_SUCCESS) {
        throw std::runtime_error{ "Failed to create Vulkan Instance!" };
    }
}

VulkanManager::~VulkanManager() {
    cleanup();
    if (m_Instance) {
        vkDestroyInstance(m_Instance, nullptr);
        m_Instance = VK_NULL_HANDLE;
    }
}

const std::vector<VkPhysicalDeviceProperties>& VulkanManager::enumerateDevices()
{
    if (m_pysicalDeviceProps.size() == 0) {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> devices{ deviceCount };
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        for (size_t i{}; i < deviceCount; ++i) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(devices[i], &props);
            if (props.apiVersion >= VK_API_VERSION_1_3) {
                m_physicalDevices.push_back(devices[i]);
                m_pysicalDeviceProps.push_back(props);
            }
        }
    }

    return m_pysicalDeviceProps;
}

bool VulkanManager::selectDevice(size_t selection) {
    cleanup();

    if (selection >= m_physicalDevices.size()) return false;

    m_deviceIndex = selection;

    // 3. Find Compute Queue Family
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevices[m_deviceIndex], &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies{ queueFamilyCount };
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevices[m_deviceIndex], &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            m_QueueFamilyIndex = i;
            break;
        }
    }

    // 4. Create Logical Device
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = m_QueueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

    VkPhysicalDeviceFeatures2 deviceFeatures2{};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.pNext = &features13;

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.pNext = &deviceFeatures2;

    if (vkCreateDevice(m_physicalDevices[m_deviceIndex], &deviceCreateInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device!");
    }

    vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndex, 0, &m_ComputeQueue);

    // 5. Create Descriptor Set Layout (Binding 0 = Storage Buffer)
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = 0;
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &layoutBinding;
    vkCreateDescriptorSetLayout(m_LogicalDevice, &layoutInfo, nullptr, &m_DescriptorSetLayout);

    // 6. Create Descriptor Pool
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;
    vkCreateDescriptorPool(m_LogicalDevice, &poolInfo, nullptr, &m_DescriptorPool);

    // 7. Create Pipeline Layout with Push Constant range mapped in
    VkPushConstantRange pushRange{};
    pushRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pushRange.offset = 0;
    pushRange.size = sizeof(ShaderInputs);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushRange;
    vkCreatePipelineLayout(m_LogicalDevice, &pipelineLayoutInfo, nullptr, &m_PipelineLayout);

    // Allocate Descriptor Set
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_DescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_DescriptorSetLayout;
    vkAllocateDescriptorSets(m_LogicalDevice, &allocInfo, &m_DescriptorSet);

    // Create execution fence
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = 0;
    vkCreateFence(m_LogicalDevice, &fenceInfo, nullptr, &m_RenderFence);

    return true;
}

void VulkanManager::setShaderSource(std::string& shaderCode)
{
    std::vector<uint32_t> spirvBytes = compileDynamicGlslToSpirV(shaderCode);

    VkShaderModuleCreateInfo shaderModuleInfo{};
    shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleInfo.codeSize = spirvBytes.size() * sizeof(uint32_t);
    shaderModuleInfo.pCode = spirvBytes.data();
    vkCreateShaderModule(m_LogicalDevice, &shaderModuleInfo, nullptr, &m_ComputeShaderModule);

    // 9. Create Compute Pipeline Object
    VkComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = m_PipelineLayout;
    pipelineInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipelineInfo.stage.module = m_ComputeShaderModule;
    pipelineInfo.stage.pName = "main";
    vkCreateComputePipelines(m_LogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_ComputePipeline);

    // 10. Create Command Allocation Pool
    VkCommandPoolCreateInfo cmdPoolInfo{};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex = m_QueueFamilyIndex;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vkCreateCommandPool(m_LogicalDevice, &cmdPoolInfo, nullptr, &m_CommandPool);

    VkCommandBufferAllocateInfo cmdAllocInfo{};
    cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdAllocInfo.commandPool = m_CommandPool;
    cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdAllocInfo.commandBufferCount = 1;
    vkAllocateCommandBuffers(m_LogicalDevice, &cmdAllocInfo, &m_CommandBuffer);
}

void VulkanManager::setCanvasSize(uint32_t width, uint32_t height) {
    if (width == m_Width && height == m_Height) return;

    cleanupResolutionResources();

    m_Width = width;
    m_Height = height;
    VkDeviceSize bufferSize = m_Width * m_Height * sizeof(PixelOutput);

    // ============================================================================
    // BUFFER 1: DEVICE LOCAL VRAM BUFFER (Compute Shader Output Target)
    // ============================================================================
    VkBufferCreateInfo devBufferInfo{};
    devBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    devBufferInfo.size = bufferSize;
    devBufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    devBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_LogicalDevice, &devBufferInfo, nullptr, &m_DeviceLocalBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create device local buffer!");
    }

    VkMemoryRequirements devMemReqs;
    vkGetBufferMemoryRequirements(m_LogicalDevice, m_DeviceLocalBuffer, &devMemReqs);

    VkMemoryAllocateInfo devAllocInfo{};
    devAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    devAllocInfo.allocationSize = devMemReqs.size;
    devAllocInfo.memoryTypeIndex = findMemoryType(devMemReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(m_LogicalDevice, &devAllocInfo, nullptr, &m_DeviceLocalMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate device local buffer memory!");
    }
    vkBindBufferMemory(m_LogicalDevice, m_DeviceLocalBuffer, m_DeviceLocalMemory, 0);

    // ============================================================================
    // BUFFER 2: HOST STAGING BUFFER (CPU-Cached System Memory Destination)
    // ============================================================================
    VkBufferCreateInfo stageBufferInfo{};
    stageBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stageBufferInfo.size = bufferSize;
    stageBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    stageBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_LogicalDevice, &stageBufferInfo, nullptr, &m_HostStagingBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create host staging buffer!");
    }

    VkMemoryRequirements stageMemReqs;
    vkGetBufferMemoryRequirements(m_LogicalDevice, m_HostStagingBuffer, &stageMemReqs);

    VkMemoryAllocateInfo stageAllocInfo{};
    stageAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    stageAllocInfo.allocationSize = stageMemReqs.size;
    stageAllocInfo.memoryTypeIndex = findMemoryType(stageMemReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    if (vkAllocateMemory(m_LogicalDevice, &stageAllocInfo, nullptr, &m_HostStagingMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate host staging memory!");
    }
    vkBindBufferMemory(m_LogicalDevice, m_HostStagingBuffer, m_HostStagingMemory, 0);

    // Persistently map the host staging memory once
    if (vkMapMemory(m_LogicalDevice, m_HostStagingMemory, 0, VK_WHOLE_SIZE, 0, (void**)&m_MappedStagingData) != VK_SUCCESS) {
        throw std::runtime_error("Failed to map host staging memory!");
    }

    // Update Descriptor Sets to point to the new Device Local VRAM buffer
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = m_DeviceLocalBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = bufferSize;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = m_DescriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(m_LogicalDevice, 1, &descriptorWrite, 0, nullptr);
}

uint32_t VulkanManager::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevices[m_deviceIndex], &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory type!");
}

const PixelOutput* VulkanManager::renderFrame(const ShaderInputs& inputs) {
    vkResetFences(m_LogicalDevice, 1, &m_RenderFence);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);

    // 1. Dispatch compute layout onto Device Local VRAM
    vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline);
    vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
    vkCmdPushConstants(m_CommandBuffer, m_PipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ShaderInputs), &inputs);

    // Using 16x16 local workgroups for ideal Nvidia occupancy scaling
    uint32_t dispatchX = (m_Width + 15) / 16;
    uint32_t dispatchY = (m_Height + 15) / 16;
    vkCmdDispatch(m_CommandBuffer, dispatchX, dispatchY, 1);

    // 2. Barrier: Halt pipeline until compute writes completely finish
    VkBufferMemoryBarrier copyBarrier{};
    copyBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    copyBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    copyBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    copyBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    copyBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    copyBarrier.buffer = m_DeviceLocalBuffer;
    copyBarrier.offset = 0;
    copyBarrier.size = VK_WHOLE_SIZE;

    vkCmdPipelineBarrier(
        m_CommandBuffer,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0, 0, nullptr, 1, &copyBarrier, 0, nullptr
    );

    // 3. Copy: Hardware block copy from VRAM over to CPU-Cached System RAM
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = m_Width * m_Height * sizeof(PixelOutput);
    vkCmdCopyBuffer(m_CommandBuffer, m_DeviceLocalBuffer, m_HostStagingBuffer, 1, &copyRegion);

    // 4. Barrier: Halt pipeline until transfer operations are fully completed
    VkBufferMemoryBarrier hostBarrier{};
    hostBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    hostBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    hostBarrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
    hostBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    hostBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    hostBarrier.buffer = m_HostStagingBuffer;
    hostBarrier.offset = 0;
    hostBarrier.size = VK_WHOLE_SIZE;

    vkCmdPipelineBarrier(
        m_CommandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_HOST_BIT,
        0, 0, nullptr, 1, &hostBarrier, 0, nullptr
    );

    vkEndCommandBuffer(m_CommandBuffer);

    // Submit queue execution block
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffer;

    vkQueueSubmit(m_ComputeQueue, 1, &submitInfo, m_RenderFence);

    // Wait for fence on CPU
    vkWaitForFences(m_LogicalDevice, 1, &m_RenderFence, VK_TRUE, UINT64_MAX);

    // 5. Cache Invalidation: Sync CPU lines with systemic RAM payload
    VkMappedMemoryRange invalidateRange{};
    invalidateRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    invalidateRange.memory = m_HostStagingMemory;
    invalidateRange.offset = 0;
    invalidateRange.size = VK_WHOLE_SIZE;
    vkInvalidateMappedMemoryRanges(m_LogicalDevice, 1, &invalidateRange);

    return m_MappedStagingData;
}

std::vector<uint32_t> VulkanManager::compileDynamicGlslToSpirV(const std::string& sourceCode) {
    shaderc_compiler_t compiler = shaderc_compiler_initialize();
    shaderc_compile_options_t options = shaderc_compile_options_initialize();
    shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_performance);
    shaderc_compilation_result_t result = shaderc_compile_into_spv(compiler, sourceCode.c_str(), sourceCode.length(), shaderc_glsl_compute_shader, "dynamic_raymarcher.comp", "main", options);
    
    if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
        std::cerr << "GLSL Compile Error:\n" << shaderc_result_get_error_message(result) << "\n";
        shaderc_result_release(result); shaderc_compile_options_release(options);
        shaderc_compiler_release(compiler); throw std::runtime_error("Dynamic shader generation compilation failed!");
    }
    
    const uint32_t* bytes = reinterpret_cast<const uint32_t*>(shaderc_result_get_bytes(result));
    size_t len = shaderc_result_get_length(result);
    std::vector<uint32_t> spirv(bytes, bytes + (len / sizeof(uint32_t)));
    shaderc_result_release(result);
    shaderc_compile_options_release(options);
    shaderc_compiler_release(compiler);
    return spirv;
}

void VulkanManager::cleanupResolutionResources() {
    if (m_HostStagingMemory != VK_NULL_HANDLE) {
        vkUnmapMemory(m_LogicalDevice, m_HostStagingMemory);
        vkFreeMemory(m_LogicalDevice, m_HostStagingMemory, nullptr);
        m_HostStagingMemory = VK_NULL_HANDLE;
    }
    if (m_HostStagingBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_LogicalDevice, m_HostStagingBuffer, nullptr);
        m_HostStagingBuffer = VK_NULL_HANDLE;
    }
    if (m_DeviceLocalMemory != VK_NULL_HANDLE) {
        vkFreeMemory(m_LogicalDevice, m_DeviceLocalMemory, nullptr);
        m_DeviceLocalMemory = VK_NULL_HANDLE;
    }
    if (m_DeviceLocalBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_LogicalDevice, m_DeviceLocalBuffer, nullptr);
        m_DeviceLocalBuffer = VK_NULL_HANDLE;
    }
    m_MappedStagingData = nullptr;
}

void VulkanManager::cleanupGeneralResources()
{
    if (m_RenderFence != VK_NULL_HANDLE) {
        vkDestroyFence(m_LogicalDevice, m_RenderFence, nullptr);
        m_RenderFence = VK_NULL_HANDLE;
    }
    if (m_CommandPool) {
        vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);
        m_CommandPool = VK_NULL_HANDLE;
    }
    if (m_ComputePipeline) {
        vkDestroyPipeline(m_LogicalDevice, m_ComputePipeline, nullptr);
        m_ComputePipeline = VK_NULL_HANDLE;
    }
    if (m_ComputeShaderModule) {
        vkDestroyShaderModule(m_LogicalDevice, m_ComputeShaderModule, nullptr);
        m_ComputeShaderModule = VK_NULL_HANDLE;
    }
    if (m_PipelineLayout) {
        vkDestroyPipelineLayout(m_LogicalDevice, m_PipelineLayout, nullptr);
        m_PipelineLayout = VK_NULL_HANDLE;
    }
    if (m_DescriptorPool) {
        vkDestroyDescriptorPool(m_LogicalDevice, m_DescriptorPool, nullptr);
        m_DescriptorPool = VK_NULL_HANDLE;
    }
    if (m_DescriptorSetLayout) {
        vkDestroyDescriptorSetLayout(m_LogicalDevice, m_DescriptorSetLayout, nullptr);
        m_DescriptorSetLayout = VK_NULL_HANDLE;
    }
    if (m_LogicalDevice) {
        vkDestroyDevice(m_LogicalDevice, nullptr);
        m_LogicalDevice = VK_NULL_HANDLE;
    }
}

void VulkanManager::cleanup() {
    cleanupResolutionResources();
    cleanupGeneralResources();
}