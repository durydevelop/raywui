#include "DGuiWidget.h"
#include <DGuiContainer.h>
#include <DGuiButton.h>
#include <DGuiEdit.h>
#include <DGuiLabel.h>
#include <DGuiStatusBar.h>
#include <DGuiImage.h>
#include <DGuiToggleSlide.h>
#include <DGuiToggleSwitch.h>
#include <DGuiLed.h>
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
#define DEFAULT_AUTO_WIDTH 10
#define DEFAULT_AUTO_HEIGHT 10

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
    // Type
    SetWidgetType(WidgetType);
    // Bounds
    SetBounds(LeftPos,TopPos,WidgetWidth,WidgetHeight);
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

DGuiWidget::DGuiWidget(DTools::DTree& WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback)
{
    // Create id
    GenerateId();
    // Parent
    SetParent(ParentWidget);
    // Events callback
    OnWidgetEvent=EventCallback;
    // Init widget from DTree
    Ready=InitFromTree(std::ref(WidgetTree));
    // Initial Name
//    if (Name.empty()) {
//        Name=TypeToName(Type)+Id;
//    }
}

DGuiWidget::~DGuiWidget()
{
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
    std::string WidgetTypeName=WidgetTree.ReadString(DJsonTree::ITEM_TYPE,"");
    if (DString::CmpNoCase(WidgetTypeName,DJsonTree::VALUE_JSON)) {
        // Widget is in a separate json
        std::string JsonFilename=WidgetTree.ReadString(DJsonTree::ITEM_NAME,"");
        return New(JsonFilename,ParentWidget,EventCallback);
    }

    DWidgetType WidgetType=NameToType(WidgetTypeName);
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
    else if (WidgetType == DLED) {
        DGuiLed *Led=new DGuiLed(WidgetTree,ParentWidget,EventCallback);
        return Led;
    }

    if (WidgetTypes.contains(WidgetType)) {
        Log::error(TAG,"Widget type %s not implemented",WidgetTypes.at(WidgetType).c_str());
    }
    else {
        Log::error(TAG,"Unknown Widget Type %s",WidgetTypeName.c_str());
    }
    return nullptr;
}

bool DGuiWidget::InitFromTree(DTools::DTree& WidgetTree)
{
    std::string StrValue; // for generic string read in tree

    if (WidgetTree.IsEmpty()) {
        LastError="InitFromTree() WidgetTree is empty";
        return false;
    }

    // ** Load base info to instantiate a DWidget **
    // Type
    DWidgetType WidgetType=DWidgetType::UNKNOWN;
    std::string WidgetTypeName=WidgetTree.ReadString(DJsonTree::ITEM_TYPE,"");
    SetWidgetType(NameToType(WidgetTypeName));

    // Name
    Name=WidgetTree.ReadString(DJsonTree::ITEM_NAME,"");
    if (Name.empty()) {
        Name=TypeToName(Type)+Id;
        Log::warning(TAG,"Widget %s with id %s has no name, default created: %s",WidgetTypeName.c_str(),Id.c_str(),Name.c_str());
    }

    // Bounds
    Rectangle WidgetBounds;
    WidgetBounds.x=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS,DJsonTree::ITEM_LEFT,0);
  /*
    if (WidgetBounds.x < 0) {
        StrValue=WidgetTree.ReadString(DJsonTree::ITEM_BOUNDS,DJsonTree::ITEM_LEFT,"");
        if (DString::CmpNoCase(StrValue,DJsonTree::VALUE_CENTER)) {
            WidgetBounds.x=DDocking::DOCK_HCENTER;
        }
    }
*/
    WidgetBounds.y=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS,DJsonTree::ITEM_TOP,0);
