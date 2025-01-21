#include "ImGuiLayer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <Engine/Core/Application.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

ImGuiLayer::ImGuiLayer(const Window& window)
    : Layer("ImGuiLayer"), m_Context(std::static_pointer_cast<VulkanContext>(window.GetContext()))
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
    init_info.Instance                  = m_Context->GetInstance();
    init_info.PhysicalDevice            = m_Context->GetDevice().GetPhysicalDevice();
    init_info.Device                    = m_Context->GetDevice().GetVkDevice();
    init_info.QueueFamily               = m_Context->GetDevice().GetQueueFamilyIndices().GraphicsFamily;
    init_info.Queue                     = m_Context->GetDevice().GetGraphicsQueue();
    // init_info.PipelineCache             = VK_NULL_HANDLE;
    // init_info.DescriptorPool = m_Context->GetDescriptorPool();
    // init_info.RenderPass     = m_Context->GetSwapchain().GetRenderPass();
    // init_info.Subpass        = 0;
    // init_info.MinImageCount  = m_Context->GetSwapchain().GetMinImageCount();
    // init_info.ImageCount     = m_Context->GetSwapchain().GetImageCount();
    // init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    // init_info.Allocator = g_Allocator;
    // init_info.CheckVkResultFn = check_vk_result;
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
    // m_Context->BeginCommandBuffer();
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::End()
{
    ImGuiIO& io      = ImGui::GetIO();
    Application& app = Application::Get();
    io.DisplaySize   = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

    // Rendering
    ImGui::Render();

    // Record Vulkan commands for ImGui rendering
    // ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_Context->GetCommandBuffer());

    // Clear the screen with the desired color (in the RenderPass setup)
    // m_Context->EndCommandBuffer();
    // m_Context->SubmitCommandBuffer();

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
