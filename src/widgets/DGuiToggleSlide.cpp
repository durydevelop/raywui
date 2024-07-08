#include "DGuiToggleSlide.h"
#include <dpplib/DVector.h>
//#include "raywui_log.h"

DGuiToggleSlide::DGuiToggleSlide(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(DBUTTON,LeftPos,TopPos,ControlWidth,ControlHeight,ParentWidget,EventCallback)
{
    InitDefault();
}
        
DGuiToggleSlide::DGuiToggleSlide(Rectangle WidgetBounds, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(DBUTTON,WidgetBounds,ParentWidget,EventCallback)
{
    InitDefault();
}

DGuiToggleSlide::DGuiToggleSlide(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(WidgetTree,ParentWidget,EventCallback)
{
    InitDefault();
    FinalizeFromTree(WidgetTree);
    Ready=(Type == DTOGGLESLIDE);
}

DGuiToggleSlide::DGuiToggleSlide(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiToggleSlide(std::move(ExtractDTree(LayoutFilename)),ParentWidget,EventCallback)
{
}

void DGuiToggleSlide::InitDefault(void)
{
    ItemIndex=0;
    TextBoundsWidth=0;
    SliderPadding=2;
    
    //DEFAULT_SIDE_SIZE=50;
    //DEFAULT_WIDTH=50;
    //DEFAULT_HEIGHT=20;
}

void DGuiToggleSlide::FinalizeFromTree(DTools::DTree& WidgetTree)
{
    // ** Read class specific properties **
    Items=WidgetTree.ReadArrayNames("Items");
}

/**
 * @brief Add a toggle Item
 * 
 * @param Text 
 */
void DGuiToggleSlide::AddItem(const std::string &Text)
{
    Items.emplace_back(Text);
}

/**
 * @brief Set current toggle index position.
 * 
 * @param Index 
 */
void DGuiToggleSlide::SetItemIndex(int Index)
{
    ItemIndex=Index < 0 ? 0 : Index;
}

/**
 * @brief Get current toggle item position.
 * 
 * @return uint8_t 
 */
int DGuiToggleSlide::GetItemIndex(void)
{
    return ItemIndex;
}

void DGuiToggleSlide::SetSliderPadding(int Padding)
{
    SliderPadding=Padding < 0 ? 0 : Padding;
}

/**
 * @brief Draw the button.
 * Override method
 * @return the #Result value of this widget.
 */
void DGuiToggleSlide::Draw()
{
    if (!Items.empty()) {
        int CurrPadding=GuiGetStyle(SLIDER,SLIDER_PADDING);
        GuiSetStyle(SLIDER, SLIDER_PADDING, SliderPadding);
        if (DrawToggleSlider(Bounds,&ItemIndex)) {
            DWidgetEvent Event = { DEventCode::TOGGLE_CHANGED, reinterpret_cast<void *>(static_cast<intptr_t>(ItemIndex)) };
            SendEvent(Event);
        }
        GuiSetStyle(SLIDER, SLIDER_PADDING, CurrPadding);
    }
}

/// @todo? Gui control property style color element from raygui (no other way nor now)
typedef enum { BORDER = 0, BASE, TEXT, OTHER } GuiPropertyElement;
/**
 * @brief Custom Draw
 * 
 * @param bounds 
 * @param active 
 * @return int 
 */
int DGuiToggleSlide::DrawToggleSlider(Rectangle bounds, int *active)
{
    int result = 0;
    GuiState state = GetGuiState();

    Rectangle slider = {
        0,      // Calculated later depending on the active toggle
        bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH) + GuiGetStyle(SLIDER, SLIDER_PADDING),
        (bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH) - (Items.size() + 1)*GuiGetStyle(SLIDER, SLIDER_PADDING))/Items.size(),
        bounds.height - 2*GuiGetStyle(SLIDER, BORDER_WIDTH) - 2*GuiGetStyle(SLIDER, SLIDER_PADDING)
    };

    // Update control
    //--------------------------------------------------------------------
    if ((state != STATE_DISABLED) && !IsGuiLocked())
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = STATE_PRESSED;
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                state = STATE_PRESSED;
                (*active)++;
                result = 1;
            }
            else state = STATE_FOCUSED;
        }

        if ((*active) && (state != STATE_FOCUSED)) state = STATE_PRESSED;
    }

    if (*active >= Items.size()) *active = 0;
    slider.x = bounds.x + GuiGetStyle(SLIDER, BORDER_WIDTH) + (*active + 1)*GuiGetStyle(SLIDER, SLIDER_PADDING) + (*active)*slider.width;
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    RayGuiDrawRectangle(bounds, GuiGetStyle(SLIDER, BORDER_WIDTH), GetColor(GuiGetStyle(TOGGLE, BORDER + (state*3))), GetColor(GuiGetStyle(TOGGLE, BASE_COLOR_NORMAL)));

    // Draw internal slider
    if (state == STATE_NORMAL) RayGuiDrawRectangle(slider, 0, BLANK, GetColor(GuiGetStyle(SLIDER, BASE_COLOR_PRESSED)));
    else if (state == STATE_FOCUSED) RayGuiDrawRectangle(slider, 0, BLANK, GetColor(GuiGetStyle(SLIDER, BASE_COLOR_FOCUSED)));
    else if (state == STATE_PRESSED) RayGuiDrawRectangle(slider, 0, BLANK, GetColor(GuiGetStyle(SLIDER, BASE_COLOR_PRESSED)));

    // Draw text in slider
    Rectangle textBounds = { 0 };
    textBounds.width = (float)GetTextWidth(DTools::DVector::JoinToStr(Items,";"),GuiGetFont(),Properties.TextSize);
    textBounds.height = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
    textBounds.x = slider.x + slider.width/2 - textBounds.width/2;
    textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

    RayGuiDrawText(Items[*active].c_str(), textBounds, GuiGetStyle(TOGGLE, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(TOGGLE, TEXT + (state*3))), GetGuiAlpha()));
    //--------------------------------------------------------------------

    return result;
}
