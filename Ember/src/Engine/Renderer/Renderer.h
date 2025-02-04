#pragma once

#include "RendererContext.h"
#include "Engine/Renderer/RendererAPI.h"

namespace Ember
{

class Application;
class Renderer
{
  public:
    static void Init();

    static Ref<RendererContext> GetContext();
    static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
};

} // namespace Ember