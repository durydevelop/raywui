#include "DGuiNewWidget.h"
//#include "raywui_log.h"

DGuiNewWidget::DGuiNewWidget(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(DBUTTON,LeftPos,TopPos,ControlWidth,ControlHeight,ParentWidget,EventCallback)
{
    InitDefault();
}
        
DGuiNewWidget::DGuiNewWidget(Rectangle WidgetBounds, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(DBUTTON,WidgetBounds,ParentWidget,EventCallback)
{
    InitDefault();
}

DGuiNewWidget::DGuiNewWidget(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(WidgetTree,ParentWidget,EventCallback)
{
    InitDefault();
    //FinalizeFromTree(WidgetTree);
    Ready=(Type == DNEWWIDGET);
}

DGuiNewWidget::DGuiNewWidget(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiNewWidget(std::move(ExtractDTree(LayoutFilename)),ParentWidget,EventCallback)
{
}

void DGuiNewWidget::InitDefault(void)
{
    //DEFAULT_SIDE_SIZE=50;
    //DEFAULT_WIDTH=50;
    //DEFAULT_HEIGHT=20;
}

void DGuiNewWidget::FinalizeFromTree(DTools::DTree& WidgetTree)
{
    // ** Read class specific properties **
}

/**
 * @brief Draw the button.
 * Override method
 * @return the #Result value of this widget.
 */
void DGuiNewWidget::Draw()
{
    /*
    if (GuiButton(Bounds, Text.c_str())) {
        DWidgetEvent Event={DEventCode::BUTTON_PRESS, nullptr};
        SendEvent(Event);
    }
    */
}
