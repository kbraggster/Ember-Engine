#pragma once

#include "Window.h"

class Application
{
  public:
    Application();
    ~Application() = default;

    void Run();

    static Application& Get() { return *s_Instance; }

  private:
    Scope<Window> m_Window;
    bool m_Running = true;

    static Application* s_Instance;
};

Application* CreateApplication();