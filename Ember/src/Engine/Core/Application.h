#pragma once

class Application
{
  public:
    Application();
    ~Application() = default;

    void Run();

    static Application& Get() { return *s_Instance; }

  private:
    bool m_Running = true;

    static Application* s_Instance;
};

Application* CreateApplication();