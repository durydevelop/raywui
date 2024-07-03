#ifndef DGuiApp_H
#define DGuiApp_H

#include "DGuiCommon.h"
#include <DGuiWidget.h>
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

class DGuiApp
{
    public:
        //typedef std::function<void (void)> AppEvent;
        DGuiApp(size_t ScreenWidth = 0, size_t ScreenHeight = 0, std::string AppTitle = "");
        ~DGuiApp();

        void SetTitle(std::string AppTitle);

        DGuiContainer* AddContainer(std::string ContainerName);
        DGuiContainer* AddContainer(DGuiContainer *NewContainer);
        DGuiContainer* AddContainerFromFile(std::string LayoutFilename);
        DGuiWidget* AddStaticWidget(DGuiWidget *NewWidget);
        DGuiWidget* AddStaticWidgetFromFile(std::string LayoutFilename);
        DGuiContainer* SetActiveContainer(DGuiContainer *Container);
        DGuiContainer* SetActiveContainer(std::string ContainerName);
        DGuiContainer* GetContainerFromName(std::string ContainerName);

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
        DGuiContainer *ActiveContainer;
    private:
        std::map<std::string,DGuiContainer*> Containers;
        std::map<std::string,DGuiWidget*> StaticWidgets;

        // Event callbacks
        OnGuiEventCallback GuiEventCallback;
        std::function<void (void)> AppStartedCallback;
        std::function<void (void)> AppStoppedCallback;
        std::function<void (void)> TickCallback;
 };

#endif