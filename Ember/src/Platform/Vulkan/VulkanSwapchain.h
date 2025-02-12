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

    void Cleanup();

    VkSwapchainKHR GetSwapchain() const { return m_Swapchain; }
    uint32_t GetImageCount() const { return static_cast<uint32_t>(m_SwapchainImages.size()); }
    const std::vector<VkImageView>& GetImageViews() const { return m_SwapchainImageViews; }
    VkExtent2D GetExtent() const { return m_SwapchainExtent; }
    std::vector<VkSemaphore> GetImageAvailableSemaphores() const { return m_ImageAvailableSemaphores; }
    std::vector<VkSemaphore> GetImageRenderFinishedSemaphores() const { return m_RenderFinishedSemaphores; }
    std::vector<VkFence> GetInFlightFences() const { return m_InFlightFences; }

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

    const uint32_t MAX_FRAMES_IN_FLIGHT = 3;
    size_t m_CurrentFrame               = 0;
    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;

    VkFormat m_SwapchainImageFormat;
    VkExtent2D m_SwapchainExtent;
};

} // namespace Ember