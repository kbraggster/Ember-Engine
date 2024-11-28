#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Engine/Renderer/GraphicsContext.h"
#include "Platform/Vulkan/VulkanDebugUtils.h"

class VulkanContext : public GraphicsContext
{
  public:
    explicit VulkanContext(GLFWwindow* windowHandle);
    ~VulkanContext() override;

    void Init() override;

  private:
    VkInstance CreateInstance();
    void CreateSurface(GLFWwindow* windowHandle);

  private:
    GLFWwindow* m_WindowHandle;

    VkInstance m_Instance;
    VulkanDebugUtils m_DebugUtils;
    VkSurfaceKHR m_Surface;
};