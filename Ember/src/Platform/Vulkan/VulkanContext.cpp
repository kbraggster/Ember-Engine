#include "VulkanContext.h"

VulkanContext::VulkanContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
{
    EM_ASSERT(windowHandle, "Window handle is null");
}

VulkanContext::~VulkanContext()
{
    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    m_DebugUtils.DestroyDebugUtils(m_Instance, m_DebugUtils.GetDebugMessenger(), nullptr);
    vkDestroyInstance(m_Instance, nullptr);
}

void VulkanContext::Init()
{
    m_Instance = CreateInstance();
    EM_CORE_INFO("Vulkan instance created");

    m_DebugUtils.SetupDebugUtils(m_Instance);

    CreateSurface(m_WindowHandle);
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

void VulkanContext::CreateSurface(GLFWwindow* windowHandle)
{
    if (glfwCreateWindowSurface(m_Instance, windowHandle, nullptr, &m_Surface))
        EM_CORE_ERROR("Error creating window surface!");
}