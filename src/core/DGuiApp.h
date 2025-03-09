#ifndef DGuiApp_H
#define DGuiApp_H

#include "DGuiCommon.h"
#include <DGuiWidget.h>
#include <dpplib/DPreferences.h>
#include <functional>
#include <string>
#include <memory>

class DGuiContainer;

typedef struct DResult {
    enum ResultStates { RESULT_ERROR=-1, RESULT_WARNING, RESULT_OK };
    int Status=RESULT_OK;
    std::string Message;

    void SetError(std::string ErrorMessage) {
        Status=RESULT_ERROR;
        Message=ErrorMessage;
    }

    bool IsOk(void) {
        return Status == RESULT_OK;
    }

    bool IsErr(void) {
        return Status == RESULT_ERROR;
    }

    bool IsWarning(void) {
        return Status == RESULT_WARNING;
    }
} DResult;

class DCanvas {
    public:
        std::map<std::string,DGuiWidget*> Dynamics;
        std::map<std::string,DGuiWidget*> Statics;
        
    private:

};

class DGuiApp
{
    public:
        DGuiApp(size_t ScreenWidth = 0, size_t ScreenHeight = 0, std::string AppTitle = "");
        ~DGuiApp();

        void SetTitle(std::string AppTitle);
        bool LoadCanvas(std::string CanvasFilename);

        //DGuiWidget* AddDynamic(std::string WidgetName);
        DGuiWidget* AddDynamic(DGuiWidget *NewWidget);
        DGuiWidget* AddDynamicFromFile(std::string LayoutFilename);
        DGuiWidget* AddDynamicFromTree(DTools::DTree& WidgetTree);
        DGuiWidget* AddStatic(DGuiWidget *NewWidget);
        DGuiWidget* AddStaticFromFile(std::string LayoutFilename);
        DGuiWidget* AddStaticFromTree(DTools::DTree& WidgetTree);
        DGuiWidget* SetCurrDynamic(DGuiWidget *Widget);
        DGuiWidget* SetCurrDynamic(std::string WidgetName);
        DGuiWidget* FindWidgetByName(std::string WidgetName);

        void ClearScreen(void);
        DResult Run(void);
        bool IsReady(void);

        // Events
        void SetOnGuiEvent(OnGuiEventCallback Callback);
        void SetOnAppStarted(std::function<void (void)> Callback = 0);
        void SetOnAppStopped(std::function<void (void)> Callback = 0);
        void SetOnTick(std::function<void (void)> Callback = 0);

        bool Running;
        //bool ShouldClearScreen;   //! If true screeen will be cleared at next draw
        int Width;
        int Height;
        std::string Title;
        struct DCurrWidget {
            DGuiWidget *Widget=nullptr;
            std::string Name;
        }CurrDynamic;
        //DCurrWidget CurrWidget;
        DCanvas Canvas;
        
    private:
        // Event callbacks
        OnGuiEventCallback GuiEventCallback;
        std::function<void (void)> AppStartedCallback;
        std::function<void (void)> AppStoppedCallback;
        std::function<void (void)> TickCallback;
};

#endif