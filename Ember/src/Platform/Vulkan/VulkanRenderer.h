#pragma once

#include "Engine/Renderer/RendererAPI.h"

namespace Ember
{

class VulkanRenderer : public RendererAPI
{
  public:
    void Init() override;
};

} // namespace Ember