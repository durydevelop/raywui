#include "DGuiLabel.h"
#include "raywui_log.h"

using namespace DTools;

const char TAG[10]="DGuiLabel";

DGuiLabel::DGuiLabel(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget) : DGuiWidget(DLABEL,LeftPos,TopPos,ControlWidth,ControlHeight,ParentWidget)
{
    InitDefault();
}

DGuiLabel::DGuiLabel(Rectangle WidgetBounds, DGuiWidget *ParentWidget) : DGuiWidget(DLABEL,WidgetBounds,ParentWidget)
{
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
    Text.SetWidgetType(DLABEL);
}

void DGuiLabel::FinalizeFromTree(DTools::DTree& WidgetTree)
{
    // Init Text
    Text.InitFromTree(WidgetTree);
    if (LabelExt.Label) {
        if (LabelExt.Label->GetFontSize() < 0) {
            // Label font size from parent
            LabelExt.Label->SetFontSize(Text.GetFontSize(),false);
        }
    }
    Update();
}

void DGuiLabel::SetText(std::string NewText, bool ForceAutoSize) {
    Text.SetText(NewText);

    if (ForceAutoSize || Properties.HSizeMode == DSizeMode::SIZE_AUTO || Properties.VSizeMode == DSizeMode::SIZE_AUTO) {
        Update();
    }
}

const std::string& DGuiLabel::GetText(void) {
    return std::ref(Text.GetText());
}

int DGuiLabel::GetFontSize(void) {
    return Text.GetFontSize();
}

void DGuiLabel::SetPrefix(std::string PrefixText, bool ForceUpdate) {
    if (Text.GetPrefix() == PrefixText) {
        return;
    }
    Text.SetPrefix(PrefixText);

    if (ForceUpdate) {
        Update();
    }
}

void DGuiLabel::SetSuffix(std::string SuffixText, bool ForceUpdate) {
    if (Text.GetSuffix() == SuffixText) {
        return;
    }
    Text.SetSuffix(SuffixText);

    if (ForceUpdate) {
        Update();
    }
}

/**
 * @brief Set the size of Text (if widget need text).
 * 
 * @param NewSize   ->  the new text size.
 */
void DGuiLabel::SetFontSize(int NewSize, bool ForceAutoSize)
{
    if (NewSize == 0) {
        NewSize=GuiGetStyle(DEFAULT,TEXT_SIZE);
    }

    Text.SetFontSize(NewSize);

    if (ForceAutoSize) {
        Update();
    }
}

void DGuiLabel::SetTextPadding(int NewPadding, bool ForceAutoSize)
{
    Text.SetPadding(NewPadding);

    if (ForceAutoSize) {
        Update();
    }
}

void DGuiLabel::SetTextSpacing(int NewSpacing, bool ForceAutoSize)
{
    Text.SetSpacing(NewSpacing);

    if (ForceAutoSize) {
        Update();
    }
}

/**
 * @brief Set text align value from align string.
 * 
 * @param AlignHoriz 
 * @param AlignVert 
 */
void DGuiLabel::SetTextAlign(std::string AlignHoriz,std::string AlignVert) {
    Text.SetAlign(AlignHoriz,AlignVert);
}

void DGuiLabel::SetTextAlign(DTextAlign TextAlign) {
    Text.SetAlign(TextAlign);
}

void DGuiLabel::Update(void)
{
    if (!Text.IsEmpty()) {
        if (Properties.HSizeMode == DSizeMode::SIZE_AUTO) {
            AutoWidth();
        }

        if (Properties.VSizeMode == DSizeMode::SIZE_AUTO) {
            AutoHeight();
        }
    }

    DGuiWidget::Update();
}

void DGuiLabel::AutoWidth(void)
{
    int TextOffset=Properties.BorderWidth+Text.GetPadding();
    Bounds.width=GetTextBounds(Text).width+(TextOffset*2);
}

void DGuiLabel::AutoHeight(void)
{
    int TextOffset=Properties.BorderWidth+Text.GetPadding();
    Bounds.height=Text.GetFontSize()+(TextOffset*2);
}

DTextAlign DGuiLabel::GetTextAlign(void)
{
    return Text.GetAlign();
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
    // Store current global raygui styles
    BackupCurrentTextStyle(TempText);
    // Set global raygui style from this->Properties
    UpdateCurrentTextStyle(Text);

    Rectangle AbsBounds=GetAbsBounds();
    // Draw background
    DrawRectangle(AbsBounds.x,AbsBounds.y,AbsBounds.width,AbsBounds.height,GetColor(Properties.BackgroundColor));
    // Draw label
    //GuiLabel(Bounds,(TextPrefix+Text+TextSuffix).c_str());
    RayGuiDrawText(Text.GetPrefix()+Text.GetText()+Text.GetSuffix(),GetTextBounds(Text),Text.GetAlign(),GetColor(Text.GetColor()));

    // Restore previous saved global raygui styles
    RestoreCurrentTextStyle(TempText);
}
