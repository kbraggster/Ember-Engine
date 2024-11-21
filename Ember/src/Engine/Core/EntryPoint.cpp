#include "Application.h"

int main()
{
    Log::Init();
    EM_CORE_INFO("Initialized Log");

    const auto app = CreateApplication();
    app->Run();
    delete app;
}