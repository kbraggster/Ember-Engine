#include "GraphicsContext.h"

#include "Engine/Renderer/Renderer.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace Ember
{

Scope<GraphicsContext> GraphicsContext::Create(void* window)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            EM_CORE_ASSERT(false, "RendererAPI::None is not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return CreateScope<VulkanContext>(static_cast<GLFWwindow*>(window));
    }

    EM_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Ember