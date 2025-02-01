#include "VulkanContext.h"

#include "Vulkan.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Ember
{

VulkanContext::VulkanContext()
{
}

VulkanContext::~VulkanContext()
{
    m_Swapchain.reset();
    // m_Device.reset();

    vkDestroySurfaceKHR(s_Instance, m_Surface, nullptr);
    m_Surface = VK_NULL_HANDLE;

    m_DebugUtils.DestroyDebugUtils(s_Instance, m_DebugUtils.GetDebugMessenger(), nullptr);

    vkDestroyInstance(s_Instance, nullptr);
    s_Instance = VK_NULL_HANDLE;
}

void VulkanContext::Init()
{
    s_Instance = CreateInstance();
    EM_CORE_INFO("Vulkan instance created");

    m_DebugUtils.SetupDebugUtils(s_Instance);

    // CreateSurface();

    m_Device.reset(new VulkanDevice(s_Instance, m_Surface));

    // m_Swapchain.reset(new VulkanSwapchain(*m_Device, m_Surface));
    // m_Swapchain->CreateSwapchain(2560, 1440);
}

VkInstance VulkanContext::CreateInstance()
{
    // Vulkan application info
    VkApplicationInfo appInfo  = {};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = "Ember Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "Ember";
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_0;

    // Vulkan instance info
    VkInstanceCreateInfo createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    // TODO: Platform specific extensions
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extensions.push_back("VK_MVK_macos_surface");
    extensions.push_back("VK_KHR_get_physical_device_properties2");

    createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    std::vector<const char*> layers;
    layers.push_back("VK_LAYER_KHRONOS_validation");

    createInfo.enabledLayerCount   = layers.size();
    createInfo.ppEnabledLayerNames = layers.data();

    VkInstance instance;
    const VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    EM_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan instance!");

    return instance;
}

void VulkanContext::CreateSurface()
{
    if (glfwCreateWindowSurface(s_Instance, m_WindowHandle, nullptr, &m_Surface))
        EM_CORE_ERROR("Error creating window surface!");
}

} // namespace Ember