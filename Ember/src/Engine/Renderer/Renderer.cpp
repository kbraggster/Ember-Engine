#include "Renderer.h"

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanRenderer.h"

#include "Engine/Core/Application.h"

namespace Ember
{

static RendererAPI* s_RendererAPI = nullptr;

static RendererAPI* InitRendererAPI()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            break;
        case RendererAPI::API::Vulkan:
            return new VulkanRenderer();
    }
    EM_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

void Renderer::Init()
{
    s_RendererAPI = InitRendererAPI();

    s_RendererAPI->Init();
}

Ref<RendererContext> Renderer::GetContext()
{
    return Application::Get().GetWindow().GetRenderContext();
}

void RendererAPI::SetAPI(API api)
{
    s_API = api;
}

} // namespace Ember