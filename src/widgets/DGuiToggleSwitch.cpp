#include "DGuiToggleSwitch.h"



DGuiToggleSwitch::DGuiToggleSwitch() : DGuiToggleSlide(-1,-1,-1,-1,Parent)
{
    InitDefault();
}

DGuiToggleSwitch::DGuiToggleSwitch(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiToggleSwitch(-1,-1,-1,-1,ParentWidget,EventCallback)
{
    Ready=InitFromTree(WidgetTree);
    InitDefault();
    Ready&=(Type == DTOGGLESWITCH);
}

DGuiToggleSwitch::DGuiToggleSwitch(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiToggleSwitch(std::move(ExtractDTree(LayoutFilename)),ParentWidget,EventCallback)
{
}

void DGuiToggleSwitch::InitDefault(void)
{
    DGuiToggleSlide::InitDefault();
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