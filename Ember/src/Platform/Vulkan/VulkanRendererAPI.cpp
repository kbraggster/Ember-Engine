#include "VulkanRendererAPI.h"

#include <vulkan/vulkan.h>

namespace Ember
{

void VulkanRendererAPI::Init()
{
    EM_CORE_INFO("Initializing Vulkan Renderer API");

    EM_CORE_INFO("Vulkan Renderer API initialized successfully");
}

void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    VkViewport viewport = {};
    viewport.x          = static_cast<float>(x);
    viewport.y          = static_cast<float>(y);
    viewport.width      = static_cast<float>(width);
    viewport.height     = static_cast<float>(height);
    viewport.minDepth   = 0.0f;
    viewport.minDepth   = 1.0f;

    // TODO: Command buffer
    vkCmdSetViewport(nullptr, 0, 1, &viewport);
}

void VulkanRendererAPI::BeginFrame()
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    // TODO: Command buffer
    vkBeginCommandBuffer(VK_NULL_HANDLE, &beginInfo);

    // TODO: Set viewport and scissor to match swapchain extent
}

} // namespace Ember