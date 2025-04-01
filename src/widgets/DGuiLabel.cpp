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

DGuiLabel::DGuiLabel(DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(std::ref(WidgetTree),ParentWidget,EventCallback)
{
    InitDefault();
    FinalizeFromTree(WidgetTree);
    Ready=(Type == DLABEL);
}

DGuiLabel::DGuiLabel(const fs::path& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiLabel(std::move(ExtractDTree(LayoutFilename)),ParentWidget,EventCallback)
{
}

DGuiLabel::DGuiLabel(DGuiWidget* ParentWidget) : DGuiLabel(Rectangle(0,0,0,0),ParentWidget)
{
}

void DGuiLabel::InitDefault(void)
{
    DEFAULT_SIDE_SIZE=50;
    DEFAULT_WIDTH=50;
    DEFAULT_HEIGHT=20;
}

void DGuiLabel::FinalizeFromTree(DTools::DTree& WidgetTree)
{
    // ** Read class specific properties **
    SetPrefix(WidgetTree.ReadString(DJsonTree::ITEM_PREFIX,""),false);
    SetSuffix(WidgetTree.ReadString(DJsonTree::ITEM_SUFFIX,""),false);
    UpdateSize();
}

void DGuiLabel::SetPrefix(std::string PrefixText, bool ForceUpdate) {
    if (TextPrefix == PrefixText) {
        return;
    }
    TextPrefix=PrefixText;

    if (ForceUpdate) {
        UpdateSize();
    }
}

void DGuiLabel::SetSuffix(std::string SuffixText, bool ForceUpdate) {
    if (TextSuffix == SuffixText) {
        return;
    }
    TextSuffix=SuffixText;

    if (ForceUpdate) {
        UpdateSize();
    }
}

void DGuiLabel::UpdateSize(void)
{
    // Expand due to the padding and border
    if (Text.empty()) {
        return;
    }
    int TextOffset=Properties.BorderWidth+Properties.TextPadding;
    SetWidth(GetTextBounds().width+(TextOffset*2));
    SetHeight(Properties.TextSize+(TextOffset*2));
}

Rectangle DGuiLabel::GetTextBounds(void)
{
    // Measure text
    /// @todo UpdateTextWith() when text changes
    int TextWidth=GetTextWidth(TextPrefix+Text+TextSuffix,Properties.TextFont,Properties.TextSize);
    
    // Calculate text bounds
    int TextOffset=Properties.BorderWidth+Properties.TextPadding;
    Rectangle AbsBounds=GetAbsBounds();
    Rectangle TextBounds;
    TextBounds.x=AbsBounds.x+TextOffset;
    TextBounds.y=AbsBounds.y+TextOffset;
    TextBounds.width=TextWidth;
    TextBounds.height=Properties.TextSize;

    return TextBounds;
}

void DGuiLabel::Clear(void) {
    Rectangle AbsBounds=GetAbsBounds();
    DrawRectangle(AbsBounds.x,AbsBounds.y,AbsBounds.width,AbsBounds.height,GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
}

/**
 * @brief Draw the label.
 */
void DGuiLabel::Draw()
{
    Rectangle AbsBounds=GetAbsBounds();
    // Draw background
    DrawRectangle(AbsBounds.x,AbsBounds.y,AbsBounds.width,AbsBounds.height,GetColor(Properties.BackGroundColor));
    // Draw label
    //GuiLabel(Bounds,(TextPrefix+Text+TextSuffix).c_str());
    RayGuiDrawText(TextPrefix+Text+TextSuffix,GetTextBounds(),Properties.TextAlign,GetColor(Properties.TextColor));
}