#include "VulkanRaymarcher.h"

VulkanRaymarcher::VulkanRaymarcher()
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
        throw std::runtime_error("Failed to create Vulkan Instance!");
    }
}

bool VulkanRaymarcher::selectDevice() {
    // 2. Select GPU supporting Vulkan 1.3
    uint32_t deviceCount = 0;  
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

    std::vector<VkPhysicalDevice> candidateDevices{};
    std::vector<VkPhysicalDeviceProperties> candidateDeviceProps{};
    for (size_t i{}; i < deviceCount; ++i) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(devices[i], &props);
        if (props.apiVersion >= VK_API_VERSION_1_3) {
            candidateDevices.push_back(devices[i]);
            candidateDeviceProps.push_back(props);
        }
    }

    int selection{ -1 };
    std::cout << "===========================================\n";
    std::cout << "      ASCII Raymarcher 3D - Ray Young\n";
    std::cout << "===========================================\n\n";
    std::cout << "          Valid Rendering Devices:\n\n";
    std::cout << "[1] - CPU (Multi-threaded)\n";

    for (size_t i{}; i < devices.size(); ++i) {
        std::cout << "[" << (i + 2) << "] - " << candidateDeviceProps[i].deviceName << " - " << candidateDeviceProps[i].deviceType << '\n';
    }

    while (true) {
        std::cout << "\nChoose Your Rendering Device: [1-" << devices.size() + 1 << "]\n\n";
        std::cin >> selection;

        if (selection == 1) return false;
        if (selection > 1 && selection <= devices.size() + 1) {
            break;
        }
    }

    m_PhysicalDevice = candidateDevices[selection - 2];

    // 3. Find Compute Queue Family
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilies.data());

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

    if (vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS) {
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
    
    return true;
}

void VulkanRaymarcher::setShaderSource(std::string& shaderCode)
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

void VulkanRaymarcher::setCanvasSize(uint32_t width, uint32_t height) {
    if (width == m_Width && height == m_Height) return;

    cleanupResolutionResources();

    m_Width = width;
    m_Height = height;
    m_BufferSize = m_Width * m_Height * sizeof(PixelOutput);

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = m_BufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkCreateBuffer(m_LogicalDevice, &bufferInfo, nullptr, &m_InfoBuffer);

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(m_LogicalDevice, m_InfoBuffer, &memReqs);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = findMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkAllocateMemory(m_LogicalDevice, &allocInfo, nullptr, &m_BufferMemory);
    vkBindBufferMemory(m_LogicalDevice, m_InfoBuffer, m_BufferMemory, 0);

    // 👉 THE FIX: Allocate a fresh descriptor set from the pool on resize!
    VkDescriptorSetAllocateInfo setAllocInfo{};
    setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocInfo.descriptorPool = m_DescriptorPool;
    setAllocInfo.descriptorSetCount = 1;
    setAllocInfo.pSetLayouts = &m_DescriptorSetLayout;

    // Reset pool or reallocate set safely
    vkAllocateDescriptorSets(m_LogicalDevice, &setAllocInfo, &m_DescriptorSet);

    // 👉 THE CRITICAL LINK: Re-bind the descriptor write mapping to the NEW size!
    VkDescriptorBufferInfo descBufferInfo = { m_InfoBuffer, 0, m_BufferSize };
    VkWriteDescriptorSet descWrite{};
    descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descWrite.dstSet = m_DescriptorSet; // Bind to active tracking handle
    descWrite.dstBinding = 0;
    descWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descWrite.descriptorCount = 1;
    descWrite.pBufferInfo = &descBufferInfo;

    // Commit to logical device
    vkUpdateDescriptorSets(m_LogicalDevice, 1, &descWrite, 0, nullptr);

    vkMapMemory(m_LogicalDevice, m_BufferMemory, 0, m_BufferSize, 0, (void**)&m_MappedData);
}

PixelOutput* VulkanRaymarcher::renderFrame(const ShaderInputs& shaderInputs) {

    uint32_t groupCountX = (m_Width + 7) / 8;
    uint32_t groupCountY = (m_Height + 7) / 8;

    // ADD THIS AUDIT PRINT:
    std::cout << "[Vulkan Render Frame Audit] Active Canvas Size: "
        << m_Width << "x" << m_Height
        << " | Buffer Size: " << m_BufferSize << " bytes\n";
    std::cout << "[Vulkan Dispatch Audit] Launching Work Groups: X="
        << groupCountX << ", Y=" << groupCountY << "\n";

    // ... rest of your command recording, vkCmdDispatch, etc ...
    vkResetCommandBuffer(m_CommandBuffer, 0);
    VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);

    vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline);
    vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);

    vkCmdPushConstants(m_CommandBuffer, m_PipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ShaderInputs), &shaderInputs);

    vkCmdDispatch(m_CommandBuffer, groupCountX, groupCountY, 1);

    VkMemoryBarrier barrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };

    barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
    vkCmdPipelineBarrier(m_CommandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 1, &barrier, 0, nullptr, 0, nullptr);
    vkEndCommandBuffer(m_CommandBuffer);
    VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffer;
    vkQueueSubmit(m_ComputeQueue, 1, &submitInfo, VK_NULL_HANDLE); vkQueueWaitIdle(m_ComputeQueue);
    return m_MappedData;
}

uint32_t VulkanRaymarcher::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties; 
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory type!");
}

std::vector<uint32_t> VulkanRaymarcher::compileDynamicGlslToSpirV(const std::string& sourceCode) {
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
    std::vector<uint32_t> spirv(bytes, bytes + (len / sizeof(uint32_t))); shaderc_result_release(result);
    shaderc_compile_options_release(options);
    shaderc_compiler_release(compiler);
    return spirv;
}

void VulkanRaymarcher::cleanupResolutionResources() {
    if (m_MappedData) {
        vkUnmapMemory(m_LogicalDevice, m_BufferMemory);
        m_MappedData = nullptr;
    }
    if (m_DescriptorSet && m_DescriptorPool) {
        vkFreeDescriptorSets(m_LogicalDevice, m_DescriptorPool, 1, &m_DescriptorSet);
        m_DescriptorSet = VK_NULL_HANDLE;
    }
    if (m_BufferMemory) {
        vkFreeMemory(m_LogicalDevice, m_BufferMemory, nullptr);
        m_BufferMemory = VK_NULL_HANDLE;
    }if (m_InfoBuffer) {
        vkDestroyBuffer(m_LogicalDevice, m_InfoBuffer, nullptr);
        m_InfoBuffer = VK_NULL_HANDLE;
    }
}

void VulkanRaymarcher::cleanup() {
    cleanupResolutionResources();
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
    if (m_Instance) {
        vkDestroyInstance(m_Instance, nullptr);
        m_Instance = VK_NULL_HANDLE;
    }
}