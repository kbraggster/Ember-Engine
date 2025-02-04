#pragma once

#include "Engine/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanDebugUtils.h"
#include "Platform/Vulkan/VulkanDevice.h"
#include "Platform/Vulkan/VulkanSwapchain.h"

struct GLFWwindow;

namespace Ember
{

class VulkanContext : public RendererContext
{
  public:
    VulkanContext();
    ~VulkanContext() override;

    void Init() override;

    Ref<VulkanDevice> GetDevice() { return m_Device; }
    Ref<VulkanSwapchain> GetSwapchain() { return m_Swapchain; }

    static VkInstance GetInstance() { return s_Instance; }

    static Ref<VulkanContext> Get() { return std::static_pointer_cast<VulkanContext>(Renderer::GetContext()); }

  private:
    VkInstance CreateInstance();
    void CreateSurface();

  private:
    inline static VkInstance s_Instance;

    GLFWwindow* m_WindowHandle;

    VkSurfaceKHR m_Surface;
    VulkanDebugUtils m_DebugUtils;
    Ref<VulkanDevice> m_Device;
    Ref<VulkanSwapchain> m_Swapchain;
};

} // namespace Ember