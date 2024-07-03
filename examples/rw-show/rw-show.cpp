/*******************************************************************************************
*
*   rw-show
*
*   rw-show is a tool that can read and show a raywui json layout file.
*   It can also convert a rgl layout file handled by raygui to json layout for raywui.
*
*
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

int main(int argc, char *argv[])
{
    const std::vector<std::string> args(argv + 1, argv + argc);

    if (args.size() == 0) {
        std::cerr << "Layout file not specified" << std::endl;
    }
    else if(args.size() > 1) {
        std::cerr << "Too much arguments than required" << std::endl;
    }

    if (args[0] == "-h" || args[0] == "--help") {
        //showUsage(argv[0]);
        return 0;
    }

    std::string layoutFilename=args[0];
    if (!DTools::DPath::Exists(layoutFilename)) {
        std::cerr << layoutFilename << " does not exist" << std::endl;
        return -1;
    }
            
    // Create App context
    DGuiApp App(800,480,"sw-show Ver. " PROJECT_VERSION);

    // Create container
    DGuiContainer *container=nullptr;
    // Load from file
    DGuiWidget *widget=DGuiWidget::New(layoutFilename,nullptr);
    if (widget->GetWidgetType() != DCONTAINER) {
        // It is stand-alone widget layout
        // so, create a container for it
        container=new DGuiContainer(-1,-1,-1,-1,nullptr);
        container->AddWidget(widget);
    }
    else {
        container=(DGuiContainer *) widget;
    }

    // Add container to app
    App.AddContainer(container);

    // Start drawing loop which waits for ESC or app quit
    DResult res=App.Run();
}
