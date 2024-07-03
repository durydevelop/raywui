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

#define VERSION "0.0.1"
#define FORM_MAIN "FormMain"
#define FORM_TWO "FormTwo"
/*
bool ButtonDxMainClick(DWidgetEvent Event) {
    //DLog::debug("FormMain->ButtonSx WidgetEvent %d from callback outside App context",Event.EventCode);
    return false;
};
*/
int main()
{
    // Create App context
    DGuiApp App(800,480,"Goldlake Frontend Ver. " VERSION);

    // Add MainForm
    //DLog::debug("Create Container1");
    DGuiContainer *FormMain=App.AddContainer(FORM_MAIN);

    // Add Button and set callback
    //DLog::debug("Create FormMainButtonFormTwo");
    DGuiButton *FormMainButtonFormTwo=new DGuiButton(50,10,100,100,nullptr);
    FormMainButtonFormTwo->SetText(FORM_TWO);
    FormMainButtonFormTwo->SetOnWidgetEvent([&] (DWidgetEvent Event) {
        //printf("FormMainButtonFormTwo WidgetEvent %d",Event.EventCode);
        App.SetActiveContainer(FORM_TWO);
        return true;
    });
    FormMain->AddWidget(FormMainButtonFormTwo);

    // Add Label
    //DLog::debug("Create FormMainLabel");
    DGuiLabel *FormMainLabel=new DGuiLabel(100,200,100,20,nullptr);
    FormMainLabel->Name="Label1";
    FormMainLabel->SetText("Press ESC to quit");
    FormMain->AddWidget(FormMainLabel);

    // Add StatusBar with 2 items
    //DLog::debug("Create FormMainStatusBar");
    DGuiStatusBar *StatusBar=new DGuiStatusBar(DGuiWidget::DOCK_BOTTOM,25,nullptr);
    StatusBar->Name="Statusbar1";
    //DLog::debug("AddItem: 0,50");
    StatusBar->AddItem("Item1",0,100,"Status: normal");
    //DLog::debug("AddItem: 100,50");
    StatusBar->AddItem("Item2",StatusBar->GetWidth()-100,100,"Version 0.0.0.0");
    StatusBar->SetBorderWidth(2);
    App.AddStaticWidget(StatusBar);

    // Add second form
    //DLog::debug("Create Container2");
    DGuiContainer *FormTwo=App.AddContainer(FORM_TWO);

    // Add Label
    DGuiLabel *LabelFormTwo=new DGuiLabel(100,200,100,20,nullptr);
    LabelFormTwo->Name="Label2";
    LabelFormTwo->SetText("I am FormSx");
    FormTwo->AddWidget(LabelFormTwo);

    // Add button for change label text
    DGuiButton* FormTwoButtonLabel=new DGuiButton(50,50,100,100,nullptr);
    FormTwoButtonLabel->Name="ButtonLabel";
    FormTwoButtonLabel->SetText("Change Label");
    FormTwoButtonLabel->SetOnWidgetEvent([&] (DWidgetEvent Event) {
        //DLog::debug("FormTwoButtonLabel WidgetEvent %d",Event.EventCode);
        LabelFormTwo->SetText("New text");
        return false;
    });
    FormTwo->AddWidget(FormTwoButtonLabel);

    // Add button for back to main form
    DGuiButton* FormTwoButtonBack=new DGuiButton(200,50,100,100,nullptr);
    FormTwoButtonBack->Name="ButtonBack";
    FormTwoButtonBack->SetText("Back");
    FormTwoButtonBack->SetOnWidgetEvent([&] (DWidgetEvent Event) {
        //DLog::debug("FormTwoButtonBack WidgetEvent %d",Event.EventCode);
        App.SetActiveContainer(FORM_MAIN);
        return true;
    });
    FormTwo->AddWidget(FormTwoButtonBack);

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
