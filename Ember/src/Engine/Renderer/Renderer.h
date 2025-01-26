#pragma once

#include "RendererAPI.h"
#include "RenderCommand.h"

namespace Ember
{

class Renderer
{
  public:
    static void Init();
    // static void Shutdown();

    static void OnWindowResize(uint32_t width, uint32_t height);
    // static void BeginScene();
    // static void EndScene();

    // static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform =
    // glm::mat4(1.0f));

    static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
};

} // namespace Ember