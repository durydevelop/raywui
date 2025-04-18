#include "DGuiToggleSwitch.h"
#include <DGuiLabel.h>

DGuiToggleSwitch::DGuiToggleSwitch() : DGuiToggleSlide(-1,-1,-1,-1,Parent)
{
    InitDefault();
}

/**
 * @brief Constructor from WidgetTree does not call DGuiWidget(std::ref(WidgetTree),ParentWidget,EventCallback) because inherith from DGuiToggleSlide.
 * N.B. Does not have a FinalizeFromTree() because DGuiToggleSwitch have only 2 states and are set in Finilized().
 * 
 * @param WidgetTree 
 * @param ParentWidget 
 * @param EventCallback 
 */
DGuiToggleSwitch::DGuiToggleSwitch(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiToggleSwitch(-1,-1,-1,-1,ParentWidget,EventCallback)
{
    Ready=InitFromTree(WidgetTree);
    Finalize();
    Text.InitFromTree(WidgetTree);
    if (LabelExt.Label) {
        if (LabelExt.Label->GetFontSize() < 0) {
            // Label font size from parent
            LabelExt.Label->SetFontSize(Text.GetFontSize(),false);
            LabelExt.Label->Update();
        }
    }
    Update();
    Ready&=(Type == DTOGGLESWITCH);
}

DGuiToggleSwitch::DGuiToggleSwitch(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiToggleSwitch(std::move(ExtractDTree(LayoutFilename)),ParentWidget,EventCallback)
{
}

void DGuiToggleSwitch::Finalize(void)
{
    Checked=false;
    // ItemIndex 0
    AddItem("OFF");
    // ItemIndex 1
    AddItem("ON");
}

bool DGuiToggleSwitch::IsChecked(void)
{
    return ItemIndex == 1;
}

void DGuiToggleSwitch::SetChecked(bool Checked)
{
    Checked ? ItemIndex=1 : ItemIndex=0;
}