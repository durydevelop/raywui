#include "DGuiWidget.h"
#include <DGuiContainer.h>
#include <DGuiButton.h>
#include <DGuiEdit.h>
#include <DGuiLabel.h>
#include <DGuiStatusBar.h>
#include <DGuiImage.h>
#include <DGuiToggleSlide.h>
#include <DGuiToggleSwitch.h>
#include <sstream>
#include <fstream>
#include "raywui_log.h"
#include <dpplib/DString.h>
#include <dpplib/DPath.h>
#include <dpplib/DCsv.h>
// Defined here because DGuiWidget is the base class inherited by any widget
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

using namespace DTools;

// Rgl layout file stuff
struct DRglLayout {
    static const uint8_t ControlLineItemsCount=10;
    enum ControlItemsIx{ CTRL_IX_ID=1, CTRL_IX_TYPE, CTRL_IX_NAME, CTRL_IX_X, CTRL_IX_Y, CTRL_IX_WIDTH, CTRL_IX_HEIGHT, CTRL_IX_ANCHOR_ID, CTRL_IX_TEXT };
    //enum RGL_ANCHOR_ITEMS_IX { ANCHOR_IX_ID=1, ANCHOR_IX_NAME, ANCHOR_IX_X, ANCHOR_IX_Y, ANCHOR_IX_ENABLED };
    //enum RGL_WINDOW_ITEMS_IX { REF_WINDOW_IX_X=1, REF_WINDOW_IX_Y, REF_WINDOW_WIDTH, REF_WINDOW_HEIGHT };
};

#define DOT DPreferences::DEFAULT_TRANSLATOR
#define DEFAULT_TEXT_PADDING 4
#define DEFAULT_TEXT_SPACING 2

const char TAG[11]="DGuiWidget";

/**
 * @brief Coordinate constructor.
 * 
 * @param WidgetType 
 * @param LeftPos 
 * @param TopPos 
 * @param WidgetWidth 
 * @param WidgetHeight 
 * @param EventCallback 
 */
DGuiWidget::DGuiWidget(DWidgetType WidgetType, int LeftPos, int TopPos, int WidgetWidth, int WidgetHeight, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback)
{
    // Create id
    GenerateId();
    // Parent
    SetParent(ParentWidget);
    // Events callback
    OnWidgetEvent=EventCallback;
    // Bounds
    SetBounds(LeftPos,TopPos,WidgetWidth,WidgetHeight);
    // Type
    SetWidgetType(WidgetType);
    // Initial Name
    Name=TypeToName(Type)+Id;

    Ready=true;
};

/**
 * @brief Bounds constructor.
 * 
 * @param WidgetType
 * @param WidgetBounds
 * @param EventCallback
 */
DGuiWidget::DGuiWidget(DWidgetType WidgetType, Rectangle WidgetBounds, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback)
{
    // Create id
    GenerateId();
    // Parent
    SetParent(ParentWidget);
    // Events callback
    OnWidgetEvent=EventCallback;
    // Bounds
    SetBounds(WidgetBounds);
    // Type
    SetWidgetType(WidgetType);
    // Initial Name
    Name=TypeToName(Type)+Id;

    Ready=true;
};

DGuiWidget::DGuiWidget(DWidgetType WidgetType, DDocking DockingPos, int OtherSize, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback)
{
    // Create id
    GenerateId();
    // Parent
    SetParent(ParentWidget);
    // Events callback
    OnWidgetEvent=EventCallback;
    // Docking
    SetDocking(DockingPos,OtherSize);
    // Type
    SetWidgetType(WidgetType);
    // Initial Name
    Name=TypeToName(Type)+Id;

    Ready=true;
};

DGuiWidget::DGuiWidget(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback)
{
    // Create id
    GenerateId();
    // Parent
    SetParent(ParentWidget);
    // Events callback
    OnWidgetEvent=EventCallback;
    // Init widget from DTree
    Ready=InitFromTree(WidgetTree);
    // Initial Name
//    if (Name.empty()) {
//        Name=TypeToName(Type)+Id;
//    }
}

DGuiWidget* DGuiWidget::New(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback)
{
    // Extract DTree
    auto WidgetTree=ExtractDTree(LayoutFilename);
    // Create from Dtree widget
    return New(WidgetTree,ParentWidget,EventCallback);
}

DGuiWidget* DGuiWidget::New(DTools::DTree& WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback)
{
    DWidgetType WidgetType=NameToType(WidgetTree.ReadString(DJsonTree::ITEM_TYPE,""));
    if (WidgetType == DCONTAINER) {
        //Log::debug(TAG,"New Container");
        DGuiContainer *Container=new DGuiContainer(WidgetTree,ParentWidget,EventCallback);
        return Container;
    }
    if (WidgetType == DLABEL) {
        //Log::debug(TAG,"New label");
        DGuiLabel *Label=new DGuiLabel(WidgetTree,ParentWidget,EventCallback);
        return Label;
    }
    else if (WidgetType == DBUTTON) {
        //Log::debug(TAG,"New button");
        DGuiButton *Button=new DGuiButton(WidgetTree,ParentWidget,EventCallback);
        return Button;
    }
    else if (WidgetType == DEDIT) {
        //Log::debug(TAG,"New edit");
        DGuiEdit *Edit=new DGuiEdit(WidgetTree,ParentWidget,EventCallback);
        return Edit;
    }
    else if (WidgetType == DSTATUSBAR) {
        //Log::debug(TAG,"New StatusBar");
        DGuiStatusBar *StatusBar=new DGuiStatusBar(WidgetTree,ParentWidget,EventCallback);
        return StatusBar;
    }
    else if (WidgetType == DIMAGE) {
        //Log::debug(TAG,"New edit");
        DGuiImage *NewImage=new DGuiImage(WidgetTree,ParentWidget,EventCallback);
        return NewImage;
    }
    else if (WidgetType == DTOGGLESLIDE) {
        DGuiToggleSlide *ToggleSlide=new DGuiToggleSlide(WidgetTree,ParentWidget,EventCallback);
        return ToggleSlide;
    }
    else if (WidgetType == DTOGGLESWITCH) {
        DGuiToggleSwitch *ToggleSwitch=new DGuiToggleSwitch(WidgetTree,ParentWidget,EventCallback);
        return ToggleSwitch;
    }

    if (WidgetTypes.contains(WidgetType)) {
        Log::error(TAG,"Widget type %s not implemented",WidgetTypes.at(WidgetType).c_str());
    }
    else {
        Log::error(TAG,"Unknown Widget Type");
    }
    return nullptr;
}

