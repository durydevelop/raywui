/*******************************************************************************************
*
*   DGuiApp v1.0.0
*
*   LICENSE:
*
*   2023 Dury Corp.
*
**********************************************************************************************/

#include <DGuiApp.h>
#include <DGuiButton.h>
#include <DGuiLabel.h>
#include <DGuiStatusBar.h>
#include <DGuiContainer.h>
#include <DGuiImage.h>
#include <DGuiToggleSwitch.h>
#include <iostream>
#include <dpplib/DPath.h>

#define VERSION "0.0.1"
#define FORM_MAIN "FormMain"
#define BUTTON_LOGIN "ButtonLogin"

int main(int argc, char *argv[])
{
    std::string layoutFilename="layout.json";

    const std::vector<std::string> args(argv + 1, argv + argc);
    std::cout << "filename=" << argv[0] << std::endl;

    for(auto itArg = std::begin(args); itArg != std::end(args); itArg++) {
        std::string sArg=*itArg;
        if (sArg == "-h" || sArg == "--help") {
            //showUsage(argv[0]);
            return false;
        }

        // Check for option type
        if (sArg[0] == '-' && itArg+1 == std::end(args)) {
            std::cerr << "Option " << sArg << " unknown" << std::endl;
            return -1;
        }
        else  {
            layoutFilename=sArg;
        }
    }

    if (!DTools::DPath::Exists(layoutFilename)) {
        std::cerr << layoutFilename << " does not exist" << std::endl;
        return -1;
    }
            
    // Create App context
    DGuiApp App(800,480,"showlayout Ver. " VERSION);

    DGuiContainer *container=nullptr;
    DGuiWidget *widget=DGuiWidget::New(layoutFilename,nullptr);
    if (widget->GetWidgetType() != DCONTAINER) {
      container=new DGuiContainer(-1,-1,-1,-1,nullptr);
      container->AddWidget(widget);
    }
    else {
        container=(DGuiContainer *) widget;
    }
    //DGuiContainer *FormLogin=new DGuiContainer(layoutFilename,nullptr);

    App.AddContainer(container);
    //DGuiImage *image=(DGuiImage *) container->GetChildFromName("Image1");
    //DGuiToggleSwitch *s=new DGuiToggleSwitch();

    // Set global gui event handler that receive all events except the ones
    // that widget has been hadled by it self by returning true from OnWidgetEvent callback
    App.SetOnGuiEvent([&] (DGuiEvent GuiEvent) {
        //DLog::debug("Global GuiEvent from %s",GuiEvent.WidgetId.c_str());
        //if (GuiEvent.WidgetId == ButtonDxMain->GetId()) {
        //    LabelMain->SeText("ButtonDx pressed");
        //}
    });

    // Now run all and wait for ESC or App quit
    DResult res=App.Run();

    // print out result message
    //DLog::debug("Result=%s",res.Message.c_str());
}
