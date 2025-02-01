#pragma once

#include "Engine/Core/Window.h"

#include "Engine/Renderer/RendererContext.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace Ember
{

class MacOSWindow : public Window
{
  public:
    explicit MacOSWindow(const WindowProps& props);
    ~MacOSWindow() override;

    void OnUpdate() override;

    unsigned int GetWidth() const override { return m_Data.Width; }
    unsigned int GetHeight() const override { return m_Data.Height; }

    // Window attributes
    void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

    void* GetNativeWindow() const override { return m_Window; }

    Ref<RendererContext> GetRenderContext() override { return m_RendererContext; };

  private:
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();

  private:
    GLFWwindow* m_Window;
    Ref<RendererContext> m_RendererContext;

    struct WindowData
    {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;

        EventCallbackFn EventCallback;
    };

    WindowData m_Data;
};

} // namespace Ember