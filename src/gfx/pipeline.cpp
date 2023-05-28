#include "pipeline.hpp"

#include <cassert>

Pipeline::Pipeline(Device &device,
            const std::string& vertFilePath,
            const std::string& fragFilePath,
            const PipelineConfigInfo& configInfo
            ) : device(device) {
    createGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
}

Pipeline::~Pipeline() {
    vkDestroyShaderModule(device.device(), fragShaderModule, nullptr);
    vkDestroyShaderModule(device.device(), vertShaderModule, nullptr);
    vkDestroyPipeline(device.device(), graphicsPipeline, nullptr);
}

std::vector<char> Pipeline::readFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file :" + filePath);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close(); 

    return buffer;
}

void Pipeline::createGraphicsPipeline(
    const std::string& vertFilePath,
    const std::string& fragFilePath,
    const PipelineConfigInfo& configInfo
    ) {

    assert(
        configInfo.pipelineLayout != VK_NULL_HANDLE &&
        "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");

    assert(
        configInfo.renderPass != VK_NULL_HANDLE &&
        "Cannot create graphics pipeline: no renderPass provided in configInfo");

    auto vertCode = readFile(vertFilePath);
    auto fragCode = readFile(fragFilePath);
    
    createShaderModule(vertCode, &vertShaderModule);
    createShaderModule(fragCode, &fragShaderModule);

    VkPipelineShaderStageCreateInfo shaderStages[] = {
        {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_VERTEX_BIT, vertShaderModule, "main", nullptr},
        {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule, "main", nullptr}
    };

    auto bindingDescriptions = VModel::Vertex::getBindingDescriptions();
    auto attributeDescriptions = VModel::Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());;
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();;

    // Combine the viewport and scissor into a viewport state
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &configInfo.viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &configInfo.scissor;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &configInfo.inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &configInfo.rasterizer;
    pipelineInfo.pMultisampleState = &configInfo.multisampling;
    pipelineInfo.pColorBlendState = &configInfo.colorBlending;
    pipelineInfo.pDepthStencilState = &configInfo.depthStencil;
    pipelineInfo.pDynamicState = nullptr;

    pipelineInfo.layout = configInfo.pipelineLayout;
    pipelineInfo.renderPass = configInfo.renderPass;
    pipelineInfo.subpass = configInfo.subpass;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline.");
    }
}

void Pipeline::createShaderModule(const std::vector<char>& code,VkShaderModule* shaderModule) {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module.");
    }    
}

PipelineConfigInfo Pipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
    PipelineConfigInfo configInfo = {};

    configInfo.inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    configInfo.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    configInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewport
    configInfo.viewport.x = 0.0f;
    configInfo.viewport.y = 0.0f;
    configInfo.viewport.width = static_cast<float>(width);
    configInfo.viewport.height = static_cast<float>(height);
    configInfo.viewport.minDepth = 0.0f;
    configInfo.viewport.maxDepth = 1.0f;

    configInfo.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    configInfo.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    
    
    configInfo.depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    configInfo.depthStencil.depthTestEnable = VK_TRUE;
    configInfo.depthStencil.depthWriteEnable = VK_TRUE;
    configInfo.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    configInfo.depthStencil.depthBoundsTestEnable = VK_FALSE;
    configInfo.depthStencil.minDepthBounds = 0.0f;  // Optional
    configInfo.depthStencil.maxDepthBounds = 1.0f;  // Optional
    configInfo.depthStencil.stencilTestEnable = VK_FALSE;
    configInfo.depthStencil.front = {};  // Optional
    configInfo.depthStencil.back = {};   // Optional
    
    configInfo.colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT
      | VK_COLOR_COMPONENT_G_BIT
      | VK_COLOR_COMPONENT_B_BIT
      | VK_COLOR_COMPONENT_A_BIT;
    configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
    configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional
    
    configInfo.colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    configInfo.colorBlending.logicOpEnable = VK_FALSE;
    configInfo.colorBlending.logicOp = VK_LOGIC_OP_COPY;  // Optional
    configInfo.colorBlending.attachmentCount = 1;
    configInfo.colorBlending.pAttachments = &configInfo.colorBlendAttachment;
    configInfo.colorBlending.blendConstants[0] = 0.0f;  // Optional
    configInfo.colorBlending.blendConstants[1] = 0.0f;  // Optional
    configInfo.colorBlending.blendConstants[2] = 0.0f;  // Optional
    configInfo.colorBlending.blendConstants[3] = 0.0f;  // Optional

    // Scissor rectangle
    configInfo.scissor.offset = {0, 0};
    configInfo.scissor.extent = {width, height};

    // Rasterizer
    configInfo.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    configInfo.rasterizer.depthClampEnable = VK_FALSE;
    configInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    configInfo.rasterizer.lineWidth = 1.0f;
    configInfo.rasterizer.cullMode = VK_CULL_MODE_NONE;
    configInfo.rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    configInfo.rasterizer.depthBiasEnable = VK_FALSE;
    configInfo.rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    configInfo.rasterizer.depthBiasClamp = 0.0f; // Optional
    configInfo.rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    return configInfo;
 }

 void Pipeline::bind(VkCommandBuffer commandBuffer) {
     vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
 }