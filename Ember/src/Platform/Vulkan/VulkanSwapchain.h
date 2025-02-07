#pragma once

#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Ember
{

class VulkanSwapchain
{
  public:
    VulkanSwapchain(const VulkanDevice& device, const VkSurfaceKHR& surface);
    ~VulkanSwapchain();

    void CreateSwapchain(uint32_t width, uint32_t height);
    void CreateSemaphores();

    uint32_t AcquireNextImage();

    VkSwapchainKHR GetSwapchain() const { return m_Swapchain; }
    uint32_t GetImageCount() const { return static_cast<uint32_t>(m_SwapchainImages.size()); }
    const std::vector<VkImageView>& GetImageViews() const { return m_SwapchainImageViews; }
    VkExtent2D GetExtent() const { return m_SwapchainExtent; }
    VkSemaphore GetImageAvailableSemaphore() const { return m_ImageAvailableSemaphore; }
    VkSemaphore GetImageRenderFinishedSemaphore() const { return m_RenderFinishedSemaphore; }

  private:
    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };
    SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

  private:
    VulkanDevice m_Device;
    VkSurfaceKHR m_Surface;

    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_SwapchainImages;
    std::vector<VkImageView> m_SwapchainImageViews;

    VkSemaphore m_ImageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore m_RenderFinishedSemaphore = VK_NULL_HANDLE;

    VkFormat m_SwapchainImageFormat;
    VkExtent2D m_SwapchainExtent;
};

} // namespace Ember