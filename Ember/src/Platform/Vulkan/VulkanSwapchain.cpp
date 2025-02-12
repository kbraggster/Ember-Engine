#include "VulkanSwapchain.h"

namespace Ember
{

VulkanSwapchain::VulkanSwapchain(const VulkanDevice& device, const VkSurfaceKHR& surface)
    : m_Device(device), m_Surface(surface)
{
}

VulkanSwapchain::~VulkanSwapchain()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(m_Device.GetVkDevice(), m_ImageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(m_Device.GetVkDevice(), m_RenderFinishedSemaphores[i], nullptr);
        vkDestroyFence(m_Device.GetVkDevice(), m_InFlightFences[i], nullptr);
    }

    Cleanup();
}

void VulkanSwapchain::CreateSwapchain(uint32_t width, uint32_t height)
{
    // Query swapchain support details
    SwapchainSupportDetails details = QuerySwapchainSupport(m_Device.GetVkPhysicalDevice(), m_Surface);

    // Choose swapchain settings
    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(details.Formats);
    VkPresentModeKHR presentMode     = ChooseSwapPresentMode(details.PresentModes);
    VkExtent2D extent                = ChooseSwapExtent(details.Capabilities, width, height);

    uint32_t imageCount = details.Capabilities.minImageCount + 1;
    if (details.Capabilities.maxImageCount > 0 && imageCount > details.Capabilities.maxImageCount)
    {
        imageCount = details.Capabilities.maxImageCount;
    }

    // Create the swapchain
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface          = m_Surface;
    createInfo.minImageCount    = imageCount;
    createInfo.imageFormat      = surfaceFormat.format;
    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
    createInfo.imageExtent      = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const QueueFamilyIndices& indices = m_Device.GetQueueFamilyIndices();
    uint32_t queueFamilyIndices[]     = {indices.GraphicsFamily, indices.PresentFamily};
    createInfo.imageSharingMode       = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount  = 0;
    createInfo.pQueueFamilyIndices    = nullptr;
    createInfo.preTransform           = details.Capabilities.currentTransform;
    createInfo.compositeAlpha         = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode            = presentMode;
    createInfo.clipped                = VK_TRUE;
    createInfo.oldSwapchain           = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(m_Device.GetVkDevice(), &createInfo, nullptr, &m_Swapchain);
    EM_CORE_ASSERT(result == VK_SUCCESS, "Failed to create swapchain!");

    EM_CORE_INFO("Swapchain created");

    // Retrieve swapchain images
    vkGetSwapchainImagesKHR(m_Device.GetVkDevice(), m_Swapchain, &imageCount, nullptr);
    m_SwapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_Device.GetVkDevice(), m_Swapchain, &imageCount, m_SwapchainImages.data());

    m_SwapchainImageFormat = surfaceFormat.format;
    m_SwapchainExtent      = extent;

    // Create image views
    m_SwapchainImageViews.resize(m_SwapchainImages.size());
    for (size_t i = 0; i < m_SwapchainImages.size(); i++)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image                           = m_SwapchainImages[i];
        viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format                          = m_SwapchainImageFormat;
        viewInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        result = vkCreateImageView(m_Device.GetVkDevice(), &viewInfo, nullptr, &m_SwapchainImageViews[i]);
        EM_CORE_ASSERT(result == VK_SUCCESS, "Failed to create image view!");
    }
    CreateSemaphores();
}

void VulkanSwapchain::CreateSemaphores()
{
    m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VK_CHECK_RESULT(
            vkCreateSemaphore(m_Device.GetVkDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]));
        VK_CHECK_RESULT(
            vkCreateSemaphore(m_Device.GetVkDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]));
        VK_CHECK_RESULT(vkCreateFence(m_Device.GetVkDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]));
    }
}

uint32_t VulkanSwapchain::AcquireNextImage()
{
    VK_CHECK_RESULT(vkWaitForFences(m_Device.GetVkDevice(), 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT32_MAX));
    VK_CHECK_RESULT(vkResetFences(m_Device.GetVkDevice(), 1, &m_InFlightFences[m_CurrentFrame]));

    // uint32_t imageIndex;
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_Device.GetVkDevice(), m_Swapchain, UINT32_MAX,
                                            m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        EM_CORE_WARN("Swapchain out of date; recreate swapchain!");
        return UINT32_MAX;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        EM_CORE_ASSERT(false, "Failed to acquire swapchain image!");

    EM_CORE_ASSERT(imageIndex < MAX_FRAMES_IN_FLIGHT, "Frame index out of bounds");

    m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return imageIndex;
}

void VulkanSwapchain::Cleanup()
{
    for (auto imageView : m_SwapchainImageViews)
        vkDestroyImageView(m_Device.GetVkDevice(), imageView, nullptr);

    m_SwapchainImageViews.clear();

    vkDestroySwapchainKHR(m_Device.GetVkDevice(), m_Swapchain, nullptr);
    m_Swapchain = VK_NULL_HANDLE;
}

VulkanSwapchain::SwapchainSupportDetails VulkanSwapchain::QuerySwapchainSupport(VkPhysicalDevice device,
                                                                                VkSurfaceKHR surface)
{
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.Capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        details.Formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.Formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        details.PresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.PresentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& format : availableFormats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& presentMode : availablePresentModes)
    {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            EM_CORE_INFO("Present Mode: Mailbox");
            return presentMode;
        }
    }
    EM_CORE_INFO("Present Mode: FIFO");
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width,
                                             uint32_t height)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }

    VkExtent2D actualExtent = {width, height};
    actualExtent.width =
        std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height =
        std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
    return actualExtent;
}

} // namespace Ember