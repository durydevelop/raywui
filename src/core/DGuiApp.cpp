#include "DGuiApp.h"
#include <raylib.h>
//#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <dpplib/DString.h>
#include <dpplib/DPath.h>
#include <chrono>
#include "raywui_log.h"

using namespace DTools;

const char TAG[8]="DGuiApp";

DGuiApp::DGuiApp(size_t ScreenWidth, size_t ScreenHeight, std::string AppTitle)
{
    Width=ScreenWidth;
    Height=ScreenHeight;
    Title=AppTitle;
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
    for (auto [Id,Widget] : Canvas.Dynamics) {
        delete Widget;
    }

    for (auto [Id,Widget] : Canvas.Statics) {
        delete Widget;
    }
}

bool DGuiApp::LoadCanvas(std::string CanvasFilename)
{
    DPreferences Json(CanvasFilename);
    if (!Json.IsReady()) {
        Log::error(TAG,"Canvas json file not ready: %s", Json.GetLastStatus().c_str());
        return false;
    }

    bool Done=true;
    
    // Load static widgets
    std::vector<DTree> List=Json.ReadArrayTrees(DJsonTree::SEC_STATIC);
    for (auto WidgetTree : List) {
        std::string Type=WidgetTree.ReadString(DJsonTree::ITEM_TYPE,"");
        if (DString::CmpNoCase(Type,DJsonTree::VALUE_JSON)) {
            // Widget from file
            if (!AddStaticFromFile(WidgetTree.ReadString(DJsonTree::ITEM_NAME,""))) {
                Done=false;
            }
        }
        else {
            // Widget from tree
            if (!AddStaticFromTree(std::ref(WidgetTree))) {
                Done=false;
            }
        }
    }

    // Load dinamic widgets
    List=Json.ReadArrayTrees(DJsonTree::SEC_DYNAMIC);
    for (auto WidgetTree : List) {
        std::string Type=WidgetTree.ReadString(DJsonTree::ITEM_TYPE,"");
        if (DString::CmpNoCase(Type,DJsonTree::VALUE_JSON)) {
            // Widget from file
            if (!AddDynamicFromFile(WidgetTree.ReadString(DJsonTree::ITEM_NAME,""))) {
                Done=false;
            }
        }
        else {
            // Widget from tree
            if (!AddDynamicFromTree(std::ref(WidgetTree))) {
                Done=false;
            }
        }
    }

    return Done;
}

bool DGuiApp::IsReady(void) {
    return IsWindowReady();
}

void DGuiApp::SetTitle(std::string AppTitle) {
    Title=AppTitle;
}

DGuiWidget* DGuiApp::SetCurrDynamic(DGuiWidget *Widget)
{
    CurrDynamic.Widget=Widget;
    if (Widget) {
        CurrDynamic.Name=Widget->Name;
    }
    else {
        CurrDynamic.Name.clear();
    }
    return Widget;
}

DGuiWidget* DGuiApp::SetCurrDynamic(std::string WidgetName)
{
    auto Widget=FindWidgetByName(WidgetName);
    if (!Widget) {
        Log::error(TAG,"Widget %s not found",WidgetName.c_str());
    }
    return SetCurrDynamic(Widget);
}

void DGuiApp::SetOnGuiEvent(OnGuiEventCallback Callback)
{
    GuiEventCallback=Callback;
    for (auto &[Id,Widget] : Canvas.Dynamics) {
        Widget->SetOnGuiEvent(Callback);
    }
    for (auto &[Id,Widget] : Canvas.Statics) {
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
    if (CurrDynamic.Widget) {
        ClearBackground(GetColor(CurrDynamic.Widget->Properties.BackGroundColor));
        EndDrawing();
    }
    else {
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    }
}
/*
DGuiContainer* DGuiApp::AddContainer(std::string ContainerName) {
    if (ContainerName.empty()) {
        ContainerName="Container"+std::to_string(Containers.size());
    }

    DGuiContainer *NewContainer=new DGuiContainer(0,0,Width,Height,nullptr);
    NewContainer->Name=ContainerName;
    return (AddContainer(NewContainer));
}
*/
DGuiWidget* DGuiApp::AddDynamic(DGuiWidget *NewWidget) {
    if (!NewWidget) {
        return nullptr;
    }
    NewWidget->SetOnGuiEvent(GuiEventCallback);
    Canvas.Dynamics.emplace(NewWidget->Name,NewWidget);
    if (Canvas.Dynamics.size() == 1) {
        // Set it to active one if it is alone
        SetCurrDynamic(NewWidget);
    }
    return NewWidget;
}

DGuiWidget* DGuiApp::AddDynamicFromFile(std::string JsonFilename) {
    DGuiWidget* NewWidget=DGuiWidget::New(JsonFilename);
    if (!NewWidget) {
        Log::error(TAG,"%s layout possibile incorrect",JsonFilename.c_str());
        return nullptr;
    }
    return AddDynamic(NewWidget);
}

DGuiWidget* DGuiApp::AddDynamicFromTree(DTree& WidgetTree) {
    DGuiWidget* NewWidget=DGuiWidget::New(WidgetTree);
    if (!NewWidget) {
        Log::error(TAG,"Cannot add layout from tree");
        return nullptr;
    }
    return AddDynamic(NewWidget);
}

DGuiWidget* DGuiApp::AddStatic(DGuiWidget *NewWidget) {
    if (!NewWidget) {
        return nullptr;
    }
    NewWidget->SetOnGuiEvent(GuiEventCallback);
    Canvas.Statics.emplace(NewWidget->Name,NewWidget);
    return NewWidget;
}

DGuiWidget* DGuiApp::AddStaticFromFile(std::string JsonFilename) {
    if (!DPath::Exists(JsonFilename)) {
        Log::error(TAG,"%s does not exist",JsonFilename.c_str());
        return nullptr;
    }
    DGuiWidget* NewWidget=DGuiWidget::New(JsonFilename);
    if (!NewWidget) {
        Log::error(TAG,"%s layout possibile incorrect",JsonFilename.c_str());
        return nullptr;
    }
    return AddStatic(NewWidget);
}

DGuiWidget* DGuiApp::AddStaticFromTree(DTree& WidgetTree) {
    DGuiWidget* NewWidget=DGuiWidget::New(WidgetTree);
    if (!NewWidget) {
        Log::error(TAG,"Cannot add layout from tree");
        return nullptr;
    }
    return AddStatic(NewWidget);
}

DGuiWidget* DGuiApp::FindWidgetByName(std::string WidgetName) {
    for (auto [Id,Widget] : Canvas.Dynamics) {
        if (Widget->Name == WidgetName) {
            return Widget;
        }
        auto w=Widget->FindWidgetByName(WidgetName);
        if (w) return w;
    }
    for (auto [Id,Widget] : Canvas.Statics) {
        if (Widget->Name == WidgetName) {
            return Widget;
        }
        auto w=Widget->FindWidgetByName(WidgetName);
        if (w) return w;
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

        if (CurrDynamic.Widget) {
            // Draw current dynamic widget
            //DLog::debug("ActiveContainer=%s",ActiveContainer->Name.c_str());
            ClearBackground(GetColor(CurrDynamic.Widget->Properties.BackGroundColor));
            CurrDynamic.Widget->Draw();
        }

        // Draw Static widgets
        for (auto& [Name,Widget] : Canvas.Statics) {
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
