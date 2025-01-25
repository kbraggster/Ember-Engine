#pragma once

#include "Engine/Renderer/RendererAPI.h"

namespace Ember
{

class VulkanRendererAPI : public RendererAPI
{
  public:
    void Init() override;
    // void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
};

} // namespace Ember