/*
    if (WidgetBounds.y < 0) {
        StrValue=WidgetTree.ReadString(DJsonTree::ITEM_BOUNDS,DJsonTree::ITEM_TOP,"");
        if (DString::CmpNoCase(StrValue,DJsonTree::VALUE_CENTER)) {
            WidgetBounds.y=DDocking::DOCK_VCENTER;
        }
    }
*/
    WidgetBounds.width=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS,DJsonTree::ITEM_WIDTH,DSizeMode::SIZE_AUTO);
    WidgetBounds.height=WidgetTree.ReadInteger(DJsonTree::ITEM_BOUNDS,DJsonTree::ITEM_HEIGHT,DSizeMode::SIZE_AUTO);
    SetBounds(WidgetBounds);

    // Border visible
    Properties.BorderVisible=WidgetTree.ReadBool(DJsonTree::ITEM_BORDER_VISIBLE,false);

    // Border width
    Properties.BorderWidth=WidgetTree.ReadInteger(DJsonTree::ITEM_BORDER_WIDTH,1);

    // Border color
    StrValue=WidgetTree.ReadString(DJsonTree::ITEM_BORDER_COLOR,"");
    if (!StrValue.empty()) {
        Properties.BorderColor=ColorStringToInt(StrValue);
    }
    else {
        Properties.BorderColor=ColorToInt(BLACK);
    }

    // Line color
    int IntValue=WidgetTree.ReadInteger(DJsonTree::ITEM_LINE_COLOR,0);
    if (IntValue > 0) {
        Properties.LineColor=IntValue;
    }

    // Background color
    IntValue=WidgetTree.ReadInteger(DJsonTree::ITEM_BACKGROUND_COLOR,0);
    if (IntValue > 0) {
        Properties.BackGroundColor=IntValue;
    }

    // Anchors (if AnchorToSide is empty, ANCHOR_NONE is set)
    std::string AnchorTree=DJsonTree::ITEM_BOUNDS + DTree::DEFAULT_TRANSLATOR + DJsonTree::ITEM_ANCHORS;
    if (!WidgetTree.HasChildren(AnchorTree)) {
        /// @todo Deprecating
        AnchorTree=DJsonTree::ITEM_BOUNDS + DTree::DEFAULT_TRANSLATOR + DJsonTree::ITEM_ANCHOR;
    }
    // Side of anchored widget
    std::string AnchorToSide=WidgetTree.ReadString(AnchorTree,DJsonTree::ITEM_SIDE_OF,"");
    // Align to anchored widget
    std::string AlignToSide=WidgetTree.ReadString(AnchorTree,DJsonTree::ITEM_SIDE_ALIGN,"");
    // Name of anchored widget
    std::string WidgetName=WidgetTree.ReadString(AnchorTree,DJsonTree::ITEM_NAME,"");
    // Offset from anchored widget
    int AnchorOffset=WidgetTree.ReadInteger(AnchorTree,DJsonTree::ITEM_OFFSET,0);
    SetAnchor(NameToAnchor(AnchorToSide),NameToAlign(AlignToSide,ALIGN_LEFT),WidgetName,AnchorOffset);

    // Parent aling
    std::string ParentAlignTree=DJsonTree::ITEM_BOUNDS + DTree::DEFAULT_TRANSLATOR +DJsonTree::ITEM_PARENT_ALIGN;
    std::vector<std::string> AlignNames;
    WidgetTree.ReadNames(ParentAlignTree,AlignNames);
    for (std::string& AlignName : AlignNames) {
        AddParentAlign(AlignName,WidgetTree.ReadInteger(ParentAlignTree,AlignName,0),false);
    }
    UpdateParentAligns();

    // Docking position
    std::string DockingSide=WidgetTree.ReadString(DJsonTree::ITEM_DOCKING,DJsonTree::ITEM_SIDE,"");
    if (!DockingSide.empty()) {
        int DockingSize=WidgetTree.ReadInteger(DJsonTree::ITEM_DOCKING,DJsonTree::ITEM_SIZE,-1);
        SetDocking(DockingSide,DockingSize);
    }

    // Outside label
    
    if (WidgetTree.HasChildren(DJsonTree::ITEM_LABEL_EXT)) {
        // Has external label
        DTree LabelTree=WidgetTree.GetTree(DJsonTree::ITEM_LABEL_EXT);
        SetLabelExt(LabelTree);
    }

    // Enabled
    Properties.Enabled=WidgetTree.ReadBool(DJsonTree::ITEM_ENABLED,true);

    // Visible
    Properties.Visible=WidgetTree.ReadBool(DJsonTree::ITEM_VISIBLE,true);

    return true;
}

void DGuiWidget::SetLabelExt(DTools::DTree& LabelTree)
{
    std::string LabelText=LabelTree.ReadString(DJsonTree::ITEM_TEXT,"");
    
    // Font size. Default(-1): parent text size
    int FontSize=LabelTree.ReadInteger(DJsonTree::ITEM_FONT_SIZE,-1);

    // Side
    std::string LabelSide=LabelTree.ReadString(DJsonTree::ITEM_SIDE,"");

    // Offsets
    int OffsetX=LabelTree.ReadInteger(DJsonTree::ITEM_OFFSET_X,-1);
    int OffsetY=LabelTree.ReadInteger(DJsonTree::ITEM_OFFSET_Y,-1);
    if (OffsetX == -1 && OffsetY == -1) {
        SetLabelExt(LabelText,FontSize,NameToSide(LabelSide,SIDE_RIGHT),LabelTree.ReadInteger(DJsonTree::ITEM_OFFSET,0));
    }
    else {
        SetLabelExt(LabelText,FontSize,NameToSide(LabelSide,SIDE_RIGHT),OffsetX,OffsetY);
    }
}

/**
 * @brief Set external label of the widget.
 * N.B.
 * with this method you can set the anchor left, right, bottom, top with only one offset value.
 * If you want to set custom x,y offset call SetLabelExt(std::string LabelText, int FontSize, DSide LabelSide, int OffsetX, int OffsetY).
 * 
 * @param LabelText     ->  Text of the label.
 * @param FontSize      ->  Label font size.
 * @param LabelSide     ->  One of DSide values.
 * @param SideOffset    ->  Offset from label side.
 */
void DGuiWidget::SetLabelExt(std::string LabelText, int FontSize, DSide LabelSide, uint SideOffset)
{
    SetLabelExt(LabelText,FontSize,LabelSide,SideOffset,SideOffset);
}

