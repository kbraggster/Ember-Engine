#include "Application.h"

int main()
{
    const auto app = CreateApplication();
    app->Run();
    delete app;
}