bool DGuiWidget::InitFromTree(DTools::DTree& WidgetTree)
{
    if (WidgetTree.IsEmpty()) {
        LastError="InitFromTree() WidgetTree is empty";
        return false;
    }
    // ** Load base info to instantiate a DWidget **
    // Type
    DWidgetType WidgetType=DWidgetType::UNKNOWN;
    std::string WidgetTypeName=WidgetTree.ReadString(DJsonTree::ITEM_TYPE,"");
    SetWidgetType(NameToType(WidgetTypeName));

    // Bounds
    Rectangle WidgetBounds;
    WidgetBounds.x=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS,DJsonTree::ITEM_LEFT,-1);
    WidgetBounds.y=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS,DJsonTree::ITEM_TOP,-1);
    WidgetBounds.width=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS,DJsonTree::ITEM_WIDTH,-1);
    WidgetBounds.height=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS,DJsonTree::ITEM_HEIGHT,-1);
    if (WidgetBounds.x < 0) {
        WidgetBounds.x=0;
    }
    if (WidgetBounds.y < 0) {
        WidgetBounds.y=0;
    }
    SetBounds(WidgetBounds);

    // Text
    SetText(WidgetTree.ReadString(DJsonTree::ITEM_TEXT,""),false);

    // ** Read widget properties **
    // Text size
    SetTextSize(WidgetTree.ReadInteger(DJsonTree::ITEM_TEXT_SIZE,0),false);

    // Text align
    std::string AlignHoriz=WidgetTree.ReadString(DJsonTree::ITEM_TEXT_ALIGN_H,"");
    std::string AlignVert=WidgetTree.ReadString(DJsonTree::ITEM_TEXT_ALIGN_V,"");
    SetTextAlign(AlignHoriz,AlignVert);
    
    // Text color
    std::string StrValue=WidgetTree.ReadString(DJsonTree::ITEM_TEXT_COLOR,"");
    if (!StrValue.empty()) {
        Properties.TextColor=ColorStringToInt(StrValue);
    }

    // Text spacing
    SetTextSpacing(WidgetTree.ReadInteger(DJsonTree::ITEM_TEXT_SPACING,0),false);

    // Text padding
    SetTextPadding(WidgetTree.ReadInteger(DJsonTree::ITEM_TEXT_PADDING,0));

    // Border width
    int IntValue=WidgetTree.ReadInteger(DJsonTree::ITEM_BORDER_WIDTH,0);
    if (IntValue > 0) {
        Properties.BorderWidth=IntValue;
    }

    // Border color
    IntValue=WidgetTree.ReadInteger(DJsonTree::ITEM_BORDER_COLOR,0);
    if (IntValue > 0) {
        Properties.BorderColor=IntValue;
    }

    // Border visible
    Properties.BorderVisible=WidgetTree.ReadBool(DJsonTree::ITEM_BORDER_VISIBLE,false);

    // Line color
    IntValue=WidgetTree.ReadInteger(DJsonTree::ITEM_LINE_COLOR,0);
    if (IntValue > 0) {
        Properties.LineColor=IntValue;
    }

    // Backround color
    IntValue=WidgetTree.ReadInteger(DJsonTree::ITEM_BACKGROUND_COLOR,0);
    if (IntValue > 0) {
        Properties.BackGroundColor=IntValue;
    }

    // AnchorId
    // Widget->Properties.AnchorId=WidgetTree.ReadString(DJsonTree::ITEM_ANCHOR_ID,""); // deprecated

    // Possible docking position
    std::string DockingSide=WidgetTree.ReadString(DJsonTree::ITEM_DOCKING,DJsonTree::ITEM_SIDE,"");
    if (!DockingSide.empty()) {
        int DockingSize=WidgetTree.ReadInteger(DJsonTree::ITEM_DOCKING,DJsonTree::ITEM_SIZE,-1);
        SetDocking(DockingSide,DockingSize);
    }

    // Outside label
    std::string LabelText=WidgetTree.ReadString(DJsonTree::ITEM_LABEL,DJsonTree::ITEM_TEXT,"");
    if (!LabelText.empty()) {
        // Has text
        int FontSize=WidgetTree.ReadInteger(DJsonTree::ITEM_LABEL,DJsonTree::ITEM_TEXT_SIZE,0);
        std::string LabelSide=WidgetTree.ReadString(DJsonTree::ITEM_LABEL,DJsonTree::ITEM_SIDE,"");
        int LabelOffset=WidgetTree.ReadInteger(DJsonTree::ITEM_LABEL,DJsonTree::ITEM_OFFSET,0);
        SetLabel(LabelText,FontSize,NameToDocking(LabelSide,DDocking::DOCK_LEFT),LabelOffset);
    }

    // Name
    Name=WidgetTree.ReadString(DJsonTree::ITEM_NAME,"");
    if (Name.empty()) {
        Name=TypeToName(Type)+Id;
        Log::warning(TAG,"Widget %s with id %s has no name, default created: %s",WidgetTypeName.c_str(),Id.c_str(),Name.c_str());
    }

    // Enabled
    Properties.Enabled=WidgetTree.ReadBool(DJsonTree::ITEM_ENABLED,true);

    // Visible
    Properties.Visible=WidgetTree.ReadBool(DJsonTree::ITEM_VISIBLE,true);

    return true;
}

