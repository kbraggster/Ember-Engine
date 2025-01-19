#include "Application.h"

Application* Application::s_Instance = nullptr;

Application::Application()
{
    s_Instance = this;

    m_Window.reset(Window::Create());
}

void Application::Run()
{
    while (m_Running)
    {
        for (Layer* layer : m_LayerStack)
            layer->OnUpdate();

        // m_ImGuiLayer->Begin();
        // for (Layer* layer : m_LayerStack)
        //     layer->OnImGuiRender();
        // m_ImGuiLayer->End();

        m_Window->OnUpdate();
    }
}

void Application::PushLayer(Layer* layer)
{
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer* layer)
{
    m_LayerStack.PushOverlay(layer);
}

Application* CreateApplication()
{
    return new Application();
}
