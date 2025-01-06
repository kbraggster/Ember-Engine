#include "VulkanDevice.h"

VulkanDevice::VulkanDevice(VkInstance& instance, VkSurfaceKHR& surface) : m_Instance(instance), m_Surface(surface)
{
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateCommandPool();
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
    vkDestroyDevice(m_Device, nullptr);
}

void VulkanDevice::PickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
    EM_CORE_ASSERT(deviceCount != 0, "Failed to find GPUs with Vulkan support!");

    EM_CORE_INFO("Device Count: {0}", deviceCount);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            m_PhysicalDevice = device;
            break;
        }
    }

    EM_CORE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU!");
}

void VulkanDevice::CreateLogicalDevice()
{
    QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.GraphicsFamily, indices.PresentFamily};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex        = queueFamily;
        queueCreateInfo.queueCount              = 1;
        queueCreateInfo.pQueuePriorities        = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy        = VK_TRUE;

    std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset"};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType              = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos       = queueCreateInfos.data();
    createInfo.pEnabledFeatures        = &deviceFeatures;
    createInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);
    EM_CORE_ASSERT(result == VK_SUCCESS, "Failed to create logical device!");

    vkGetDeviceQueue(m_Device, indices.GraphicsFamily, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, indices.PresentFamily, 0, &m_PresentQueue);

    EM_CORE_INFO("Logical device created!");
}

void VulkanDevice::CreateCommandPool()
{
    QueueFamilyIndices indices = FindPhysicalQueueFamilies();

    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex        = indices.GraphicsFamily;
    createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    EM_CORE_ASSERT(vkCreateCommandPool(m_Device, &createInfo, nullptr, &m_CommandPool) == VK_SUCCESS,
                   "Failed to create command pool!");
}

bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = FindQueueFamilies(device);

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    bool isComplete    = indices.IsComplete();
    bool hasAnisotropy = supportedFeatures.samplerAnisotropy;

    EM_CORE_INFO("Device suitability: QueueComplete = {0}, Anisotropy = {1}", isComplete, hasAnisotropy);

    return isComplete && hasAnisotropy;
}

QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    uint32_t i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.GraphicsFamily         = i;
            indices.GraphicsFamilyHasValue = true;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
        if (queueFamily.queueCount > 0 && presentSupport)
        {
            indices.PresentFamily         = i;
            indices.PresentFamilyHasValue = true;
        }
        if (indices.IsComplete())
            break;

        i++;
    }

    return indices;
}