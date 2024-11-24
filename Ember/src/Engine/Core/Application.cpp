#include "Application.h"

Application* Application::s_Instance = nullptr;

Application::Application()
{
    s_Instance = this;

    m_Window = std::unique_ptr<Window>(Window::Create());
}

void Application::Run()
{
    while (m_Running)
    {
        // EM_CORE_TRACE("{0}", m_Running);
        m_Window->OnUpdate();
    }
}

Application* CreateApplication()
{
    return new Application();
}
