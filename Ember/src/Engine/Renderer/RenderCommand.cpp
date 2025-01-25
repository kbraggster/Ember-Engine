#include "RenderCommand.h"

namespace Ember
{

Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

}