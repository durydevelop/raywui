#include "DGuiLabel.h"
#include "raywui_log.h"

using namespace DTools;

const char TAG[10]="DGuiLabel";

DGuiLabel::DGuiLabel(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget) : DGuiWidget(DLABEL,LeftPos,TopPos,ControlWidth,ControlHeight,ParentWidget)
{
    InitDefault();
}

DGuiLabel::DGuiLabel(Rectangle WidgetBounds, DGuiWidget *ParentWidget) : DGuiWidget(DLABEL,WidgetBounds,ParentWidget) {
    InitDefault();
}

DGuiLabel::DGuiLabel(DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(WidgetTree,ParentWidget,EventCallback)
{
    InitDefault();
    Ready=(Type == DLABEL);
}

DGuiLabel::DGuiLabel(const fs::path& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiLabel(std::move(ExtractDTree(LayoutFilename)),ParentWidget,EventCallback)
{
    Log::debug(TAG,"LayoutFilename Contructor");
}

DGuiLabel::DGuiLabel(DGuiWidget* ParentWidget) : DGuiLabel(Rectangle(0,0,0,0),ParentWidget)
{
    Log::debug(TAG,"Void Contructor");
}

void DGuiLabel::InitDefault(void)
{
    DEFAULT_SIDE_SIZE=50;
    DEFAULT_WIDTH=50;
    DEFAULT_HEIGHT=20;
}

void DGuiLabel::SetPrefix(std::string PrefixText) {
    if (TextPrefix == PrefixText) {
        return;
    }
    TextPrefix=PrefixText;
    //Changed=true;
}

void DGuiLabel::SetSuffix(std::string SuffixText) {
    if (TextSuffix == SuffixText) {
        return;
    }
    TextSuffix=SuffixText;
    //Changed=true;
}

void DGuiLabel::ClearText(void) {
    //Rectangle rect=GetTextBounds(LABEL, Bounds);
    DrawRectangle(Bounds.x,Bounds.y,Bounds.width,Bounds.height,GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
}

/**
 * @brief Draw the label.
 */
void DGuiLabel::Draw()
{
    DrawRectangle(Bounds.x,Bounds.y,Bounds.width,Bounds.height,GetColor(Properties.BackGroundColor));
    GuiLabel(Bounds,(TextPrefix+Text+TextSuffix).c_str());
}