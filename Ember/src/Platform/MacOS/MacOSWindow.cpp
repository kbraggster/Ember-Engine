#include "MacOSWindow.h"

#include "Engine/Events/ApplicationEvent.h"

#include "Platform/Vulkan/VulkanContext.h"

namespace Ember
{

static bool s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char* description)
{
    EM_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

Window* Window::Create(const WindowProps& props)
{
    return new MacOSWindow(props);
}

MacOSWindow::MacOSWindow(const WindowProps& props)
{
    MacOSWindow::Init(props);
}

MacOSWindow::~MacOSWindow()
{
    MacOSWindow::Shutdown();
}

void MacOSWindow::Init(const WindowProps& props)
{
    m_Data.Title  = props.Title;
    m_Data.Width  = props.Width;
    m_Data.Height = props.Height;

    EM_CORE_INFO("Creating Window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    if (!s_GLFWInitialized)
    {
        // TODO: glfwTerminate on system shutdown
        int success = glfwInit();
        EM_CORE_ASSERT(success, "Could not initialize GLFW!");
        glfwSetErrorCallback(GLFWErrorCallback);

        s_GLFWInitialized = true;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height), m_Data.Title.c_str(),
                                nullptr, nullptr);

    m_RendererContext = RendererContext::Create();
    m_RendererContext->Init();

    glfwSetWindowUserPointer(m_Window, &m_Data);
    SetVSync(true);

    // Set GLFW callbacks
    // TODO: GLFW callbacks?
    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
        WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

        data.Width  = width;
        data.Height = height;

        // WindowResizeEvent event(width, height);
        // data.EventCallback(event);
    });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
        WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        WindowCloseEvent event;
        data.EventCallback(event);
    });
}

void MacOSWindow::Shutdown()
{
    glfwDestroyWindow(m_Window);
}

void MacOSWindow::OnUpdate()
{
    glfwPollEvents();
    // glfwSwapBuffers(m_Window);
}

void MacOSWindow::SetVSync(const bool enabled)
{
    m_Data.VSync = enabled;
}

bool MacOSWindow::IsVSync() const
{
    return m_Data.VSync;
}

} // namespace Ember