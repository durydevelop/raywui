#include "DGuiApp.h"
#include <raylib.h>
//#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <DGuiContainer.h>
#include <chrono>
#include "raywui_log.h"

const char TAG[8]="DGuiApp";

DGuiApp::DGuiApp(size_t ScreenWidth, size_t ScreenHeight, std::string AppTitle)
{
    Width=ScreenWidth;
    Height=ScreenHeight;
    Title=AppTitle;
    ActiveContainer=nullptr;
    Running=false;
    GuiEventCallback=nullptr;
    AppStartedCallback=nullptr;

    InitWindow(Width,Height,Title.c_str());
    /*
    auto cm=GetCurrentMonitor();
    Width=GetMonitorWidth(cm);
    Height=GetMonitorHeight(cm);
    SetWindowSize(Width,Height);
    */
};

DGuiApp::~DGuiApp()
{
    Log::debug(TAG,"~DGuiApp()");
    // Close window and OpenGL context
    CloseWindow();
    for (auto [id,widget] : StaticWidgets) {
        delete widget;
    }

    for (auto [id,container] : Containers) {
        delete container;
    }
};

bool DGuiApp::IsReady(void) {
    return IsWindowReady();
}

void DGuiApp::SetTitle(std::string AppTitle) {
    Title=AppTitle;
}

DGuiContainer* DGuiApp::SetActiveContainer(DGuiContainer *Container)
{
    ActiveContainer=Container;
    return ActiveContainer;
}

DGuiContainer* DGuiApp::SetActiveContainer(std::string ContainerName)
{
    auto Container=GetContainerFromName(ContainerName);
    if (!Container) {
        Log::error(TAG,"Container %s not found",ContainerName.c_str());
        return nullptr;
    }
    ActiveContainer=Container;
    return ActiveContainer;
}

void DGuiApp::SetOnGuiEvent(OnGuiEventCallback Callback)
{
    GuiEventCallback=Callback;
    for (auto &[Id,Container] : Containers) {
        Container->SetOnGuiEvent(Callback);
    }
    for (auto &[Id,Widget] : StaticWidgets) {
        Widget->SetOnGuiEvent(Callback);
    }
}

void DGuiApp::SetOnAppStarted(std::function<void (void)> Callback) {
    AppStartedCallback=Callback;
}

void DGuiApp::SetOnAppStopped(std::function<void (void)> Callback) {
    AppStoppedCallback=Callback;
}

void DGuiApp::SetOnTick(std::function<void (void)> Callback) {
    TickCallback=Callback;
}

void DGuiApp::ClearScreen(void)
{
    if (ActiveContainer) {
        ClearBackground(GetColor(ActiveContainer->Properties.BackGroundColor));
        EndDrawing();
    }
    else {
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    }
}

DGuiContainer* DGuiApp::AddContainer(std::string ContainerName) {
    if (ContainerName.empty()) {
        ContainerName="Container"+std::to_string(Containers.size());
    }

    DGuiContainer *NewContainer=new DGuiContainer(0,0,Width,Height,nullptr);
    NewContainer->Name=ContainerName;
    return (AddContainer(NewContainer));
}

DGuiContainer* DGuiApp::AddContainer(DGuiContainer *NewContainer) {
    if (!NewContainer) {
        return nullptr;
    }
    Containers.emplace(NewContainer->GetId(),NewContainer);
    if (Containers.size() == 1) {
        // Set it to active one if it is alone
        ActiveContainer=NewContainer;
    }
    NewContainer->SetOnGuiEvent(GuiEventCallback);
    return NewContainer;
}

DGuiContainer* DGuiApp::AddContainerFromFile(std::string JsonFilename) {
    DGuiContainer* NewContainer=new DGuiContainer(JsonFilename,nullptr);
    AddContainer(NewContainer);
    return NewContainer;
}

DGuiWidget* DGuiApp::AddStaticWidget(DGuiWidget *NewWidget) {
    if (!NewWidget) {
        return nullptr;
    }
    NewWidget->SetOnGuiEvent(GuiEventCallback);

    StaticWidgets.emplace(NewWidget->Name,NewWidget);
    return NewWidget;
}

DGuiWidget* DGuiApp::AddStaticWidgetFromFile(std::string JsonFilename) {
    DGuiWidget* Widget=DGuiWidget::New(JsonFilename);
    if (!Widget) {
        Log::error(TAG,"%s layout possibile incorrect",JsonFilename.c_str());
        return nullptr;
    }
    Widget->SetOnGuiEvent(GuiEventCallback);

    StaticWidgets.emplace(Widget->Name,Widget);
    return Widget;
}

DGuiContainer* DGuiApp::GetContainerFromName(std::string ContainerName) {
    for (auto [Id,Container] : Containers) {
        if (Container->Name == ContainerName) {
            return Container;
        }
    }
    return nullptr;
}

DResult DGuiApp::Run(void)
{
    DResult Result;
    #ifdef NET_DRIVER_EM
        if (Width == 0 || Height == 0) {
            Result.SetError("In WASM context, Width or Height cannot be 0");
            return Result;    
        }
    #endif

    if (!IsWindowReady()) {
        Result.SetError("Cannot run, OpenGL context was not initialized correctly");
        return Result;
    }
    
    SetTargetFPS(60);
    Running=true;
    if (AppStartedCallback) {
        AppStartedCallback();
    }

    // Main loop (detect window close button or ESC key)
    while (!WindowShouldClose()) {
        //auto tick=std::chrono::system_clock::now();
        if (TickCallback) {
            TickCallback();
        }
        if (ActiveContainer) {
            //DLog::debug("ActiveContainer=%s",ActiveContainer->Name.c_str());
            ClearBackground(GetColor(ActiveContainer->Properties.BackGroundColor));
            // Draw container
            ActiveContainer->Draw();
        }
        // Draw Static widgets
        for (auto [Name,Widget] : StaticWidgets) {
            Widget->Draws();
        }
        EndDrawing();
    }

    Running=false;

    if (AppStoppedCallback) {
        AppStoppedCallback();
    }
    
    return Result;
};