/**
 * @brief Set external label of the widget.
 * 
 * @param LabelText     ->  Text of the label.
 * @param FontSize      ->  Label font size.
 * @param LabelSide     ->  One of DSide values.
 * @param OffsetX       ->  X offset from upper-left corner of widget.
 * @param OffsetY       ->  Y offset from upper-left corner of widget.
 */
void DGuiWidget::SetLabelExt(std::string LabelText, int FontSize, DSide LabelSide, int OffsetX, int OffsetY)
{
    // Text
    if (LabelText.empty()) {
        if (LabelExt.Label) {
            delete LabelExt.Label;
            LabelExt.Label=nullptr;
        }
        return;
    }
    
    // Font size
    if (FontSize == 0) {
        // Use default
        FontSize=GuiGetStyle(DEFAULT,TEXT_SIZE);
    }
    else if (FontSize > 0 && FontSize < GuiGetStyle(DEFAULT,TEXT_SIZE)) {
        FontSize=GuiGetStyle(DEFAULT,TEXT_SIZE);
    }

    if (!LabelExt.Label) {
        // Not yet created
        LabelExt.Label=new DGuiLabel(nullptr);
    }

    LabelExt.OffsetX=OffsetX;
    LabelExt.OffsetY=OffsetY;
    LabelExt.Side=LabelSide;
    LabelExt.Label->SetFontSize(FontSize,false);
    LabelExt.Label->SetText(LabelText,false);
    LabelExt.Label->Update();

    UpdateLabelExt();
}

