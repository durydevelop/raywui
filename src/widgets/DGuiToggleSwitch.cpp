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
    AddItem("ON");
    AddItem("OFF");
}

bool DGuiToggleSwitch::IsChecked(void)
{
    return Checked;
}

void DGuiToggleSwitch::SetChecked(bool Checked)
{
    this->Checked=Checked;
}