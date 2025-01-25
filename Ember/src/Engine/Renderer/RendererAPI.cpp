#include "RendererAPI.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Ember
{

RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;

Scope<RendererAPI> RendererAPI::Create()
{
    switch (s_API)
    {
        case RendererAPI::API::None:
            EM_CORE_ASSERT(false, "RendererAPI::None is not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return CreateScope<VulkanRendererAPI>();
    }

    EM_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Ember