void DGuiWidget::UpdateLabelExt(void)
{
    if (LabelExt.Label) {
        // Calculate position
        switch (LabelExt.Side) {
            case SIDE_BOTTOM:
                LabelExt.Label->Bounds.x=Bounds.x+((Bounds.width-LabelExt.Label->Bounds.width)/2); // Center horizzontally
                LabelExt.Label->Bounds.y=Bounds.y+Bounds.height+LabelExt.OffsetY;
                break;
            case SIDE_TOP:
                LabelExt.Label->Bounds.x=Bounds.x+((Bounds.width-LabelExt.Label->Bounds.width)/2); // Center horizzontally
                LabelExt.Label->Bounds.y=Bounds.y-LabelExt.Label->Bounds.height-LabelExt.OffsetY;
                break;
            case SIDE_LEFT:
                LabelExt.Label->Bounds.x=Bounds.x-LabelExt.Label->Bounds.width-LabelExt.OffsetX;
                LabelExt.Label->Bounds.y=Bounds.y+((Bounds.height-LabelExt.Label->Bounds.height)/2); // Center vertically
                break;
            case SIDE_RIGHT:
                LabelExt.Label->Bounds.x=Bounds.x+Bounds.width+LabelExt.OffsetX;
                LabelExt.Label->Bounds.y=Bounds.y+((Bounds.height-LabelExt.Label->Bounds.height)/2); // Center vertically
                break;
            case SIDE_CUSTOM:
                LabelExt.Label->Bounds.x=Bounds.x+LabelExt.OffsetX;
                LabelExt.Label->Bounds.y=Bounds.y+LabelExt.OffsetY;
            default:
                break;
        }
    }
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

DDocking DGuiWidget::NameToDocking(const std::string& SideName, DDocking Default)
{
    for (auto item : Dockings) {
        if (item.second == SideName) {
            return item.first;
        }
    }
    return Default;
}

std::string DGuiWidget::DockingToName(DDocking DockingPos)
{
    std::string DockingSideName;
    if (Dockings.contains(DockingPos)) {
        DockingSideName=Dockings.at(DockingPos);
    }
    return DockingSideName;
}

DSide DGuiWidget::NameToSide(const std::string& SideName, DSide Default)
{
    for (auto item : Sides) {
        if (item.second == SideName) {
            return item.first;
        }
    }
    return Default;
}

std::string DGuiWidget::SideToName(DSide Side)
{
    std::string SideName;
    if (Sides.contains(Side)) {
        SideName=Sides.at(Side);
    }
    return SideName;
}

DAnchorSide DGuiWidget::NameToAnchor(const std::string& AnchorSideName)
{
    DAnchorSide AnchorSide=ANCHOR_NONE;
    for (auto item : AnchorSides) {
        if (item.second == AnchorSideName) {
            return item.first;
        }
    }
    return AnchorSide;
}

std::string DGuiWidget::AnchorToName(DAnchorSide AnchorSide)
{
    std::string AnchorSideName;
    if (AnchorSides.contains(AnchorSide)) {
        AnchorSideName=AnchorSides.at(AnchorSide);
    }
    return AnchorSideName;
}

DAlign DGuiWidget::NameToAlign(const std::string& AlignName, DAlign Default)
{
    for (auto item : Aligns) {
        if (item.second == AlignName) {
            return item.first;
        }
    }
    return Default;
}

std::string DGuiWidget::AlignToName(DAlign Align)
{
    std::string AlignName;
    if (Aligns.contains(Align)) {
        AlignName=Aligns.at(Align);
    }
    return AlignName;
}

void DGuiWidget::GenerateId(void)
{
    // Create id
    std::ostringstream ss;
    ss << (void const *)this;
    Id=ss.str();
    Uid=reinterpret_cast<std::uintptr_t>(this);
}

void DGuiWidget::SetParent(DGuiWidget *ParentContainer) {
    Parent=ParentContainer;
    if (Parent) { // && Parent->GetWidgetType() == DCONTAINER) {
        // Shift into parent container
//        Bounds.x+=Parent->Bounds.x;
//        Bounds.y+=Parent->Bounds.y;
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
//        SetWidth(Bounds.width);
//        SetHeight(Bounds.height);
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
        OnGuiEvent(DGuiEvent {Type, Id, Uid, WidgetEvent});
    }
}

/**
 * @brief Set the widget position (relative to te parent container).
 * 
 * @param LeftPos   ->  left position of upper left corner.
 * @param TopPos    ->  right position of upper left corner.
 */
void DGuiWidget::SetPos(int LeftPos, int TopPos)
{
    if (LeftPos < 0) {
        switch (LeftPos) {
            case DOCK_LEFT:
                SetDocking(DOCK_LEFT,Bounds.width);
                break;
            case DOCK_RIGHT:
                SetDocking(DOCK_RIGHT,Bounds.width);
                break;
            case DOCK_CENTER:
                SetDocking(DOCK_CENTER,Bounds.width);
                break;
            case DOCK_HCENTER:
                SetDocking(DOCK_HCENTER,Bounds.width);
                break;
            default:
                break;
        }
    }
    else if (TopPos < 0) {
        switch (TopPos) {
            case DOCK_TOP:
                SetDocking(DOCK_TOP,Bounds.height);
                break;
            case DOCK_BOTTOM:
                SetDocking(DOCK_BOTTOM,Bounds.height);
                break;
            case DOCK_CENTER:
                SetDocking(DOCK_CENTER,Bounds.height);
                break;
            case DOCK_VCENTER:
                SetDocking(DOCK_VCENTER,Bounds.height);
                break;
            default:
                SetPos(LeftPos,TopPos);
        }
    }
    else {
        Bounds.x=LeftPos;
        Bounds.y=TopPos;
    }
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
 * @param Width  -> width of widget:
 *                      <  0    Parent width
 *                      == 0    Default
 *                      >  0    Set
 */
void DGuiWidget::SetWidth(int Width)
{
    bool NeedUpate=false;
    if (Bounds.width != Width) {
            NeedUpate=true;
    }

    if (Width == DSizeMode::SIZE_PARENT) {
        // Parent width
        Properties.HSizeMode=DSizeMode::SIZE_PARENT;
        if (Parent) {
            //Log::debug(TAG,"Parent->GetWidth()");
            Bounds.width=Parent->GetWidth();
        }
        else {
            //Log::debug(TAG,"GetScreenWidth()");
            Bounds.width=GetScreenWidth();
        }
    }
    else if (Width == DSizeMode::SIZE_DEFAULT) {
        // Default width
        Properties.HSizeMode=DSizeMode::SIZE_DEFAULT;
        Bounds.width=DEFAULT_WIDTH;
    }
    else if (Width == DSizeMode::SIZE_AUTO) {
        // Auto size due to text
        Properties.HSizeMode=DSizeMode::SIZE_AUTO;
    }
    else {
        // Fix size (Width > 0)
        Properties.HSizeMode=DSizeMode::SIZE_FIX;
        Bounds.width=Width;
    }

    if (NeedUpate) {
        Update();
    }
};

/**
 * @brief Set the widget height.
 * 
 * @param Height ->  height of widget.
 */
void DGuiWidget::SetHeight(int Height)
{
    bool NeedUpdate=false;
    if (Bounds.height != Height) {
            NeedUpdate=true;
    }

    if (Height == DSizeMode::SIZE_PARENT) {
        // Parent height
        Properties.VSizeMode=DSizeMode::SIZE_PARENT;
        if (Parent) {
            Bounds.height=Parent->GetHeight();
        }
        else {
            Bounds.height=GetScreenHeight();
        }
    }
    else if (Height == DSizeMode::SIZE_DEFAULT) {
        // Default height
        Properties.VSizeMode=DSizeMode::SIZE_DEFAULT;
        Bounds.height=DEFAULT_HEIGHT;
    }
    else if (Height == DSizeMode::SIZE_AUTO) {
        // Auto size due to the text
        Properties.VSizeMode=DSizeMode::SIZE_AUTO;
    }
    else {
        // Fix Mode (Height > 0)
        Properties.VSizeMode=DSizeMode::SIZE_FIX;
        Bounds.height=Height;
    }

    if (NeedUpdate) {
        Update();
    }
};

/**
 * @brief If this method is not implemented by a sub class, set width to the DEFAULT_AUTO_WIDTH value
 */
void DGuiWidget::AutoWidth(void)
{
    Bounds.width=DEFAULT_AUTO_WIDTH;
}

/**
 * @brief If this method is not implemented by a sub class, set height to the DEFAULT_AUTO_HEIGHT value
 */
void DGuiWidget::AutoHeight(void)
{
    Bounds.height=DEFAULT_AUTO_HEIGHT;
}

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
    SetPos(LeftPos,TopPos);
    SetSize(Width,Height);
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
}

Rectangle DGuiWidget::GetAbsBounds(void)
{
    Rectangle AbsBounds=Bounds;
    if (Parent) {
        Rectangle ParentBounds=Parent->GetAbsBounds();
        AbsBounds.x+=ParentBounds.x;
        AbsBounds.y+=ParentBounds.y;
    }
    return AbsBounds;
}

/**
 * @brief 
 * 
 * @param WidgetName 
 */
bool DGuiWidget::SetAnchor(DGuiWidget::DAnchor Anchor)
{
    return SetAnchor(Anchor.AnchorToSide,Anchor.AlignToSide,Anchor.WidgetName,Anchor.AnchorOffset);
}

bool DGuiWidget::SetAnchor(DAnchorSide AnchorToSide, DAlign AlignToSide, std::string WidgetName, int AnchorOffset)
{
    Properties.Anchor.AnchorToSide=AnchorToSide;
    Properties.Anchor.AlignToSide=AlignToSide;
    Properties.Anchor.WidgetName=WidgetName;
    Properties.Anchor.AnchorOffset=AnchorOffset;

    return UpdateAnchor();
}

/**
 * @brief Move widget position (only) due to the Properties.Anchor content.
 * Set position to right/left/bottom/top side **OUT** of an other widget.
 * Works only if:
 * - This widget has parent container.
 * - The widget in parent container is found.
 * N.B.
 * If use a Dtree for layout, be sure that this widget is after RightOf widget.
 * 
 * @return false 
 */
bool DGuiWidget::UpdateAnchor(void)
{
    if (Properties.Anchor.WidgetName.empty() || Properties.Anchor.AnchorToSide == ANCHOR_NONE) {
        return false;
    }

    
    if (!Parent) {
        Log::warning(TAG,"Cannot set Anchor of widget <%s> because it has not parent container", Name.c_str());
        return false;
    }

    DGuiWidget *Widget=Parent->FindWidgetByName(Properties.Anchor.WidgetName);
    if (!Widget) {
        Log::error(TAG,"Cannot set Anchor because container <%s> has not widget <%s>",Parent->Name.c_str(),Properties.Anchor.WidgetName.c_str());
        return false;
    }

    switch (Properties.Anchor.AnchorToSide) {
        case ANCHOR_RIGHT_OF:
            // Set Position to the right of Widget
            Log::debug(TAG,"Setting anchor to RightOf <%s>",Properties.Anchor.WidgetName.c_str());
            Bounds.x=Widget->Bounds.x+Widget->Bounds.width+Properties.Anchor.AnchorOffset;
            if (Properties.Anchor.AlignToSide == ALIGN_BOTTOM) {
                // Align to Bottom
                Bounds.y=Widget->Bounds.y+Widget->Bounds.height-Bounds.height;
            }
            else if (Properties.Anchor.AlignToSide == ALIGN_TOP) {
                // Align to top
                Bounds.y=Widget->Bounds.y;
            }
            else {
                // Default align to center
                Bounds.y=Widget->Bounds.y+abs((Widget->Bounds.height-Bounds.height)/2);
            }
            break;
        case ANCHOR_LEFT_OF:
            // Set Position to the left of Widget
            Log::debug(TAG,"Setting anchor to LeftOf <%s>",Properties.Anchor.WidgetName.c_str());
            Bounds.x=Widget->Bounds.x-Widget->Bounds.width-Properties.Anchor.AnchorOffset;
            if (Properties.Anchor.AlignToSide == ALIGN_BOTTOM) {
                // Align to Bottom
                Bounds.y=Widget->Bounds.y+Widget->Bounds.height-Bounds.height;
            }
            else if (Properties.Anchor.AlignToSide == ALIGN_TOP) {
                // Align to top
                Bounds.y=Widget->Bounds.y;
            }
            else {
                // Default align to center
                Bounds.y=Widget->Bounds.y+abs((Widget->Bounds.height-Bounds.height)/2);
            }
            break;
        case ANCHOR_BOTTOM_OF:
            // Set Position to the bottom of Widget
            Log::debug(TAG,"Setting anchor to BottomOf <%s>",Properties.Anchor.WidgetName.c_str());
            if (Properties.Anchor.AlignToSide == ALIGN_RIGHT) {
                // Align to right
                Bounds.x=Widget->Bounds.x+Widget->Bounds.width-Bounds.width;
            }
            else if (Properties.Anchor.AlignToSide == ALIGN_LEFT) {
                // Align to left
                Bounds.x=Widget->Bounds.x;
            }
            else {
                // Default align to center
                Bounds.x=Widget->Bounds.x+abs((Widget->Bounds.width-Bounds.width)/2);
            }
            Bounds.y=Widget->Bounds.y+Widget->Bounds.height+Properties.Anchor.AnchorOffset;
            break;
        case ANCHOR_TOP_OF:
            // Set Position to the top of Widget
            Log::debug(TAG,"Setting anchor to TopOf <%s>",Properties.Anchor.WidgetName.c_str());
            if (Properties.Anchor.AlignToSide == ALIGN_RIGHT) {
                // Align to right
                Bounds.x=Widget->Bounds.x+Widget->Bounds.width-Bounds.width;
            }
            else if (Properties.Anchor.AlignToSide == ALIGN_LEFT) {
                // Align to right
                Bounds.x=Widget->Bounds.x;
            }
            else {
                // Default align to center
                Bounds.x=Widget->Bounds.x+abs((Widget->Bounds.width-Bounds.width)/2);
            }
            Bounds.y=Widget->Bounds.y-Widget->Bounds.height-Properties.Anchor.AnchorOffset;
            break;
        default:
            // Should never happens...
            Log::debug(TAG,"Anchor value <%d> of widget <%s> NOT supprted",Properties.Anchor.AnchorToSide,Properties.Anchor.WidgetName.c_str());
            return false;
    }

    return true;
}

void DGuiWidget::SetParentAligns(std::map<std::string,int> AlignList, bool ForceUpdate)
{
    for (auto& [AlignName,AlignOffset] : AlignList) {
        AddParentAlign(AlignName,AlignOffset,false);
    }

    if (ForceUpdate) {
        UpdateParentAligns();
    }
}

void DGuiWidget::AddParentAlign(std::string AlignName, int AlignOffset, bool ForceUpdate) {
    DAlign Align=NameToAlign(AlignName,DAlign::ALIGH_NONE);
    if (Align != DAlign::ALIGH_NONE) {
        Properties.ParentAligns.emplace(std::make_pair(Align,AlignOffset));
    }
    
    if (ForceUpdate) {
        UpdateParentAligns();
    }
}

/**
 * @brief 
 * - ALIGN_HCENTER and ALIGN_VCENTER always have priority.
 * - If ALIGN_CENTER is set, only last found alignment is used.
 * - ALIGN_CENTER stand alone is equivalent of ALIGN_HCENTER | ALIGN_VCENTER.
 * 
 */
void DGuiWidget::UpdateParentAligns(void)
{
    if (!Parent || Properties.ParentAligns.empty()) {
        return;
    }

    bool Centered=false;
    bool HCentered=false;
    bool VCentered=false;

    if (Properties.ParentAligns.contains(DAlign::ALIGN_CENTER) && Parent) {
        Centered=true;
        if (Properties.ParentAligns.size() == 1) {
            HCentered=true;
            VCentered=true;
        }
    }

    if (Properties.ParentAligns.contains(DAlign::ALIGN_LEFT)) {
        Bounds.x=Properties.ParentAligns[DAlign::ALIGN_LEFT];
        if (Centered) {
            VCentered=true;
        }
    }

    if (Properties.ParentAligns.contains(DAlign::ALIGN_RIGHT)) {
        Bounds.x=Parent->Bounds.width-Bounds.width-Properties.ParentAligns[DAlign::ALIGN_RIGHT];
        if (Centered) {
            VCentered=true;
        }
    }

    if (Properties.ParentAligns.contains(DAlign::ALIGN_BOTTOM)) {
        Bounds.y=Parent->Bounds.height-Bounds.height-Properties.ParentAligns[DAlign::ALIGN_BOTTOM];
        if (Centered) {
            HCentered=true;
        }
    }

    if (Properties.ParentAligns.contains(DAlign::ALIGN_TOP)) {
        Bounds.y=Properties.ParentAligns[DAlign::ALIGN_TOP];
        if (Centered) {
            HCentered=true;
        }
    }

    if (Properties.ParentAligns.contains(DAlign::ALIGN_HCENTER) && Parent) {
        HCentered=true;
    }
    if (Properties.ParentAligns.contains(DAlign::ALIGN_VCENTER) && Parent) {
        VCentered=true;
    }

    if (HCentered) {
        Bounds.x=(Parent->Bounds.width-Bounds.width)/2;
    }
    if (VCentered) {
        Bounds.y=(Parent->Bounds.height-Bounds.height)/2;
    }
}

void DGuiWidget::Update(void)
{
    if (Properties.HSizeMode == DSizeMode::SIZE_AUTO) {
        AutoWidth();
    }

    if (Properties.VSizeMode == DSizeMode::SIZE_AUTO) {
        AutoHeight();
    }
    
    UpdateParentAligns();
    UpdateAnchor();
    UpdateLabelExt();
}

void DGuiWidget::SetDocking(std::string DockingSideName, int OtherSize) {
    DDocking Docking=NameToDocking(DockingSideName,DOCK_CENTER);
    SetDocking(Docking,OtherSize);
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
            Bounds.x=ParentWidth-Bounds.width;
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
            Bounds.x=(ParentWidth-Bounds.width)/2;
            break;
        case DOCK_VCENTER:
            Bounds.y=(ParentHeight-Bounds.height)/2;
            break;
        case DOCK_CENTER:
            Bounds.x=(ParentWidth-Bounds.width)/2;
            Bounds.y=(ParentHeight-Bounds.height)/2;
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

void DGuiWidget::SetBorderColor(Color BorderColor) {
    Properties.BorderColor=ColorToInt(BorderColor);
}

DWidgetType DGuiWidget::GetWidgetType(void) {
    return Type;
}

std::string DGuiWidget::GetWidgetTypeName(void) {
    return TypeToName(Type);
}

/**
 * @return widget ID as string.
 */
std::string DGuiWidget::GetId(void) {
    return Id;
}

DGuiWidget* DGuiWidget::FindWidgetByName(std::string WidgetName)
{
    return nullptr;
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

Rectangle DGuiWidget::GetTextBounds(const DText& Text)
{
    // Calculate text bounds
    int TextOffset=Properties.BorderWidth+Text.Padding;
    Rectangle AbsBounds=GetAbsBounds();
    Rectangle TextBounds;
    TextBounds.x=AbsBounds.x+TextOffset;
    TextBounds.y=AbsBounds.y+TextOffset;

    if (Properties.HSizeMode == DSizeMode::SIZE_AUTO) {
        // Measure text width
        int TextWidth=GetTextWidth(Text.Prefix+Text.Text+Text.Suffix,Text.TextFont,Text.FontSize, Text.Spacing);
        TextBounds.width=TextWidth;
    }
    else {
        TextBounds.width=AbsBounds.width-(TextOffset*2);
    }

    if (Properties.VSizeMode == DSizeMode::SIZE_AUTO) {
        // Measure text height
        TextBounds.height=Text.FontSize;
    }
    else {
        TextBounds.height=AbsBounds.height-(TextOffset*2);
    }

    return TextBounds;
}

/**
 * @brief Fill widget bounds with default backgound color.
 */
void DGuiWidget::Clear(void) {
    DrawRectangle(Bounds.x,Bounds.y,Bounds.width,Bounds.height,GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
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
    
    // Colors
    Properties.BorderColor=ColorToInt(BLACK);
    Properties.LineColor=GuiGetStyle(Type,LINE_COLOR);
    Properties.BackGroundColor=GuiGetStyle(Type,BACKGROUND_COLOR);

    // Others
    Properties.BorderWidth=GuiGetStyle(Type,BORDER_WIDTH);
    Properties.BorderVisible=false;

    // Anchor
    Properties.Anchor.AnchorToSide=DAnchorSide::ANCHOR_NONE;
    Properties.Anchor.AlignToSide=DAlign::ALIGN_LEFT;
    Properties.Anchor.AnchorOffset=0;

    // Width mode
    Properties.HSizeMode=DSizeMode::SIZE_AUTO;
    Properties.VSizeMode=DSizeMode::SIZE_AUTO;

    DebugView=false;
}

// ********************** Methods used during draw **********************
/**
 * @brief Backup current raygui text style (called by widget that have DText).
 */
void DGuiWidget::BackupCurrentTextStyle(DText& TempText)
{
    TempText.TextColor=GuiGetStyle(Type,TEXT_COLOR_NORMAL);
    TempText.Align.Horiz=(DTextAlignH) GuiGetStyle(Type,TEXT_ALIGNMENT);
    TempText.Padding=GuiGetStyle(Type,TEXT_PADDING);
    TempText.FontSize=GuiGetStyle(DEFAULT,TEXT_SIZE);
    TempText.Spacing=GuiGetStyle(DEFAULT,TEXT_SPACING);
    TempText.Align.Vert=(DTextAlignV) GuiGetStyle(DEFAULT,TEXT_ALIGNMENT_VERTICAL);
}

/**
 * @brief Set all raygui Text styles for current widget (called by widget that have DText).
 */
void DGuiWidget::UpdateCurrentTextStyle(DText& Text)
{
    GuiSetStyle(Type,TEXT_COLOR_NORMAL,Text.TextColor);
    GuiSetStyle(Type,TEXT_ALIGNMENT,Text.Align.Horiz);
    GuiSetStyle(Type,TEXT_PADDING,Text.Padding);
    GuiSetStyle(DEFAULT,TEXT_SIZE,Text.FontSize);
    GuiSetStyle(DEFAULT,TEXT_SPACING,Text.Spacing);
    GuiSetStyle(DEFAULT,TEXT_ALIGNMENT_VERTICAL,Text.Align.Vert);

    /* TODO: Global style
        TEXT_WRAP_WORD
        TEXT_LINE_SPACING
    */
}

/**
 * @brief Restore previous saved current text sytles (called by widget that have DText).
 */
void DGuiWidget::RestoreCurrentTextStyle(DText& TempText)
{
    GuiSetStyle(Type,TEXT_COLOR_NORMAL,TempText.TextColor);
    GuiSetStyle(Type,TEXT_ALIGNMENT,TempText.Align.Horiz);
    GuiSetStyle(Type,TEXT_PADDING,TempText.Padding);
    GuiSetStyle(DEFAULT,TEXT_SIZE,TempText.FontSize);
    GuiSetStyle(DEFAULT,TEXT_SPACING,TempText.Spacing);
    GuiSetStyle(DEFAULT,TEXT_ALIGNMENT_VERTICAL,TempText.Align.Vert);
}

/**
 * @brief Backup current raygui global style values in a temporary DProperty
 */
void DGuiWidget::BackupCurrentGuiStyle(void) {
    // Borders
    TempStyle.BorderWidth=GuiGetStyle(Type,BORDER_WIDTH);
    TempStyle.BorderColor=GuiGetStyle(Type,BORDER_COLOR_NORMAL);

    // Other Colors 
    TempStyle.LineColor=GuiGetStyle(Type,LINE_COLOR);
    TempStyle.BackGroundColor=GuiGetStyle(Type,BACKGROUND_COLOR);
    TempStyle.Enabled=Properties.Enabled;
}

/**
 * @brief Set all raygui styles for current widget from DProperty
 */
void DGuiWidget::UpdateCurrentGuiStyle(void) {
    // Borders
    GuiSetStyle(Type,BORDER_WIDTH,Properties.BorderWidth);
    GuiSetStyle(Type,BORDER_COLOR_NORMAL,Properties.BorderColor);

    // Other colors
    GuiSetStyle(Type,LINE_COLOR,Properties.LineColor);
    GuiSetStyle(Type,BACKGROUND_COLOR,Properties.BackGroundColor);

    Properties.Enabled ? GuiEnable() : GuiDisable();
}

/**
 * @brief Restore previous saved current sytles
 */
void DGuiWidget::RestoreCurrentGuiStyle(void) {
    // Borders
    GuiSetStyle(Type,BORDER_WIDTH,TempStyle.BorderWidth);
    GuiSetStyle(Type,BORDER_COLOR_NORMAL,TempStyle.BorderColor);

    // Other colors
    GuiSetStyle(Type,LINE_COLOR,TempStyle.LineColor);
    GuiSetStyle(Type,BACKGROUND_COLOR,TempStyle.BackGroundColor);

    TempStyle.Enabled ? GuiEnable() : GuiDisable();
}

void DGuiWidget::Draws(void) {
    if (Properties.Visible) {
        // Store current global raygui styles
        BackupCurrentGuiStyle();
        // Set global raygui style from this->Properties
        UpdateCurrentGuiStyle();

        if (CheckCollisionPointRec(GetMousePosition(),GetAbsBounds()) && Focus.Enable) {
            Focus.Enabled=true;
        }
        else {
            Focus.Enabled=false;
        }

        // Draw Widget
        Draw();

        if (Properties.BorderVisible || DebugView) {
            auto b=GetAbsBounds();
            DrawRectangleLinesEx(b,Properties.BorderWidth,GetColor(Properties.BorderColor));
        }

        if (LabelExt.Label) {
            if (Type != DLABEL) {
                // DGuiLabel cannot have a label (otherwise loops forever and does not make sense)
                // Draw Label (use Draws() to execute complete draw cycle)
                LabelExt.Label->Draws();
            }
        }

        // Restore previous saved global raygui styles
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

void DGuiWidget::SetDebugView(bool Enabled)
{
    DebugView=Enabled;
    
    if (LabelExt.Label) {
        LabelExt.Label->SetDebugView(Enabled);
    }
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

void DGuiWidget::RayGuiDrawRectangle(Rectangle Bounds, int BorderWidth, Color BorderColor, Color Tint)
{
    GuiDrawRectangle(Bounds,BorderWidth,BorderColor,Tint);
}

void DGuiWidget::RayGuiDrawText(const char *TextStr, Rectangle TextBounds, int Alignment, Color Tint)
{
    GuiDrawText(TextStr,TextBounds,Alignment,Tint);
}

void DGuiWidget::RayGuiDrawText(std::string TextStr, Rectangle TextBounds, DTextAlign Alignment, Color Tint)
{
    GuiDrawText(TextStr.c_str(),TextBounds,Alignment.Horiz,Tint);
}

float DGuiWidget::GetGuiAlpha(void)
{
    return guiAlpha;
}

// ********************** Static convenient methods *********************

// Gui get text width considering icon
int DGuiWidget::GetTextWidth(std::string TextStr, Font TextFont, float FontSize, int TextSpacing)
{
    #if !defined(ICON_TEXT_PADDING)
        #define ICON_TEXT_PADDING   4
    #endif

    if (TextStr.empty()) {
        return 0; /// @todo return -1 ?
    }

    // detect icon pattern
    int textIconOffset = 0;
    int textSize=0;
    
    if (TextStr[0] == '#') {
        for (int i = 1; (i < 5) && (TextStr[i] != '\0'); i++) {
            if (TextStr[i] == '#') {
                textIconOffset = i;
                break;
            }
        }
    }

    const char *text=&(TextStr.c_str()[textIconOffset]);

    textSize=MeasureText(text,FontSize)+(TextSpacing*TextStr.size());

    return textSize;
}