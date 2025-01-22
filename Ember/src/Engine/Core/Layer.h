#pragma once

namespace Ember
{

class Layer
{
  public:
    Layer(const std::string& name = "Layer");
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate() {}
    virtual void OnImGuiRender() {}

    const std::string& GetName() const { return m_DebugName; }

  private:
    std::string m_DebugName;
};

} // namespace Ember