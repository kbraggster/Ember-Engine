#pragma once

class GraphicsContext
{
  public:
    virtual ~GraphicsContext() = default;
    virtual void Init()        = 0;
};