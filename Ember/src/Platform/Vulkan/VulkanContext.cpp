#include "VulkanContext.h"

#include "Vulkan.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <backends/imgui_impl_vulkan.h>

namespace Ember
{

VulkanContext::VulkanContext()
{
}

VulkanContext::~VulkanContext()
{
    if (!m_CommandBuffers.empty())
    {
        vkFreeCommandBuffers(m_Device->GetVkDevice(), m_Device->GetVkCommandPool(), (uint32_t)m_CommandBuffers.size(),
                             m_CommandBuffers.data());
        m_CommandBuffers.clear();
        EM_CORE_INFO("Command buffers destroyed");
    }

    if (!m_Framebuffers.empty())
    {
        for (auto framebuffer : m_Framebuffers)
            vkDestroyFramebuffer(m_Device->GetVkDevice(), framebuffer, nullptr);

        m_Framebuffers.clear();
        EM_CORE_INFO("Framebuffers destroyed");
    }

    vkDestroyDescriptorPool(m_Device->GetVkDevice(), m_DescriptorPool, nullptr);
    vkDestroyRenderPass(m_Device->GetVkDevice(), m_RenderPass, nullptr);

    m_Swapchain.reset();
    m_Device.reset();

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

    CreateSurface();

    m_Device.reset(new VulkanDevice(s_Instance, m_Surface));

    CreateDescriptorPool();
    CreateRenderPass();

    m_Swapchain.reset(new VulkanSwapchain(*m_Device, m_Surface));
    m_Swapchain->CreateSwapchain(2560, 1440);

    CreateCommandBuffers();
    CreateFramebuffers();
}

void VulkanContext::BeginCommandBuffer(uint32_t frameIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags            = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = nullptr;

    VkResult result = vkBeginCommandBuffer(m_CommandBuffers[frameIndex], &beginInfo);
    EM_CORE_ASSERT(result == VK_SUCCESS, "Failed to begin command buffer!");
}

void VulkanContext::SubmitCommandBuffer(uint32_t frameIndex)
{
    // Wait for the image to be available
    VkSemaphore waitSemaphores[]      = {m_Swapchain->GetImageAvailableSemaphore()};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    // Signal when the command buffer finishes executing
    VkSemaphore signalSemaphores[] = {m_Swapchain->GetImageRenderFinishedSemaphore()};

    // Set up the submit info
    VkSubmitInfo submitInfo{};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = waitSemaphores;
    submitInfo.pWaitDstStageMask    = waitStages;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &m_CommandBuffers[frameIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    // Submit the command buffer to the queue
    VkResult result = vkQueueSubmit(m_Device->GetVkGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    EM_CORE_ASSERT(result == VK_SUCCESS, "Failed to submit command buffer to queue!");

    // Present the swapchain image
    VkSwapchainKHR swapchain = m_Swapchain->GetSwapchain();
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &swapchain;
    presentInfo.pImageIndices      = &frameIndex;

    result = vkQueuePresentKHR(m_Device->GetVkGraphicsQueue(), &presentInfo);
    EM_CORE_ASSERT(result == VK_SUCCESS, "Failed to present swapchain image!");
}

void VulkanContext::EndCommandBuffer(uint32_t frameIndex)
{
    VK_CHECK_RESULT(vkEndCommandBuffer(m_CommandBuffers[frameIndex]));
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

void VulkanContext::CreateDescriptorPool()
{
    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE},
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets                    = 0;
    for (VkDescriptorPoolSize& pool_size : pool_sizes)
        pool_info.maxSets += pool_size.descriptorCount;
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes    = pool_sizes;
    VK_CHECK_RESULT(vkCreateDescriptorPool(m_Device->GetVkDevice(), &pool_info, nullptr, &m_DescriptorPool));
    EM_CORE_INFO("Vulkan descriptor pool created");
}

void VulkanContext::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format         = VK_FORMAT_B8G8R8A8_SRGB; // e.g., VK_FORMAT_B8G8R8A8_SRGB or your swapchain format
    colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;  // Clear the framebuffer at the start
    colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE; // Store the result after rendering
    colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0; // This references the first attachment
    colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments    = &colorAttachment;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies   = &dependency;

    VK_CHECK_RESULT(vkCreateRenderPass(m_Device->GetVkDevice(), &renderPassInfo, nullptr, &m_RenderPass));
    EM_CORE_INFO("Vulkan renderpass created");
}

void VulkanContext::CreateCommandBuffers()
{
    m_CommandBuffers.resize(m_Swapchain->GetImageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = m_Device->GetVkCommandPool();
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

    VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device->GetVkDevice(), &allocInfo, m_CommandBuffers.data()));
}

void VulkanContext::CreateFramebuffers()
{
    m_Framebuffers.resize(m_Swapchain->GetImageCount());

    for (size_t i = 0; i < m_Framebuffers.size(); i++)
    {
        VkImageView attachments[] = {m_Swapchain->GetImageViews()[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = m_RenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments    = attachments;
        framebufferInfo.width           = m_Swapchain->GetExtent().width;
        framebufferInfo.height          = m_Swapchain->GetExtent().height;
        framebufferInfo.layers          = 1;

        VK_CHECK_RESULT(vkCreateFramebuffer(m_Device->GetVkDevice(), &framebufferInfo, nullptr, &m_Framebuffers[i]));
    }
}

} // namespace Ember