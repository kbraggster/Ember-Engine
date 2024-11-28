#pragma once

#include <vulkan/vulkan.h>

class VulkanDebugUtils
{
  public:
    VulkanDebugUtils()  = default;
    ~VulkanDebugUtils() = default;

    void SetupDebugUtils(VkInstance& instance);
    void DestroyDebugUtils(VkInstance& instance, VkDebugUtilsMessengerEXT messenger, VkAllocationCallbacks* allocator);

    VkDebugUtilsMessengerEXT& GetDebugMessenger() { return m_DebugMessenger; }

  private:
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger);

  private:
    VkDebugUtilsMessengerEXT m_DebugMessenger;
};