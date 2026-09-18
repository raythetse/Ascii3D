#include "ConsoleFunctions.h"
#include "GameEngine.h"
#include <fstream>
#include <iostream>



#include "VKHandle.h"
#include "VulkanFunctions.h"



int main()
{
    //ConsoleFunctions::setWindowName(L"ASCII Ray-Marcher - Ray Young");
    //ConsoleFunctions::setupDedicatedViewport();
    //ConsoleFunctions::desyncSTDIO();
    //ConsoleFunctions::setPrecision(3);
    //ConsoleFunctions::goFullscreen();
    //GameEngine{ 15, 5, 100.0, 1.0, 0.001 }.run();
    //ConsoleFunctions::restoreTerminal();

    VkPhysicalDevice physicalDevice = VF::selectDeviceFromPrompt();
    if (physicalDevice == VK_NULL_HANDLE) {
        //runSoftwareCpuRenderer();
    }
    else {
        auto queueFamilyIndex = VF::getQueueFamilyIndex(physicalDevice);
        auto logicalDevice = VF::getLogicalDevice(physicalDevice, queueFamilyIndex);

        VkQueue computeQueue;
        vkGetDeviceQueue(logicalDevice, queueFamilyIndex, 0, &computeQueue);

        /// Setting up memory allocation
        struct PixelOutput {
            float r, g, b;
            float depth;
        };

        const uint32_t CONSOLE_WIDTH = 120;
        const uint32_t CONSOLE_HEIGHT = 40;
        VkDeviceSize bufferSize = CONSOLE_WIDTH * CONSOLE_HEIGHT * sizeof(PixelOutput);

        VkBuffer infoBuffer;
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Only our compute queue needs this

        if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &infoBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create buffer handle!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(logicalDevice, infoBuffer, &memRequirements);

        uint32_t memoryTypeIndex = VF::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        VkDeviceMemory bufferMemory;
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;

        if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate GPU memory!");
        }

        // Bind the physical memory block to our buffer object
        vkBindBufferMemory(logicalDevice, infoBuffer, bufferMemory, 0);

        std::cout << "Successfully allocated " << bufferSize << " bytes of GPU memory for your RGB+Depth frame!\n";

        VkDescriptorSetLayout descriptorSetLayout;

        // 1. Describe the binding slot (Slot 0)
        VkDescriptorSetLayoutBinding layoutBinding = {};
        layoutBinding.binding = 0; // Match the binding = 0 layout qualifier in your future shader
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        layoutBinding.descriptorCount = 1; // Just 1 array buffer
        layoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT; // Accessible by compute shader only
        layoutBinding.pImmutableSamplers = nullptr;

        // 2. Wrap it in the creation structure
        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &layoutBinding;

        // 3. Create the layout using your logicalDevice
        if (vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout!");
        }

        std::cout << "Descriptor Set Layout blueprint created successfully.\n";

        VkDescriptorPool descriptorPool;

        // 1. Specify how many descriptors of each type we need inside the pool
        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; // We only need storage buffers
        poolSize.descriptorCount = 1;                      // Just 1 descriptor slot

        // 2. Combine into the pool creation structure
        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = 1; // We are only allocating 1 descriptor set total from this pool

        // 3. Create the pool using your logicalDevice
        if (vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool!");
        }

        std::cout << "Descriptor Pool allocated successfully.\n";

        // 1. Declare the missing handle variable!
        VkDescriptorSet descriptorSet;

        // 2. Describe the allocation request from the pool
        VkDescriptorSetAllocateInfo descriptorAllocInfo = {};
        descriptorAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorAllocInfo.descriptorPool = descriptorPool; // Your compiled pool
        descriptorAllocInfo.descriptorSetCount = 1;
        descriptorAllocInfo.pSetLayouts = &descriptorSetLayout; // Your blueprint layout

        // 3. Allocate it from the hardware pool
        if (vkAllocateDescriptorSets(logicalDevice, &descriptorAllocInfo, &descriptorSet) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor set!");
        }

        std::cout << "Descriptor Set handle successfully allocated from pool.\n";

        // 1. Describe the specific buffer region we want to bind
        VkDescriptorBufferInfo descriptorBufferInfo = {};
        descriptorBufferInfo.buffer = infoBuffer; // Your RGB+Depth buffer handle
        descriptorBufferInfo.offset = 0;          // Start from the very beginning of the buffer
        descriptorBufferInfo.range = bufferSize;  // Bind the entire size of the buffer (CONSOLE_WIDTH * CONSOLE_HEIGHT * sizeof(PixelOutput))

        // 2. Configure the write operation
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSet;                  // Write into the set you just allocated
        descriptorWrite.dstBinding = 0;                          // Target Binding Slot 0 (matches layout(binding = 0) in your shader)
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &descriptorBufferInfo;     // Pass our buffer address mapping

        // 3. Commit the update to the logical device!
        vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, nullptr);

        std::cout << "Descriptor Set securely bound to infoBuffer!\n";

        // 1. Generate your dynamic shader code string on the fly
        std::string dynamicShaderGlsl = R"(
            #version 450
            layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

            struct PixelOutput { 
                float r, g, b; 
                float depth; 
            };
    
            layout(std430, binding = 0) buffer OutputBuffer { 
                PixelOutput pixels[]; 
            };

            void main() {
                // Get global pixel coordinate
                uint x = gl_GlobalInvocationID.x;
                uint y = gl_GlobalInvocationID.y;

                // Hardcode constraints directly to match your canvas variables 
                if (x >= 120 || y >= 40) return;

                // Flatten 2D grid index to 1D array index
                uint index = (y * 120) + x;

                // FORCE EVERY PIXEL TO WRITE SOLID STABLE TEST DATA
                pixels[index].r = x / 120.0; 
                pixels[index].g = y / 40.0;
                pixels[index].b = 0.0f;
                pixels[index].depth = 12.34f;
            }
        )";

        // 2. Compile it right now in memory
        std::vector<uint32_t> spirvBinary = VF::compileDynamicGlslToSpirV(dynamicShaderGlsl);

        // 3. Create the Vulkan Shader Module directly from the binary vector
        VkShaderModuleCreateInfo shaderModuleInfo = {};
        shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleInfo.codeSize = spirvBinary.size() * sizeof(uint32_t); // Size in bytes
        shaderModuleInfo.pCode = spirvBinary.data();                        // Array of uint32_t

        VkShaderModule computeShaderModule;
        if (vkCreateShaderModule(logicalDevice, &shaderModuleInfo, nullptr, &computeShaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create dynamic compute shader module!");
        }

        std::cout << "Dynamic GLSL successfully compiled to SPIR-V and loaded into Vulkan!\n";

        VkPipelineLayout pipelineLayout;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; // Tie your descriptor blueprint here

        if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        std::cout << "Pipeline Layout established.\n";

        VkPipeline computePipeline;

        VkComputePipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = pipelineLayout; // Ties our descriptor set bindings to the shader

        // Define the shader stage execution parameters
        pipelineInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipelineInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        pipelineInfo.stage.module = computeShaderModule; // Your dynamically compiled module
        pipelineInfo.stage.pName = "main";               // The entry point function inside your GLSL string

        // Optional pipeline caching flags (we pass VK_NULL_HANDLE for simple setups)
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        // Create the unified compute pipeline
        if (vkCreateComputePipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &computePipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create compute pipeline!");
        }

        std::cout << "Hardware Compute Pipeline compiled and ready for execution!\n";

        VkCommandPool commandPool;

        VkCommandPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        // Tie the pool to your compute queue family index we found earlier
        poolCreateInfo.queueFamilyIndex = queueFamilyIndex;
        // Allow the pool to reset buffers individually if you want to reuse them every frame
        poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(logicalDevice, &poolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool!");
        }

        VkCommandBuffer commandBuffer;

        VkCommandBufferAllocateInfo cbAllocInfo = {};
        cbAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cbAllocInfo.commandPool = commandPool;
        cbAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Can be submitted directly to a queue
        cbAllocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(logicalDevice, &cbAllocInfo, &commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffer!");
        }

        // 1. Open the command buffer for recording
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // We will submit and re-record this layout

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        // 2. Bind your compiled compute pipeline
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);

        // 3. Bind the descriptor set that points to your infoBuffer
        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_COMPUTE,
            pipelineLayout,
            0, 1, &descriptorSet,
            0, nullptr
        );

        // 4. Calculate thread groups (120 / 8 = 15 groups, 40 / 8 = 5 groups)
        uint32_t groupCountX = (CONSOLE_WIDTH + 7) / 8;  // Safe ceiling division
        uint32_t groupCountY = (CONSOLE_HEIGHT + 7) / 8;

        // 5. Tell the GPU to launch the threads!
        vkCmdDispatch(commandBuffer, groupCountX, groupCountY, 1);
        // 1-4. (Your existing command recording code: begin, bind pipeline, bind descriptors)
        vkCmdDispatch(commandBuffer, groupCountX, groupCountY, 1);

        // 👉 NEW: Inject a memory barrier to flush the Storage Buffer to Host memory!
        VkMemoryBarrier memoryBarrier = {};
        memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        // What access was just performed? A shader storage write.
        memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        // What access is about to be performed? A CPU read.
        memoryBarrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, // Wait for compute shader to finish
            VK_PIPELINE_STAGE_HOST_BIT,           // Block until it's ready for the CPU/Host
            0,                                    // No dependency flags
            1, &memoryBarrier,                    // Global memory barrier count & pointer
            0, nullptr,                           // No explicit buffer barriers
            0, nullptr                            // No explicit image barriers
        );

        // 6. Close the command ledger
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        // Submit the commands to the async hardware queue
        vkQueueSubmit(computeQueue, 1, &submitInfo, VK_NULL_HANDLE);

        // Wait for the GPU to completely finish execution before proceeding on the CPU
        vkQueueWaitIdle(computeQueue);

        std::cout << "Attempting to map memory for final print...\n";

        void* dataPtr = nullptr;
        vkMapMemory(logicalDevice, bufferMemory, 0, bufferSize, 0, &dataPtr);

        std::string charGradient{ "-~+*vri325E8B@" };
        PixelOutput* finalOutput = static_cast<PixelOutput*>(dataPtr);
        for (uint32_t y = 0; y < CONSOLE_HEIGHT; ++y) {
            for (uint32_t x = 0; x < CONSOLE_WIDTH; ++x) {
                uint32_t idx = y * CONSOLE_WIDTH + x;
                PixelOutput pixel = finalOutput[idx];

                // Example: simple ASCII brightness ramp using the Red channel output
                double brightness = static_cast<double>(pixel.g);
                brightness = std::clamp(brightness, 0.0, 1.0);
                // Feed brightness/depth into your existing console rendering character select...
                brightness = std::floor(brightness * (charGradient.size() - 1));

                std::cout << charGradient[brightness];
            }
            std::cout << '\n';
        }

        // Unmap immediately so the memory lock is safely released
        vkUnmapMemory(logicalDevice, bufferMemory);
        
        std::cout << "Memory unmapped cleanly!\n";





        //CLEAN UP CODE
        if (commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
        commandPool = VK_NULL_HANDLE;
}
        // 1. Free the physical GPU memory block
        if (bufferMemory != VK_NULL_HANDLE) {
            vkFreeMemory(logicalDevice, bufferMemory, nullptr);
            bufferMemory = VK_NULL_HANDLE;
        }

        // 2. Destroy the logical buffer handle
        if (infoBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(logicalDevice, infoBuffer, nullptr);
            infoBuffer = VK_NULL_HANDLE;
        }

        // Add this right before vkDestroyDescriptorSetLayout
        if (descriptorPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
            descriptorPool = VK_NULL_HANDLE;
        }

        // Add this right before vkDestroyDevice
        if (descriptorSetLayout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);
            descriptorSetLayout = VK_NULL_HANDLE;
        }

        if (computePipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(logicalDevice, computePipeline, nullptr);
            computePipeline = VK_NULL_HANDLE;
        }

        if (pipelineLayout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
            pipelineLayout = VK_NULL_HANDLE;
        }

        // FIX: Destroy your dynamically generated shader module handle!
        if (computeShaderModule != VK_NULL_HANDLE) {
            vkDestroyShaderModule(logicalDevice, computeShaderModule, nullptr);
            computeShaderModule = VK_NULL_HANDLE;
        }

        // 3. Destroy the logical device (this should happen after your buffers are gone)
        if (logicalDevice != VK_NULL_HANDLE) {
            vkDestroyDevice(logicalDevice, nullptr);
            logicalDevice = VK_NULL_HANDLE;
        }
    }

    return 0;
}
