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
    SetText(WidgetTree.ReadString(DJsonTree::ITEM_TEXT,""));

    // ** Read widget properties **
    // Text size
    Properties.TextSize=WidgetTree.ReadInteger(DJsonTree::ITEM_TEXT_SIZE,-1);
    // Text align
    std::string AlignHoriz=WidgetTree.ReadString(DJsonTree::ITEM_TEXT_ALIGN_H,"");
    std::string AlignVert=WidgetTree.ReadString(DJsonTree::ITEM_TEXT_ALIGN_V,"");
    SetTextAlign(AlignHoriz,AlignVert);
    
// @todo (sono già gestite da Widget->Draw)
//    Properties.TextColor=GuiGetStyle(Type,TEXT_COLOR_NORMAL);
//    Properties.TextSpacing=GuiGetStyle(Type,TEXT_SPACING);
    
    // Borders
//    Properties.BorderWidth=GuiGetStyle(Type,BORDER_WIDTH);
//   Properties.BorderColor=GuiGetStyle(Type,BORDER_COLOR_NORMAL);

    // Other Colors
//    Properties.LineColor=GuiGetStyle(Type,LINE_COLOR);
//    Properties.BackGroundColor=GuiGetStyle(Type,BACKGROUND_COLOR);


    // AnchorId
    // Widget->Properties.AnchorId=WidgetTree.ReadString(DJsonTree::ITEM_ANCHOR_ID,""); // deprecated

    // Possible docking position
    std::string DockingSide=WidgetTree.ReadString(DJsonTree::ITEM_DOCKING,DJsonTree::ITEM_SIDE,"");
    int DockingSize=WidgetTree.ReadInteger(DJsonTree::ITEM_DOCKING,DJsonTree::ITEM_SIZE,-1);
    if (!DockingSide.empty()) {
        SetDocking(DockingSide,DockingSize);
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

    // ShowBorder
    Properties.ShowBorder=WidgetTree.ReadBool(DJsonTree::ITEM_SHOW_BORDER,false);

    // Border width
    int BorderWidth=WidgetTree.ReadInteger(DJsonTree::ITEM_BORDER_WIDTH, 0);
    if (BorderWidth > 0) Properties.BorderWidth=BorderWidth;

    return true;
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
    Bounds=WidgetBounds;
};

/**
 * @brief Set the size of Text (if widget need text).
 * 
 * @param NewSize   ->  the new text size.
 */
void DGuiWidget::SetTextSize(int NewSize)
{
    Properties.TextSize=NewSize;
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

void DGuiWidget::SetText(std::string NewText) {
    if (NewText == Text) {
        return;
    }
    Text=NewText;
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
 */
void DGuiWidget::SetWidgetType(DWidgetType WidgetType)
{
    Type=WidgetType;
    // Text
    Properties.TextColor=GuiGetStyle(Type,TEXT_COLOR_NORMAL);
    Properties.TextPadding=GuiGetStyle(Type, TEXT_PADDING);
    //int align=GuiGetStyle(Type,TEXT_ALIGNMENT);
    Properties.TextAlign.Horiz=(DTextAlignH) GuiGetStyle(Type,TEXT_ALIGNMENT);
    // from global (DEFAULT) control
    Properties.TextAlign.Vert=(DTextAlignV) GuiGetStyle(DEFAULT,TEXT_ALIGNMENT_VERTICAL);
    Properties.TextSize=GuiGetStyle(DEFAULT,TEXT_SIZE);
    Properties.TextSpacing=GuiGetStyle(DEFAULT,TEXT_SPACING);
    
    // Borders
    Properties.BorderWidth=GuiGetStyle(Type,BORDER_WIDTH);
    Properties.BorderColor=GuiGetStyle(Type,BORDER_COLOR_NORMAL);

    // Other Colors
    Properties.LineColor=GuiGetStyle(Type,LINE_COLOR);
    Properties.BackGroundColor=GuiGetStyle(Type,BACKGROUND_COLOR);
}

// ********************** Methods used during draw **********************
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

void DGuiWidget::SetWidgetGuiStyle(void) {
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

    // Properties from global (DEFAULT) control
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
        SetWidgetGuiStyle();

        // Draw Widget
        Draw();

        // Draw Borders
        if(Properties.ShowBorder) {
            DrawRectangleLinesEx(Bounds,Properties.BorderWidth,GetColor(Properties.BorderColor));
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
DTree DGuiWidget::ExtractDTree(const std::string& Filename) {
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

int DGuiWidget::GetGuiTextWidth(const char *text)
{
    return GetTextWidth(text);
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