#include "VulkanDevice.h"

VulkanDevice::VulkanDevice(VkInstance& instance, VkSurfaceKHR& surface) : m_Instance(instance), m_Surface(surface)
{
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateCommandPool();
}

VulkanDevice::~VulkanDevice()
{
    if (m_Device != VK_NULL_HANDLE)
    {
        if (m_CommandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
            m_CommandPool = VK_NULL_HANDLE;
        }

        vkDestroyDevice(m_Device, nullptr);
        m_Device = VK_NULL_HANDLE;
    }
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
            m_PhysicalDevice     = device;
            m_QueueFamilyIndices = FindQueueFamilies(device);
            break;
        }
    }

    EM_CORE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU!");

    // Query and log physical device details
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &deviceFeatures);

    EM_CORE_INFO("Vulkan Info:");
    EM_CORE_INFO("  API Version: {0}.{1}.{2}", VK_VERSION_MAJOR(deviceProperties.apiVersion),
                 VK_VERSION_MINOR(deviceProperties.apiVersion), VK_VERSION_PATCH(deviceProperties.apiVersion));
    EM_CORE_INFO("  Device Vendor: {0}", GetVendorName(deviceProperties));
    EM_CORE_INFO("  Device Name: {0}", deviceProperties.deviceName);
    EM_CORE_INFO("  Device Type: {0}", DeviceTypeToString(deviceProperties.deviceType));
    EM_CORE_INFO("  Driver Version: {0}", deviceProperties.driverVersion);
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

    EM_CORE_INFO("Logical device created");
}

void VulkanDevice::CreateCommandPool()
{
    QueueFamilyIndices indices = FindPhysicalQueueFamilies();

    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex        = indices.GraphicsFamily;
    createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkResult result = vkCreateCommandPool(m_Device, &createInfo, nullptr, &m_CommandPool);
    EM_CORE_ASSERT(result == VK_SUCCESS, "Failed to create command pool!");
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

std::string VulkanDevice::DeviceTypeToString(VkPhysicalDeviceType type)
{
    switch (type)
    {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return "Integrated GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return "Discrete GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return "Virtual GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return "CPU";
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return "Other";
        default:
            return "Unknown";
    }
}

std::string VulkanDevice::GetVendorName(const VkPhysicalDeviceProperties& props)
{
    switch (props.vendorID)
    {
        case 0x1022:
            return "AMD";
        case 0x10DE:
            return "NVIDIA";
        case 0x8086:
            return "Intel";
        case 0x106B:
            return "Apple";
        default:
            return "Unknown Vendor";
    }
}
