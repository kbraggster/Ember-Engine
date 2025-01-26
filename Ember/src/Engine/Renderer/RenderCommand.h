#pragma once

#include "RendererAPI.h"

namespace Ember
{

class RenderCommand
{
  public:
    static void Init() { s_RendererAPI->Init(); }

    static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        s_RendererAPI->SetViewport(x, y, width, height);
    }

  private:
    static Scope<RendererAPI> s_RendererAPI;
};

} // namespace Ember