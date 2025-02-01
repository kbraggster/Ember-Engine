#pragma once

#include "Window.h"

#include "LayerStack.h"

#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/ImGui/ImGuiLayer.h"

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

    void Close();

    Window& GetWindow() { return *m_Window; }

    static Application& Get() { return *s_Instance; }

  private:
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);

  private:
    Scope<Window> m_Window;
    bool m_Running   = true;
    bool m_Minimized = false;

    LayerStack m_LayerStack;
    // ImGuiLayer* m_ImGuiLayer;

    static Application* s_Instance;

    friend class Renderer;
};

// To be defined in CLIENT
Application* CreateApplication();

} // namespace Ember