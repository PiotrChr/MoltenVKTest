#include "gfx/window.hpp"
#include "gfx/pipeline.hpp"
#include "gfx/device.hpp"
#include "gfx/swap_chain.hpp"
#include "gfx/model.hpp"

#include <memory>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <array>

class App {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        App() {
            loadModels();
            createPipelineLayout();
            createPipeline();
            createCommandBuffers();
        };
        ~App() {
            vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
        };

        App(const App&) = delete;
        App& operator=(const App&) = delete;

        void loadModels() {
            std::vector<VModel::Vertex> vertices{{{0.0f, -0.5f}}, {{0.5f, 0.5f}}, {{-0.5f, 0.5f}}};
            model = std::make_unique<VModel>(device, vertices);
        };

        void run() {
            while (!window.shouldClose()) {
                glfwPollEvents();
                drawFrame();
            }

            vkDeviceWaitIdle(device.device());
        };
    private:
        void createPipelineLayout() {
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 0;
            pipelineLayoutInfo.pSetLayouts = nullptr;
            pipelineLayoutInfo.pushConstantRangeCount = 0;
            pipelineLayoutInfo.pPushConstantRanges = nullptr;

            if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create pipeline layout.");
            }
        };
        void createPipeline() {
            auto pipelineConfig = Pipeline::defaultPipelineConfigInfo(swapChain.width(), swapChain.height());
            pipelineConfig.renderPass = swapChain.getRenderPass();
            
            pipelineConfig.pipelineLayout = pipelineLayout;
            pipeline = std::make_unique<Pipeline>(
                device,
                "compiledShaders/temp.vert.spv",
                "compiledShaders/temp.frag.spv",
                pipelineConfig
            );
        };
        
        void createCommandBuffers() {
            commandBuffers.resize(swapChain.imageCount());

            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool = device.getCommandPool();
            allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

            if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to allocate command buffers!");
            }

            for (int i = 0; i < commandBuffers.size(); i++) {
                VkCommandBufferBeginInfo beginInfo{};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

                if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
                }

                VkRenderPassBeginInfo renderPassInfo{};
                renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderPassInfo.renderPass = swapChain.getRenderPass();
                renderPassInfo.framebuffer = swapChain.getFrameBuffer(i);

                renderPassInfo.renderArea.offset = {0, 0};
                renderPassInfo.renderArea.extent = swapChain.getSwapChainExtent();

                std::array<VkClearValue, 2> clearValues{};
                clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
                clearValues[1].depthStencil = {1.0f, 0};
                renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
                renderPassInfo.pClearValues = clearValues.data();

                vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

                pipeline->bind(commandBuffers[i]);
                model->bind(commandBuffers[i]);
                model->draw(commandBuffers[i]);

                // vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

                vkCmdEndRenderPass(commandBuffers[i]);
                if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
                }
            }
        };
        void drawFrame() {
            uint32_t imageIndex;
            auto result = swapChain.acquireNextImage(&imageIndex);
            if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                throw std::runtime_error("failed to acquire swap chain image!");
            }

            result = swapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
            if (result != VK_SUCCESS) {
                throw std::runtime_error("failed to present swap chain image!");
            }
            
        };

        VWindow window{WIDTH, HEIGHT, "Hello Vulkan!"};
        Device device{window};
        SwapChain swapChain{device, window.getExtent()};
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<VModel> model;
};