#pragma once

#include "Window.h"

#include "LayerStack.h"

#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/ImGui/ImGuiLayer.h"
#include "Platform/Vulkan/VulkanContext.h"

int main(int argc, char** argv);

namespace Ember
{

class Application
{
  public:
    Application();
    ~Application() = default;

    void Run();

    void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    static Application& Get() { return *s_Instance; }

    Window& GetWindow() { return *m_Window; }

    void Close();

  private:
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);

  private:
    Scope<Window> m_Window;
    bool m_Running   = true;
    bool m_Minimized = false;

    LayerStack m_LayerStack;
    ImGuiLayer* m_ImGuiLayer;

    static Application* s_Instance;
};

// To be defined in CLIENT
Application* CreateApplication();

} // namespace Ember