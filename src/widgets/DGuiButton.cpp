#include "DGuiButton.h"
#include <DGuiLabel.h>
//#include "raywui_log.h"

DGuiButton::DGuiButton(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(DBUTTON,LeftPos,TopPos,ControlWidth,ControlHeight,ParentWidget,EventCallback)
{
    InitDefault();
}
        
DGuiButton::DGuiButton(Rectangle WidgetBounds, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(DBUTTON,WidgetBounds,ParentWidget,EventCallback)
{
    InitDefault();
}

DGuiButton::DGuiButton(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(std::ref(WidgetTree),ParentWidget,EventCallback)
{
    InitDefault();
    FinalizeFromTree(WidgetTree);
    Ready=(Type == DBUTTON);
}

DGuiButton::DGuiButton(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiButton(std::move(ExtractDTree(LayoutFilename)),ParentWidget,EventCallback)
{
}

void DGuiButton::InitDefault(void)
{
    DEFAULT_SIDE_SIZE=50;
    DEFAULT_WIDTH=50;
    DEFAULT_HEIGHT=20;
    Text.SetWidgetType(DBUTTON);
}

void DGuiButton::FinalizeFromTree(DTools::DTree& WidgetTree)
{
    // Init text
    Text.InitFromTree(WidgetTree);
    if (LabelExt.Label) {
        if (LabelExt.Label->GetFontSize() < 0) {
            // Label font size from parent
            LabelExt.Label->SetFontSize(Text.GetFontSize(),false);
        }
    }
    Update();
}

void DGuiButton::Update(void)
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

void DGuiButton::AutoWidth(void)
{
    int TextOffset=Properties.BorderWidth+Text.Padding;
    Bounds.width=GetTextBounds(Text).width+(TextOffset*2);
}

void DGuiButton::AutoHeight(void)
{
    int TextOffset=Properties.BorderWidth+Text.Padding;
    Bounds.height=Text.FontSize+(TextOffset*2);
}

/**
 * @brief Draw the button.
 * Override method
 * @return the #Result value of this widget.
 */
void DGuiButton::Draw()
{
    // Store current global raygui styles
    BackupCurrentTextStyle(TempText);
    // Set global raygui style from this->Properties
    UpdateCurrentTextStyle(Text);

    Rectangle AbsBounds=GetAbsBounds();
    if (GuiButton(AbsBounds, Text.Text.c_str())) {
        DWidgetEvent Event={DEventCode::BUTTON_PRESS, 0, nullptr};
        SendEvent(Event);
    }

    // Restore previous saved global raygui styles
    RestoreCurrentTextStyle(TempText);
}
