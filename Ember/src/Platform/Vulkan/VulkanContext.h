#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Engine/Renderer/GraphicsContext.h"

class VulkanContext : public GraphicsContext
{
  public:
    explicit VulkanContext(const GLFWwindow* windowHandle);
    ~VulkanContext() override;

    void Init() override;

    VkInstance CreateInstance();

  private:
    const GLFWwindow* m_WindowHandle;

    VkInstance m_Instance;
};