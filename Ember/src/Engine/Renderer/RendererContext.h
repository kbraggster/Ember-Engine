#pragma once

namespace Ember
{

class RendererContext
{
  public:
    RendererContext()          = default;
    virtual ~RendererContext() = default;

    virtual void Init() = 0;

    static Ref<RendererContext> Create();
};

} // namespace Ember
