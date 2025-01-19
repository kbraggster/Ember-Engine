#pragma once

#include "Window.h"

#include "LayerStack.h"

class Application
{
  public:
    Application();
    ~Application() = default;

    void Run();

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    static Application& Get() { return *s_Instance; }

  private:
    Scope<Window> m_Window;
    bool m_Running = true;

    LayerStack m_LayerStack;

    static Application* s_Instance;
};

Application* CreateApplication();