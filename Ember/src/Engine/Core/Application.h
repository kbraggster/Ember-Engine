#pragma once

#include "Window.h"

#include "LayerStack.h"

#include "Engine/ImGui/ImGuiLayer.h"
#include "Platform/Vulkan/VulkanContext.h"

class Application
{
  public:
    Application();
    ~Application() = default;

    void Run();

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    static Application& Get() { return *s_Instance; }

    Window& GetWindow() { return *m_Window; }

  private:
    Scope<Window> m_Window;
    bool m_Running = true;

    LayerStack m_LayerStack;
    ImGuiLayer* m_ImGuiLayer;

    static Application* s_Instance;
};

Application* CreateApplication();