#pragma once

#include "vulkan/vulkan.h"

struct QueueFamilyIndices
{
    uint32_t GraphicsFamily;
    uint32_t PresentFamily;
    bool GraphicsFamilyHasValue = false;
    bool PresentFamilyHasValue  = false;
    bool IsComplete() const { return GraphicsFamilyHasValue && PresentFamilyHasValue; }
};

class VulkanDevice
{
  public:
    VulkanDevice(VkInstance& instance, VkSurfaceKHR& surface);
    ~VulkanDevice();

    VkDevice GetDevice() const { return m_Device; }
    VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
    VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
    VkQueue GetPresentQueue() const { return m_PresentQueue; }
    QueueFamilyIndices GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

  private:
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateCommandPool();
    bool IsDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(m_PhysicalDevice); }

    std::string DeviceTypeToString(VkPhysicalDeviceType type);
    std::string GetVendorName(const VkPhysicalDeviceProperties& props);

  private:
    VkInstance m_Instance;
    VkDevice m_Device;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

    VkSurfaceKHR m_Surface;
    VkCommandPool m_CommandPool;
    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;
    QueueFamilyIndices m_QueueFamilyIndices;
};