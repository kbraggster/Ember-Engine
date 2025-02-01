#pragma once

namespace Ember
{

class RendererAPI
{
  public:
    enum class API
    {
        None   = 0,
        Vulkan = 1
    };

  public:
    virtual ~RendererAPI() = default;

    virtual void Init() = 0;

    static API GetAPI() { return s_API; }
    static void SetAPI(API api);

  private:
    inline static API s_API = API::Vulkan;
};

} // namespace Ember