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

int main(int argc, char *argv[])
{
    const std::vector<std::string> args(argv + 1, argv + argc);
    std::vector<std::string> layoutList;

    for(auto itArg = std::begin(args); itArg != std::end(args); itArg++) {
        std::string sArg=*itArg;
        if (sArg == "-h" || sArg == "--help") {
            //showUsage(argv[0]);
            return false;
        }
        else if (sArg[0] == '-' && itArg+1 == std::end(args)) {
            std::cerr << "Options not supported" << std::endl;
            return -1;
        }
        else if (sArg == "all" || sArg == "ALL") {
            layoutList.clear();
            break;
        }
        else {
            layoutList.emplace_back(sArg);
        }
    }

    // Create App context
    DGuiApp App(800,480,"rw-generate Ver. " PROJECT_VERSION);

    for (auto layoutName : layoutList) {
        
    }
}
