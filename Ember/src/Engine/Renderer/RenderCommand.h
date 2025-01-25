#pragma once

#include "RendererAPI.h"

namespace Ember
{

class RenderCommand
{
  public:
    static void Init() { s_RendererAPI->Init(); }

  private:
    static Scope<RendererAPI> s_RendererAPI;
};

} // namespace Ember