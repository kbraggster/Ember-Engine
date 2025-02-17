#include "ImGuiLayer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <Engine/Core/Application.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace Ember
{

ImGuiLayer::ImGuiLayer(Window& window)
    : Layer("ImGuiLayer"), m_Context(std::dynamic_pointer_cast<VulkanContext>(window.GetRenderContext()))
{
}

void ImGuiLayer::OnAttach()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular
    // ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    Application& app   = Application::Get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance                  = VulkanContext::GetInstance();
    init_info.PhysicalDevice            = m_Context->GetDevice()->GetVkPhysicalDevice();
    init_info.Device                    = m_Context->GetDevice()->GetVkDevice();
    init_info.QueueFamily               = m_Context->GetDevice()->GetQueueFamilyIndices().GraphicsFamily;
    init_info.Queue                     = m_Context->GetDevice()->GetVkGraphicsQueue();
    init_info.PipelineCache             = VK_NULL_HANDLE;
    init_info.DescriptorPool            = m_Context->GetDescriptorPool();
    init_info.RenderPass                = m_Context->GetRenderPass();
    init_info.Subpass                   = 0;
    init_info.MinImageCount             = 2;
    init_info.ImageCount                = 2;
    ImGui_ImplVulkan_Init(&init_info);
}

void ImGuiLayer::OnDetach()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::Begin()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::End()
{
    ImGuiIO& io      = ImGui::GetIO();
    Application& app = Application::Get();
    io.DisplaySize   = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

    // Render ImGui draw data
    ImGui::Render();

    uint32_t currentFrameIndex = m_Context->GetSwapchain()->AcquireNextImage();

    VkCommandBuffer commandBuffer = m_Context->GetCommandBuffer(currentFrameIndex);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = m_Context->GetRenderPass();
    renderPassInfo.framebuffer       = m_Context->GetFramebuffer(currentFrameIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_Context->GetSwapchain()->GetExtent();

    VkClearValue clearValue{};
    clearValue.color               = {{1.0f, 0.1f, 0.1f, 1.0f}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearValue;

    m_Context->BeginCommandBuffer(currentFrameIndex);

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Record Vulkan commands for ImGui rendering
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

    // End the render pass
    vkCmdEndRenderPass(commandBuffer);

    // Submit recorded command buffer
    m_Context->EndCommandBuffer(currentFrameIndex);
    m_Context->SubmitCommandBuffer(currentFrameIndex);

    // Handle platform windows rendering if enabled
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backupContext = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backupContext);
    }
}

void ImGuiLayer::OnImGuiRender()
{
    static bool show = true;
    ImGui::ShowDemoWindow(&show);
}

} // namespace Ember