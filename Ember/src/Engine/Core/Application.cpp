#include "Application.h"

Application* Application::s_Instance = nullptr;

Application::Application()
{
    s_Instance = this;
}

void Application::Run()
{
    while (m_Running)
    {
        // EM_CORE_TRACE("{0}", m_Running);
    }
}

Application* CreateApplication()
{
    return new Application();
}