void DGuiWidget::SetLabel(std::string LabelText, int FontSize, DDocking LabelSide, int LabelOffset)
{
    // Text
    if (LabelText.empty()) {
        return;
    }
    
    // Font size
    if (FontSize == 0) {
        // Use widget one
        FontSize=Properties.TextSize;
    }
    else if (FontSize < GuiGetStyle(DEFAULT,TEXT_SIZE)) {
        FontSize=GuiGetStyle(DEFAULT,TEXT_SIZE);
    }

    if (!Label) {
        // Not yet created
        Label=new DGuiLabel(nullptr);
    }

    Label->SetTextSize(FontSize,false);
    Label->SetText(LabelText,true);
    
    // Calculate position
    switch (LabelSide) {
        case DDocking::DOCK_BOTTOM:
            Label->Bounds.x=Bounds.x+((Bounds.width-Label->Bounds.width)/2); // Center horizzontally
            Label->Bounds.y=Bounds.y+Bounds.height+LabelOffset;
            break;
        case DDocking::DOCK_TOP:
            Label->Bounds.x=Bounds.x+((Bounds.width-Label->Bounds.width)/2); // Center horizzontally
            Label->Bounds.y=Bounds.y-Label->Bounds.height-LabelOffset;
            break;
        case DDocking::DOCK_LEFT:
            Label->Bounds.x=Bounds.x-Label->Bounds.width-LabelOffset;
            Label->Bounds.y=Bounds.y+((Bounds.height-Label->Bounds.height)/2); // Center vertically
            break;
        case DDocking::DOCK_RIGHT:
            Label->Bounds.x=Bounds.x+Bounds.width+LabelOffset;
            Label->Bounds.y=Bounds.y+((Bounds.height-Label->Bounds.height)/2); // Center vertically
            break;
        default:
            break;
    }

    Properties.LabelSide=LabelSide;
    Properties.LabelOffset=LabelOffset;
}

DGuiWidget::DDocking DGuiWidget::NameToDocking(std::string SideName, DGuiWidget::DDocking Default)
{
    if (SideName.empty()) {
        return Default;
    }
    else if (DString::CmpNoCase(SideName,DJsonTree::VALUE_BOTTOM)) {
        return DGuiWidget::DDocking::DOCK_BOTTOM;
    }
    else if (DString::CmpNoCase(SideName,DJsonTree::VALUE_TOP)) {
        return DGuiWidget::DDocking::DOCK_TOP;
    }
    else if (DString::CmpNoCase(SideName,DJsonTree::VALUE_LEFT)) {
        return DGuiWidget::DDocking::DOCK_LEFT;
    }
    else if (DString::CmpNoCase(SideName,DJsonTree::VALUE_RIGHT)) {
        return DGuiWidget::DDocking::DOCK_RIGHT;
    }
    return Default;
}

void DGuiWidget::GenerateId(void)
{
    // Create id
    std::ostringstream ss;
    ss << (void const *)this;
    Id=ss.str();
}

void DGuiWidget::SetParent(DGuiWidget *ParentContainer) {
    Parent=ParentContainer;
    if (Parent) {
        if (Bounds.width < 0) {
            // Parent width
            SetWidth(Parent->GetWidth());
        }
        if (Bounds.height < 0) {
            // Parent Height
            SetHeight(Parent->GetHeight());
        }
    }
    else {
        // Parent null, force re-set size
        SetWidth(Bounds.width);
        SetHeight(Bounds.height);
    }
}

void DGuiWidget::SetOnWidgetEvent(OnWidgetEventCallback EventCallback)
{
    OnWidgetEvent=EventCallback;
}

void DGuiWidget::SetOnGuiEvent(OnGuiEventCallback EventCallback)
{
    OnGuiEvent=EventCallback;
}

void DGuiWidget::SendEvent(DWidgetEvent WidgetEvent)
{
    bool Handled=false;
    if (OnWidgetEvent) {
        Handled=OnWidgetEvent(WidgetEvent);
    }
    if (OnGuiEvent && !Handled) {
        // Send DGuiEvent only if OnWidgetEvent() return false
        OnGuiEvent(DGuiEvent {Type, Id, WidgetEvent});
    }
}

/**
 * @brief Set the widget position.
 * 
 * @param LeftPos   ->  left position of upper left corner.
 * @param TopPos  ->  right position of upper left corner.
 */
void DGuiWidget::SetPos(int LeftPos, int TopPos)
{
    Bounds.x=LeftPos;
    Bounds.y=TopPos;
    //Changed=true;
};

/**
 * @brief Set the widget size.
 * 
 * @param Width  ->  width of widget.
 * @param Height ->  height of widget.
 */
void DGuiWidget::SetSize(int Width, int Height)
{
    SetWidth(Width);
    SetHeight(Height);
};

/**
 * @brief Set the widget width.
 * 
 * @param Width  ->  width of widget.
 */
void DGuiWidget::SetWidth(int Width)
{
    if (Width < 0) {
        // Parent width
        if (Parent) {
            //Log::debug(TAG,"Parent->GetWidth()");
            Bounds.width=Parent->GetWidth();
        }
        else {
            //Log::debug(TAG,"GetScreenWidth()");
            Bounds.width=GetScreenWidth();
        }
    }
    else if (Width == 0) {
        // Default width
        Bounds.width=DEFAULT_WIDTH;
    }
    else {
        Bounds.width=Width;
    }
};

/**
 * @brief Set the widget height.
 * 
 * @param Height ->  height of widget.
 */
