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
        std::cout << m_Running << std::endl;
    }
}

Application* CreateApplication()
{
    return new Application();
}
