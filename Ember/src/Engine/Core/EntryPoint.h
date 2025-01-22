#pragma once

#include "Base.h"
#include "Application.h"

extern Application* CreateApplication();

int main(int argc, char** argv)
{

    Log::Init();
    EM_CORE_INFO("Initialized Log");

    const auto app = CreateApplication();
    app->Run();
    delete app;
}