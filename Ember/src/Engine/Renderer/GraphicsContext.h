#pragma once

namespace Ember
{

class GraphicsContext
{
  public:
    virtual ~GraphicsContext() = default;

    virtual void Init() = 0;

    static Scope<GraphicsContext> Create(void* window);
};

} // namespace Ember