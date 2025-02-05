#pragma once

struct GLFWwindow;

namespace Ember
{

class RendererContext
{
  public:
    RendererContext()          = default;
    virtual ~RendererContext() = default;

    virtual void Init()                              = 0;
    virtual void SetWindowHandle(GLFWwindow* window) = 0;

    static Ref<RendererContext> Create();
};

} // namespace Ember
