#pragma once

#include "Base.h"
#include "Application.h"

extern Ember::Application* Ember::CreateApplication();

int main(int argc, char** argv)
{
    Ember::Log::Init();
    EM_CORE_INFO("Initialized Log");

    const auto app = Ember::CreateApplication();
    app->Run();
    delete app;
}