void DGuiWidget::SetHeight(int Height)
{
    if (Height < 0) {
        // Parent height
        if (Parent) {
            Bounds.height=Parent->GetHeight();
        }
        else {
            Bounds.height=GetScreenHeight();
        }
    }
    else if (Height == 0) {
        // Default height
        Bounds.height=DEFAULT_HEIGHT;
    }
    else {
        Bounds.height=Height;
    }
};

/**
 * @brief Set the widget position and size.
 * 
 * @param LeftPos   ->  left position of upper left corner.
 * @param TopPos    ->  right position of upper left corner.
 * @param Width     ->  width of widget.
 * @param Height    ->  height of widget.
 */
void DGuiWidget::SetBounds(int LeftPos, int TopPos, int Width, int Height)
{
    if (LeftPos < 0) {
        switch (LeftPos) {
            case DOCK_LEFT:
                SetDocking(DOCK_LEFT,Width);
                break;
            case DOCK_RIGHT:
                SetDocking(DOCK_RIGHT,Width);
                break;
            case DOCK_CENTER:
                SetDocking(DOCK_HCENTER,Width);
                break;
            default:
                SetPos(LeftPos,TopPos);
                SetSize(Width,Height);
        }
    }
    else if (TopPos < 0) {
        switch (TopPos) {
            case DOCK_TOP:
                SetDocking(DOCK_TOP,Height);
                break;
            case DOCK_BOTTOM:
                SetDocking(DOCK_BOTTOM,Height);
                break;
            case DOCK_CENTER:
                SetDocking(DOCK_VCENTER,Height);
                break;
            default:
                SetPos(LeftPos,TopPos);
                SetSize(Width,Height);
        }
    }
    else {
        SetPos(LeftPos,TopPos);
        SetSize(Width,Height);
    }
};

/**
 * @brief Set the widget position and size as #Rectange object.
 * 
 * @param WidgetBounds  ->  a #Rectangle object with bounds.
 */
void DGuiWidget::SetBounds(Rectangle WidgetBounds)
{
    SetPos(WidgetBounds.x,WidgetBounds.y);
    SetSize(WidgetBounds.width,WidgetBounds.height);
};

/**
 * @brief Set the size of Text (if widget need text).
 * 
 * @param NewSize   ->  the new text size.
 */
void DGuiWidget::SetTextSize(int NewSize, bool ForceAutoSize)
{
    if (NewSize <= 0) {
        NewSize=GuiGetStyle(DEFAULT,TEXT_SIZE);
    }

    Properties.TextSize=NewSize;

    if (ForceAutoSize) {
        AutoSize();
    }
}

void DGuiWidget::SetTextPadding(int NewPadding)
{
    if (NewPadding <= 0) {
        NewPadding=DEFAULT_TEXT_PADDING;
    }

    Properties.TextPadding=NewPadding;
}

void DGuiWidget::SetTextSpacing(int NewSpacing, bool ForceAutoSize)
{
    if (NewSpacing <= 0) {
        NewSpacing=DEFAULT_TEXT_SPACING;
    }

    Properties.TextSpacing=NewSpacing;

    if (ForceAutoSize) {
        AutoSize();
    }
}

void DGuiWidget::AutoSize(void)
{
    // Measure text
    int TextWidth=GetTextWidth(Text,guiFont,Properties.TextSize);
    /// @todo int TextWidth=GetTextWidth(Text,Properties.TextFont,Properties.TextSize);
    // Expand
    int HeightPadding=(Properties.BorderWidth*2)+(Properties.TextPadding*2);
    int WidthPadding=HeightPadding+Properties.TextSpacing;
    SetWidth(TextWidth+WidthPadding);
    SetHeight(Properties.TextSize+HeightPadding);
}

void DGuiWidget::UpdateLabel(void)
{
    if (Label) {
        SetLabel(Label->Text,Label->Properties.TextSize,Properties.LabelSide,Properties.LabelOffset);
    }
}

void DGuiWidget::SetDocking(std::string DockingSideName, int OtherSize) {
    if (DString::CmpNoCase(DockingSideName,DJsonTree::VALUE_BOTTOM)) {
        SetDocking(DGuiWidget::DOCK_BOTTOM,OtherSize);
    }
    else if (DString::CmpNoCase(DockingSideName,DJsonTree::VALUE_TOP)) {
        SetDocking(DGuiWidget::DOCK_TOP,OtherSize);
    }
    else if (DString::CmpNoCase(DockingSideName,DJsonTree::VALUE_LEFT)) {
        SetDocking(DGuiWidget::DOCK_LEFT,OtherSize);
    }
    else if (DString::CmpNoCase(DockingSideName,DJsonTree::VALUE_RIGHT)) {
        SetDocking(DGuiWidget::DOCK_RIGHT,OtherSize);
    }
    else if (DString::CmpNoCase(DockingSideName,DJsonTree::VALUE_CENTER)) {
        SetDocking(DGuiWidget::DOCK_CENTER,OtherSize);
    }
    else {
        Log::warning(TAG,"Docking side unkown: %s",DockingSideName.c_str());
    }
}

/**
 * @brief 
 * 
 * @param DockingPos 
 * @param size:
 * Height if DockingPos is DOCK_LEFT or DOCK_RIGHT.
 * Width if DockingPos is DOCK_BOTTOM or DOCK_TOP.
 */
