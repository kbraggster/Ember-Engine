#pragma once

#include "VulkanDebugUtils.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

#include "Engine/Renderer/Renderer.h"

struct GLFWwindow;

namespace Ember
{

class VulkanContext : public RendererContext
{
  public:
    VulkanContext();
    ~VulkanContext() override;

    void Init() override;

    void SetWindowHandle(GLFWwindow* window) override { m_WindowHandle = window; }

    void BeginCommandBuffer(uint32_t frameIndex);
    void SubmitCommandBuffer(uint32_t frameIndex);
    void EndCommandBuffer(uint32_t frameIndex);

    Ref<VulkanDevice> GetDevice() { return m_Device; }
    Ref<VulkanSwapchain> GetSwapchain() { return m_Swapchain; }

    VkDescriptorPool GetDescriptorPool() const { return m_DescriptorPool; }
    VkRenderPass GetRenderPass() const { return m_RenderPass; }
    VkCommandBuffer GetCommandBuffer(uint32_t frameIndex) const { return m_CommandBuffers[frameIndex]; }
    VkFramebuffer GetFramebuffer(uint32_t frameIndex) const { return m_Framebuffers[frameIndex]; }

    static VkInstance GetInstance() { return s_Instance; }
    static Ref<VulkanContext> Get() { return std::static_pointer_cast<VulkanContext>(Renderer::GetContext()); }

  private:
    VkInstance CreateInstance();
    void CreateSurface();
    void CreateDescriptorPool();
    void CreateRenderPass();
    void CreateCommandBuffers();
    void CreateFramebuffers();

    void RecreateSwapchain();

  private:
    inline static VkInstance s_Instance;

    GLFWwindow* m_WindowHandle;

    VkSurfaceKHR m_Surface;
    VkDescriptorPool m_DescriptorPool;
    VkRenderPass m_RenderPass;
    std::vector<VkCommandBuffer> m_CommandBuffers;
    std::vector<VkFramebuffer> m_Framebuffers;
    VulkanDebugUtils m_DebugUtils;
    Ref<VulkanDevice> m_Device;
    Ref<VulkanSwapchain> m_Swapchain;
};

} // namespace Ember