#include "DGuiLed.h"
#include "raywui_log.h"
#include "dpplib/DString.h"

using namespace DTools;

const char TAG[10] = "DGuiLed";

/// @todo: SetPadding()
/// @todo: SetRadius()

DGuiLed::DGuiLed(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiContainer(std::ref(WidgetTree),ParentWidget,EventCallback)
{
    InitDefault();
    FinalizeFromTree(WidgetTree);
    Ready=(Type == DLED);
}

DGuiLed::DGuiLed(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiLed(std::move(ExtractDTree(LayoutFilename)),ParentWidget,EventCallback)
{
}

void DGuiLed::InitDefault(void)
{
    Radius=Bounds.width/2;
    LedOffset={0,0};
    LedPadding={0,0,0,0};
    OnColor=ColorToInt(RED);
    OffColor=ColorToInt(GRAY);
    CurrBorderColor=ColorToInt(BLACK);
    SetSize(Radius*2,Radius*2);
    Off();
}

void DGuiLed::FinalizeFromTree(DTools::DTree& WidgetTree)
{
    // ** Read class specific properties **
    Radius=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS,DJsonTree::ITEM_RADIUS,10);

    LedPadding.Left=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS+DTree::DEFAULT_TRANSLATOR+DJsonTree::ITEM_PADDING,DJsonTree::ITEM_LEFT,0);
    LedPadding.Right=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS+DTree::DEFAULT_TRANSLATOR+DJsonTree::ITEM_PADDING,DJsonTree::ITEM_RIGHT,0);
    LedPadding.Bottom=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS+DTree::DEFAULT_TRANSLATOR+DJsonTree::ITEM_PADDING,DJsonTree::ITEM_BOTTOM,0);
    LedPadding.Top=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS+DTree::DEFAULT_TRANSLATOR+DJsonTree::ITEM_PADDING,DJsonTree::ITEM_TOP,0);

    if (Properties.Anchor.AnchorToSide == ANCHOR_NONE) {
        // No anchors defined, use center to set position
        int LedOffsetX=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS,DJsonTree::ITEM_CENTER_X,0);
        int LedOffsetY=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS,DJsonTree::ITEM_CENTER_Y,0);
        SetPos(LedOffsetX-Radius,LedOffsetX-Radius);
    }

    SetSize((Radius*2)+LedPadding.Left+LedPadding.Right,(Radius*2)+LedPadding.Bottom+LedPadding.Top);

    std::string LabelIntText=WidgetTree.ReadString(DJsonTree::ITEM_LABEL_INT,DJsonTree::ITEM_TEXT,"");
    std::string LabelIntSideStr=WidgetTree.ReadString(DJsonTree::ITEM_LABEL_INT,DJsonTree::ITEM_SIDE,"");
    int LabelIntOffset=WidgetTree.ReadInteger(DJsonTree::ITEM_LABEL_INT,DJsonTree::ITEM_OFFSET,0);
    int LabelIntFontSize=WidgetTree.ReadInteger(DJsonTree::ITEM_LABEL_INT,DJsonTree::ITEM_FONT_SIZE,0);
    if (!LabelIntText.empty()) {
        SetLabelInt(LabelIntText,LabelIntFontSize,NameToSide(LabelIntSideStr,SIDE_LEFT),LabelIntOffset);
    }
    std::string onc=WidgetTree.ReadString(DJsonTree::ITEM_ON_COLOR,"#FF0000"); // Red
    std::string offc=WidgetTree.ReadString(DJsonTree::ITEM_OFF_COLOR,"#808080"); //Gray
    OnColor=ColorStringToInt(onc);
    OffColor=ColorStringToInt(offc);
    SwitchedOn=WidgetTree.ReadBool(DJsonTree::ITEM_SWITCHED_ON,false);
    SwitchTo(SwitchedOn);
}

void DGuiLed::On(void)
{
    SwitchTo(true);
}

void DGuiLed::Off(void)
{
    SwitchTo(false);
}

void DGuiLed::Toggle(void) {
    SwitchTo(!SwitchedOn);
}

void DGuiLed::SwitchTo(bool On)
{
    SwitchedOn=On;
    CurrFilledColor=SwitchedOn ? OnColor : OffColor;
}

void DGuiLed::SwitchToColor(uint NewColor)
{
    CurrFilledColor=NewColor;
}

void DGuiLed::SwitchToColor(Color NewColor)
{
    CurrFilledColor=ColorToInt(NewColor);
}

/*
// Override
void DGuiLed::SetPos(int LeftPos, int TopPos)
{
    DGuiWidget::SetPos(LeftPos,TopPos);
    Center.x=Bounds.x+Radius;
    Center.y=Bounds.y+Radius;
}

void DGuiLed::SetSize(int Width, int Height) {
    DGuiWidget::SetSize(Width,Height);
    //SetPos(Bounds.x-Radius,Bounds.y-Radius);
    //UpdateLabelInt();
}
*/
void DGuiLed::SetCenter(int LeftPos, int TopPos)
{
    SetPos(LeftPos-Radius,TopPos-Radius);
}

/// Override: setting LabelInt to the left or top, led needs to move its position.
void DGuiLed::SetLabelInt(std::string LabelIntText, int FontSize, DSide Side, int Offset)
{
    DGuiContainer::SetLabelInt(LabelIntText,FontSize,Side,Offset);
    if (LabelInt.Label) {
        // Move led if labelint is on the left/top
        switch (Side) {
            case SIDE_LEFT:
                LedOffset.x=LabelInt.Label->GetWidth();
                break;
            case SIDE_TOP:
                LedOffset.y=LabelInt.Label->GetHeight();
                break;
            default:
                break;
        }
    }
    UpdateParentAligns();
    UpdateAnchor();
}

/**
 * @brief Draw the image.
 * Override method
 * @return the #Result value of this widget.
 */
void DGuiLed::Draw()
{
    // Need to call parent draw TODO: can be omitted?
    DGuiContainer::Draw();

    Rectangle AbsBounds=GetAbsBounds();
    int LedX=AbsBounds.x+LedOffset.x+Radius+LedPadding.Left;
    int LedY=AbsBounds.y+LedOffset.y+Radius+LedPadding.Top;
    DrawCircle(LedX,LedY,Radius,GetColor(CurrFilledColor));
    DrawCircleLines(LedX,LedY,Radius,GetColor(CurrBorderColor));
}