void DGuiWidget::SetDocking(DDocking DockingSide, int OtherSize) {
    int ParentHeight=0;
    int ParentWidth=0;

    if (Parent == nullptr) {
        ParentHeight=GetScreenHeight();
        ParentWidth=GetScreenWidth();
    }
    else {
        ParentHeight=Parent->GetHeight();
        ParentWidth=Parent->GetWidth();
    }

    // Calculate bounds due to the screen position
    switch (DockingSide) {
        case DOCK_LEFT:
            Bounds.height=ParentHeight;
            Bounds.width=OtherSize <= 0 ? DEFAULT_SIDE_SIZE : OtherSize;
            Bounds.x=0;
            Bounds.y=0;
            break;
        case DOCK_RIGHT:
            Bounds.height=ParentHeight;
            Bounds.width=OtherSize <= 0 ? DEFAULT_SIDE_SIZE : OtherSize;
            Bounds.x=ParentWidth-Bounds.height;
            Bounds.y=0;
            break;
        case DOCK_BOTTOM:
            Bounds.height=OtherSize <= 0 ? DEFAULT_SIDE_SIZE : OtherSize;
            Bounds.width=ParentWidth;
            Bounds.x=0;
            Bounds.y=ParentHeight-Bounds.height;
            break;
        case DOCK_TOP:
            Bounds.height=OtherSize <= 0 ? DEFAULT_SIDE_SIZE : OtherSize;
            Bounds.width=ParentWidth;
            Bounds.x=0;
            Bounds.y=0;
            break;
        case DOCK_HCENTER:
        case DOCK_VCENTER:
        case DOCK_CENTER:
            /// @todo
        default:
            break;
    }
}

void DGuiWidget::SetBorderWidth(uint8_t NewWidth) {
    if (NewWidth == Properties.BorderWidth) {
        return;
    }
    Properties.BorderWidth=NewWidth;
}

void DGuiWidget::SetBorderVisible(bool Visible) {
    Properties.BorderVisible=Visible;
}

DWidgetType DGuiWidget::GetWidgetType(void) {
    return Type;
}

std::string DGuiWidget::GetWidgetTypeName(void) {
    return TypeToName(Type);
}

/**
 * @return current text size.
 */
int DGuiWidget::GetTextSize(void)
{
    return Properties.TextSize;
}

/**
 * @return widget ID as string.
 */
std::string DGuiWidget::GetId(void) {
    return Id;
}

size_t DGuiWidget::GetWidth(void) {
    return Bounds.width;
}

size_t DGuiWidget::GetHeight(void) {
    return Bounds.height;
}

DGuiWidget* DGuiWidget::GetParent(void) {
    return Parent;
}

void DGuiWidget::SetText(std::string NewText, bool ForceAutoSize) {
    if (NewText == Text) {
        return;
    }
    Text=NewText;

    if (ForceAutoSize) {
        AutoSize();
    }
}

const std::string& DGuiWidget::GetText(void) {
    return std::ref(Text);
}

/**
 * @brief Fill widget bounds with default backgound color.
 */
void DGuiWidget::Clear(void) {
    DrawRectangle(Bounds.x,Bounds.y,Bounds.width,Bounds.height,GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
}

/**
 * @brief Set text align value from align string.
 * 
 * @param AlignHoriz 
 * @param AlignVert 
 */
void DGuiWidget::SetTextAlign(std::string AlignHoriz,std::string AlignVert) {
    if (DString::StartsWith(AlignHoriz,DJsonTree::VALUE_LEFT)) {
        Properties.TextAlign.Horiz=DTextAlignH::TEXT_ALIGN_HLEFT;
    }
    else if (DString::StartsWith(AlignHoriz,DJsonTree::VALUE_CENTER)) {
        Properties.TextAlign.Horiz=DTextAlignH::TEXT_ALIGN_HCENTER;
    }
    else if (DString::StartsWith(AlignHoriz,DJsonTree::VALUE_RIGHT)) {
        Properties.TextAlign.Horiz=DTextAlignH::TEXT_ALIGN_HRIGHT;
    }

    if (DString::StartsWith(AlignVert,DJsonTree::VALUE_TOP)) {
        Properties.TextAlign.Vert=DTextAlignV::TEXT_ALIGN_VTOP;
    }
    else if (DString::StartsWith(AlignVert,DJsonTree::VALUE_CENTER)) {
        Properties.TextAlign.Vert=DTextAlignV::TEXT_ALIGN_VCENTER;
    }
    else if (DString::StartsWith(AlignVert,DJsonTree::VALUE_BOTTOM)) {
        Properties.TextAlign.Vert=DTextAlignV::TEXT_ALIGN_VBOTTOM;
    }
}

void DGuiWidget::SetEnabled(bool Enabled) {
    Properties.Enabled=Enabled;
}

void DGuiWidget::SetVisible(bool Visible) {
    Properties.Visible=Visible;
}

/**
 * @brief Set all common proprerties to its current values.
 * 
 */
void DGuiWidget::SetWidgetType(DWidgetType WidgetType)
{
    Type=WidgetType;
    // Text
    /// @todo Properties.TextFont=GuiGetFont();
    Properties.TextColor=GuiGetStyle(Type,TEXT_COLOR_NORMAL);
    Properties.TextPadding=DEFAULT_TEXT_PADDING; //GuiGetStyle(Type, TEXT_PADDING);
    Properties.TextAlign.Horiz=(DTextAlignH) GuiGetStyle(Type,TEXT_ALIGNMENT);
    // From global (DEFAULT) control
    Properties.TextAlign.Vert=(DTextAlignV) GuiGetStyle(DEFAULT,TEXT_ALIGNMENT_VERTICAL);
    Properties.TextSize=GuiGetStyle(DEFAULT,TEXT_SIZE);
    Properties.TextSpacing=DEFAULT_TEXT_SPACING; //GuiGetStyle(DEFAULT,TEXT_SPACING);
    
    // Colors
    Properties.BorderColor=GuiGetStyle(Type,BORDER_COLOR_NORMAL);
    Properties.LineColor=GuiGetStyle(Type,LINE_COLOR);
    Properties.BackGroundColor=GuiGetStyle(Type,BACKGROUND_COLOR);

    // Others
    Properties.BorderWidth=GuiGetStyle(Type,BORDER_WIDTH);
    Properties.BorderVisible=false;
}

// ********************** Methods used during draw **********************
/**
 * @brief Backup current raygui global style values
 * 
 */
void DGuiWidget::BackupCurrentGuiStyle(void) {
    // Text
    TempStyle.TextColor=GuiGetStyle(Type,TEXT_COLOR_NORMAL);
    TempStyle.TextAlign.Horiz=(DTextAlignH) GuiGetStyle(Type,TEXT_ALIGNMENT);
    TempStyle.TextPadding=GuiGetStyle(Type,TEXT_PADDING);
    
    // Borders
    TempStyle.BorderWidth=GuiGetStyle(Type,BORDER_WIDTH);
    TempStyle.BorderColor=GuiGetStyle(Type,BORDER_COLOR_NORMAL);

    // Other Colors 
    TempStyle.LineColor=GuiGetStyle(Type,LINE_COLOR);
    TempStyle.BackGroundColor=GuiGetStyle(Type,BACKGROUND_COLOR);

    // Properties from global (DEFAULT) control
    TempStyle.TextSize=GuiGetStyle(DEFAULT,TEXT_SIZE);
    TempStyle.TextSpacing=GuiGetStyle(DEFAULT,TEXT_SPACING);
    TempStyle.TextAlign.Vert=(DTextAlignV) GuiGetStyle(DEFAULT,TEXT_ALIGNMENT_VERTICAL);
}

void DGuiWidget::UpdateCurrentGuiStyle(void) {
    // Text
    GuiSetStyle(Type,TEXT_COLOR_NORMAL,Properties.TextColor);
    GuiSetStyle(Type,TEXT_ALIGNMENT,Properties.TextAlign.Horiz);
    GuiSetStyle(Type,TEXT_PADDING,Properties.TextPadding);

    // Borders
    GuiSetStyle(Type,BORDER_WIDTH,Properties.BorderWidth);
    GuiSetStyle(Type,BORDER_COLOR_NORMAL,Properties.BorderColor);

    // Other colors
    GuiSetStyle(Type,LINE_COLOR,Properties.LineColor);
    GuiSetStyle(Type,BACKGROUND_COLOR,Properties.BackGroundColor);

    // Properties for all (DEFAULT) control
    GuiSetStyle(DEFAULT,TEXT_SIZE,Properties.TextSize);
    GuiSetStyle(DEFAULT,TEXT_SPACING,Properties.TextSpacing);
    GuiSetStyle(DEFAULT,TEXT_ALIGNMENT_VERTICAL,Properties.TextAlign.Vert);

    Properties.Enabled ? GuiEnable() : GuiDisable();
    
    /* TODO: Global style
        TEXT_WRAP_WORD
        TEXT_LINE_SPACING
    */
}

void DGuiWidget::RestoreCurrentGuiStyle(void) {
    // Text
    GuiSetStyle(Type,TEXT_COLOR_NORMAL,TempStyle.TextColor);
    GuiSetStyle(Type,TEXT_ALIGNMENT,TempStyle.TextAlign.Horiz);
    GuiSetStyle(Type,TEXT_PADDING,TempStyle.TextPadding);
    
    // Borders
    GuiSetStyle(Type,BORDER_WIDTH,TempStyle.BorderWidth);
    GuiSetStyle(Type,BORDER_COLOR_NORMAL,TempStyle.BorderColor);

    // Other colors
    GuiSetStyle(Type,LINE_COLOR,TempStyle.LineColor);
    GuiSetStyle(Type,BACKGROUND_COLOR,TempStyle.BackGroundColor);

    // Properties from global (DEFAULT) control
    GuiSetStyle(DEFAULT,TEXT_SIZE,TempStyle.TextSize);
    GuiSetStyle(DEFAULT,TEXT_SPACING,TempStyle.TextSpacing);
    GuiSetStyle(DEFAULT,TEXT_ALIGNMENT_VERTICAL,TempStyle.TextAlign.Vert);

    TempStyle.Enabled ? GuiEnable() : GuiDisable();
}

void DGuiWidget::Draws(void) {
    if (Properties.Visible) {
        BackupCurrentGuiStyle();
        UpdateCurrentGuiStyle();

        // Draw Widget
        Draw();

        if (Properties.BorderVisible) {
            DrawRectangleLinesEx(Bounds,Properties.BorderWidth,GetColor(Properties.BorderColor));
        }

        if (Label) {
            if (Type != DLABEL) {
                // DGuiLabel cannot have a label (otherwise loops forever and does not make sense)
                // Draw Label (use Draws() to execute complete draw cycle)
                Label->Draws();
            }
        }

        RestoreCurrentGuiStyle();
    }
}

/**
 * @brief Load all gui widgets from file, can be a json formatted or rgl.
 * If file is an rgl saved from rGui app it will be converted in json.
 * @param Filename  ->  file to load.
 * @return true if all widget are loaded.
 * @return false if an error occours or not all widgets are loaded.
 * 
 */
DTree DGuiWidget::ExtractDTree(const fs::path& Filename) {
    Log::debug(TAG,"Loading %s",Filename.c_str());
    std::string JsonFilename;
    if (DString::CmpNoCase(DPath::GetExt(Filename),"rgl")) {
        // Raygui rgl format, convert to json
        JsonFilename=DPath::ChangeExt(Filename,"json",false);
        if (DPath::Exists_StdFs(JsonFilename.c_str())) {
            Log::warning(TAG,"found a Json version of same layout, assume to use it");
        }
        else {
            if (!DPath::Exists(Filename)) {
                Log::error(TAG,"Layout file does not exist: %s",Filename.c_str());
                return std::move(DTree());
            }
            Log::debug(TAG,"rgl file need to be converted to json");
            JsonFilename=RglToJson(Filename);
        }
    }
    else {
        JsonFilename=Filename;
    }

    if (JsonFilename.empty()) {
        Log::error(TAG,"Not valid layout file...");
        return std::move(DTree());
    }

    DPreferences Json(JsonFilename,false);
    if (!Json.IsReady()) {
        Log::error(TAG,"Error parsing %s: %s",JsonFilename.c_str(), Json.GetLastStatus().c_str());
        return std::move(DTree());
    }

    DTree dt=Json.GetTree();
    return (std::move(dt));
}

/**
 * @brief Convert an rgl layout file in its json version.
 * Result file have same name but json extension.
 * 
 * @param Filename  ->  file to convert.
 * @return new filename on succesfully conversion otherwise an empty string.
 */
std::string DGuiWidget::RglToJson(std::string Filename) {
    Log::debug(TAG,"Converting %s",Filename.c_str());

    // Load rgl file
    auto FileStream=std::ifstream(Filename,std::ios::in);
    if (!FileStream.is_open()) {
        Log::error(TAG,"%s cannot be opened.",Filename.c_str());
        return std::string();
    }

    // Create a new empty json
    DPreferences JsonFile(DPath::ChangeExt(Filename,"json",false));
    if (!JsonFile.IsReady()) {
        Log::error(TAG,"%s cannot be created.",Filename.c_str());
        return std::string();
    }

    // Set to Container type
    JsonFile.WriteString(DJsonTree::ITEM_TYPE,TypeToName(DCONTAINER));
    JsonFile.WriteString(DJsonTree::ITEM_NAME,fs::path(Filename).stem());

    // Add Widgets
    std::vector<DRglControl> ControlsList;
    for (std::string Line; std::getline(FileStream,Line);) {
        // Ignore all lines that not starts with 'r', 'a' or 'c'
        if (Line.empty()) {
            continue;
        }
        else if (Line[0] == 'r') {
            // Ref. window:    r <x> <y> <width> <height>
            // TODO
        }
        else if (Line[0] == 'a') {
            // Anchor info:    a <id> <name> <posx> <posy> <enabled>
            // TODO
        }
        else if (Line[0] == 'c') {
            // Control
            DRglControl RglControl=DecodeRglLine(Line);
            if (RglControl.WidgetType == DWidgetType::UNKNOWN) {
                return std::string();
            }
            ControlsList.emplace_back(std::move(RglControl));
        }
    }

    // Write controls to json
    for (size_t ixC=0; ixC<ControlsList.size(); ixC++) {
        auto Control=ControlsList[ixC];
        std::string TreeName=DJsonTree::SEC_CHILDREN+"."+Control.Name;
        //JsonFile.WriteString(TreeName,DJsonTree::ITEM_ANCHOR_ID,Widget->Properties.AnchorId); // deprecated
        JsonFile.WriteString(TreeName,DJsonTree::ITEM_TYPE,TypeToName(Control.WidgetType));
        JsonFile.WriteString(TreeName,DJsonTree::ITEM_TEXT,Control.Text);
        TreeName=TreeName+DOT+DJsonTree::SEC_BOUNDS;
        JsonFile.WriteInteger(TreeName,DJsonTree::ITEM_LEFT,Control.Bounds.x);
        JsonFile.WriteInteger(TreeName,DJsonTree::ITEM_TOP,Control.Bounds.y);
        JsonFile.WriteInteger(TreeName,DJsonTree::ITEM_WIDTH,Control.Bounds.width);
        JsonFile.WriteInteger(TreeName,DJsonTree::ITEM_HEIGHT,Control.Bounds.height);
    }
    if (!JsonFile.Save()) {
        Log::error(TAG,"Error saving %s file: %s",JsonFile.GetFilename().c_str(),JsonFile.GetLastStatus().c_str());
        return std::string();
    }

    return(JsonFile.GetFilename());
}

/**
 * @brief Generate a DLayoutControl from text line ans save it to json.
 * 
 * @param Line      ->  Text line to decode.
 * @param JsonFile  ->  A DPreferences instance reference.
 * @return true on success.
 * @return false if some error occours.
 */
DRglControl DGuiWidget::DecodeRglLine(std::string Line) {
    DRglControl RglControl;
    std::vector<std::string> LineItems;

    DCsv::ReadCSVRow(LineItems,Line,' ');
    if (LineItems.size() < DRglLayout::ControlLineItemsCount-1) {
        Log::error(TAG,"Line fields nr %d wrong",LineItems.size());
        return RglControl;
    }

    if (LineItems.size() == DRglLayout::ControlLineItemsCount-1) {
        LineItems.emplace_back(std::string());
    }

    // ** Load base info to instantiate a DWidget **
    // Type
    int WidgetType=DString::ToInt(LineItems[DRglLayout::CTRL_IX_TYPE]);
    WidgetType-=3; // TODO: For now it is a trick to match with RayGui type
    if (!WidgetTypes.contains((DWidgetType) WidgetType)) {
        Log::error(TAG,"Control type unkown: %d",WidgetType);
        return RglControl;
    }
    RglControl.WidgetType=(DWidgetType) WidgetType;

    // Bounds
    RglControl.Bounds.x=DString::ToInt(LineItems[DRglLayout::CTRL_IX_X]);
    RglControl.Bounds.y=DString::ToInt(LineItems[DRglLayout::CTRL_IX_Y]);
    RglControl.Bounds.width=DString::ToInt(LineItems[DRglLayout::CTRL_IX_WIDTH]);
    RglControl.Bounds.height=DString::ToInt(LineItems[DRglLayout::CTRL_IX_HEIGHT]);

    // Text
    for (size_t ixV=DRglLayout::ControlLineItemsCount-1; ixV<LineItems.size(); ixV++) {
        RglControl.Text.append(LineItems[ixV]);
        if (ixV < LineItems.size()-1) {
            RglControl.Text.append(" ");
        }
    }

    RglControl.Name=LineItems[DRglLayout::CTRL_IX_NAME];
    //Widget->Properties.AnchorId=LineItems[CTRL_IX_ANCHOR_ID]; // deprecated

    return (RglControl);
}

DWidgetType DGuiWidget::NameToType(const std::string& WidgetTypeName)
{
    DWidgetType WidgetType=DWidgetType::UNKNOWN;
    for (auto item : WidgetTypes) {
        if (item.second == WidgetTypeName) {
            return item.first;
        }
    }
    return WidgetType;
}

std::string DGuiWidget::TypeToName(DWidgetType WidgetType)
{
    std::string WidgetTypeName;
    if (WidgetTypes.contains(WidgetType)) {
        WidgetTypeName=WidgetTypes.at(WidgetType);
    }
    return WidgetTypeName;
}

/**
 * @brief Decode a string that rappresent a color in its integer value.
 * 
 * Supported formats:
 * Hex:
 *  "#rrggbb"    -> RGB
 *  "#rrggbbaa"  -> RGB + alpha
 *  "0x...."
 *  "Hex...."
 * 
 * @todo
 *  [ ] Name    Red
 *  [ ] Rgb     rgb(255, 0, 0)
 *  [x] Hex     #ff0000
 *  [ ] Hsl     hsl(0, 100%, 50%)
 *  [ ] Hwb     hwb(0, 0%, 0%)
 *  [ ] Cmyk	cmyk(0%, 100%, 100%, 0%)
 *  [ ] Ncol	R0, 0%, 0%
 * 
 * @param ColorString string in hex color format.
 * @return the integer rappresentation of a color in Raylib.
 */
unsigned int DGuiWidget::ColorStringToInt(std::string ColorString)
{
    std::vector<std::string> Patterns={"#", "0x", "HEX"};
    for (std::string& Pattern : Patterns) {
        if (DString::StartsWith(ColorString, Pattern)) {
            std::string s;
            if (ColorString.size() >= Pattern.size()+8) {
                s=ColorString.substr(Pattern.size(),8);
            }
            else if (ColorString.size() >= Pattern.size()+6) {
                s=ColorString.substr(Pattern.size(),6);
                s.append("FF");
            }
            if (!s.empty()) {
                std::stringstream ss;
                ss << std::hex << s;
                unsigned int value;
                ss >> value;
                return value;
            }
        }
    }
    return 0;
}

bool DGuiWidget::IsReady(void)
{
    return Ready;
}

std::string DGuiWidget::GetLastError(void)
{
    return LastError;
}

// **************** Wrappers raygui implementation access ***************
GuiState DGuiWidget::GetGuiState(void)
{
    return guiState;
}

int DGuiWidget::GetGuiTextBoxCursorIndex(void)
{
    return textBoxCursorIndex;
}

void DGuiWidget::SetGuiTextBoxCursorIndex(int cursorIndex)
{
    textBoxCursorIndex=cursorIndex;
}

bool DGuiWidget::IsGuiLocked(void)
{
    return guiLocked;
}

// Gui get text width considering icon
int DGuiWidget::GetTextWidth(std::string TextStr, Font TextFont, float FontSize)
{
    #if !defined(ICON_TEXT_PADDING)
        #define ICON_TEXT_PADDING   4
    #endif

    // detect icon pattern
    int textIconOffset = 0;
    if (!TextStr.empty()) {
        if (TextStr[0] == '#') {
            for (int i = 1; (i < 5) && (TextStr[i] != '\0'); i++) {
                if (TextStr[i] == '#') {
                    textIconOffset = i;
                    break;
                }
            }
        }

        const char *text=&(TextStr.c_str()[textIconOffset]);

        int textSize=MeasureText(text,FontSize);
        /// @todo misurare icona

        return textSize;
/*
        // Custom MeasureText() implementation
        // Make sure guiFont is set, GuiGetStyle() initializes it lazynessly
        //float fontSize = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
        //Vector2 textSize = { 0 };
        if ((TextFont.texture.id > 0) && (text != NULL))
        {
            // Get size in bytes of text, considering end of line and line break
            int size = 0;
            for (int i = 0; i < MAX_LINE_BUFFER_SIZE; i++)
            {
                if ((text[i] != '\0') && (text[i] != '\n')) size++;
                else break;
            }

            float scaleFactor = FontSize/(float)TextFont.baseSize;
            textSize.y = (float)TextFont.baseSize*scaleFactor;
            float glyphWidth = 0.0f;

            for (int i = 0, codepointSize = 0; i < size; i += codepointSize)
            {
                int codepoint = GetCodepointNext(&text[i], &codepointSize);
                int codepointIndex = GetGlyphIndex(TextFont, codepoint);

                if (TextFont.glyphs[codepointIndex].advanceX == 0) glyphWidth = ((float)TextFont.recs[codepointIndex].width*scaleFactor);
                else glyphWidth = ((float)TextFont.glyphs[codepointIndex].advanceX*scaleFactor);

                textSize.x += (glyphWidth + (float)GuiGetStyle(DEFAULT, TEXT_SPACING));
            }
        }

        if (textIconOffset > 0) textSize.x += (RAYGUI_ICON_SIZE - ICON_TEXT_PADDING);
*/
    }

    //return (int)textSize.x;
}

void DGuiWidget::RayGuiDrawRectangle(Rectangle rec, int borderWidth, Color borderColor, Color color)
{
    GuiDrawRectangle(rec,borderWidth,borderColor,color);
}

void DGuiWidget::RayGuiDrawText(const char *text, Rectangle textBounds, int alignment, Color tint)
{
    GuiDrawText(text,textBounds,alignment,tint);
}

float DGuiWidget::GetGuiAlpha(void)
{
    return guiAlpha;
}