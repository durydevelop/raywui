#include "DGuiStatusBar.h"
//#define RAYGUI_IMPLEMENTATION
//#include <raygui.h>
#include "raywui_log.h"
#include <DGuiContainer.h>
#include <dpplib/DPreferences.h>

const char TAG[14]="DGuiStatusBar";

DGuiStatusBar::DGuiStatusBar(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget) : DGuiWidget(DSTATUSBAR,LeftPos,TopPos,ControlWidth,ControlHeight,ParentWidget)
{
}

DGuiStatusBar::DGuiStatusBar(Rectangle WidgetBounds, DGuiWidget *ParentWidget) : DGuiWidget(DSTATUSBAR,WidgetBounds,ParentWidget)
{
}

DGuiStatusBar::DGuiStatusBar(DDocking DockingPos, int OtherSize, DGuiWidget *ParentWidget) : DGuiWidget(DSTATUSBAR,DockingPos,OtherSize,ParentWidget)
{
}

DGuiStatusBar::DGuiStatusBar(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(WidgetTree,ParentWidget,EventCallback)
{
    FinalizeFromTree(WidgetTree);
    Ready=(Type == DSTATUSBAR);
}

DGuiStatusBar::DGuiStatusBar(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiStatusBar(std::move(ExtractDTree(LayoutFilename)),ParentWidget,EventCallback)
{

}

void DGuiStatusBar::FinalizeFromTree(DTools::DTree& WidgetTree)
{
    // ** Read class specific properties **
    // StatusBar items        
    DTools::DTree SubItems=WidgetTree.GetTree(DJsonTree::SEC_STATUSBAR_ITEMS);
    std::vector<std::string> Items;
    SubItems.ReadNames(Items);
    Log::debug(TAG,"StatusBar have %d items",Items.size());
    // Add Items
    for (std::string& ItemName : Items) {
        int Left=0;
        int Width=-1; // auto
        std::string DockingSide=SubItems.ReadString(ItemName+"."+DJsonTree::ITEM_DOCKING,DJsonTree::ITEM_SIDE,"");
        if (DockingSide == DJsonTree::VALUE_LEFT) {
            //Log::debug(TAG,"Statusbar item docked to the left");
            Left=DGuiWidget::DOCK_LEFT;
            Width=SubItems.ReadInteger(ItemName+"."+DJsonTree::ITEM_DOCKING,DJsonTree::ITEM_SIZE,DGuiWidget::WIDTH_AUTO);
        }
        else if (DockingSide == DJsonTree::VALUE_RIGHT) {
            //Log::debug(TAG,"Statusbar item docked to the right");
            Left=DGuiWidget::DOCK_RIGHT;
            Width=SubItems.ReadInteger(ItemName+"."+DJsonTree::ITEM_DOCKING,DJsonTree::ITEM_SIZE,DGuiWidget::WIDTH_AUTO);
        }
        else if (DockingSide == DJsonTree::VALUE_CENTER) {
            //Log::debug(TAG,"Statusbar item docked to center");
            Left=DGuiWidget::DOCK_CENTER;
            Width=SubItems.ReadInteger(ItemName+"."+DJsonTree::ITEM_DOCKING,DJsonTree::ITEM_SIZE,DGuiWidget::WIDTH_AUTO);
        }
        else {
            Left=SubItems.ReadInteger(ItemName,DJsonTree::ITEM_LEFT,DGuiWidget::DOCK_LEFT);
            if (Left < DGuiWidget::DOCK_CENTER) {
                //Log::warning(TAG,"Left value of %d is not supported, set to 0");
                Left=0;
            }
            Width=SubItems.ReadInteger(ItemName,DJsonTree::ITEM_WIDTH,DGuiWidget::WIDTH_AUTO);
        }
        // Text
        std::string Text=SubItems.ReadString(ItemName,DJsonTree::ITEM_TEXT,"");
        // Text align
        std::string AlHoriz=SubItems.ReadString(ItemName,DJsonTree::ITEM_TEXT_ALIGN_H,"");
        std::string AlVert=SubItems.ReadString(ItemName,DJsonTree::ITEM_TEXT_ALIGN_V,"");
        

        // Add Item
        //Log::debug(TAG,"%s: Left=%d, Width=%d, Text=<%s>",ItemName.c_str(),Left,Width,Text.c_str());
        AddItem(ItemName,Left,Width,Text);
        // Set align
        DGuiStatusBar::DStatusBarItem *Item=GetItem(ItemName);
        if (Item) {
            Item->SetTextAlign(AlHoriz,AlVert);
        }
    }
}

/*
DGuiStatusBar::DGuiStatusBar(DDocking DockingPos, int SideSize, DGuiWidget *ParentWidget) : DGuiWidget(DSTATUSBAR,0,0,0,SideSize,ParentWidget)
{
    DEFAULT_SIDE_SIZE=16;
    SetDocking(DockingPos,SideSize);
}
*/
/*
DGuiStatusBar* DGuiStatusBar::LoadFromJson(std::string JsonFilename, DGuiWidget *Parent) {
    DTools::DPreferences Json(JsonFilename);
    if (!Json.IsReady()) {
        Log::error(TAG,"%s cannot be opened: %s",JsonFilename.c_str(),Json.GetLastStatus().c_str());
        return nullptr;
    }
    
    DGuiWidget *Widget=DGuiWidget::New(Json.GetRootTree(),Parent);
    if (Widget->GetWidgetType() != DWidgetType::DSTATUSBAR) {
        Log::error(TAG,"Not a StatusBar widget: %s",Widget->GetWidgetTypeName().c_str());
        return nullptr;
    }

    return (DGuiStatusBar *) Widget;
}
*/
/*
void DGuiStatusBar::SetParent(DGuiWidget *ParentContainer) {
    Parent=ParentContainer;
    if (Parent) {
        if (Bounds.width <= 0) {
            Bounds.width=Parent->GetWidth();
        }
        if (Bounds.height <= 0) {
            Bounds.height=Parent->GetHeight();
        }
    }
}
*/
/**
 * @brief 
 * 
 * @param ItemName 
 * @param LeftPos ->    -1 = FROM_LEFT -2 FROM_RIGHT
 * @param Width 
 * @param ItemText 
 */
void DGuiStatusBar::AddItem(std::string ItemName, int LeftPos, int Width, std::string ItemText) {
    if (Width == WIDTH_AUTO) {
        // Auto width
        Width=MeasureText(ItemText.c_str(),Properties.TextSize);
        //Width=(ItemText.size()*Properties.TextSize); //+((ItemText.size()-1)*Properties.TextSpacing);
        if (Width == 0) {
            // Default width
            Width=100;
        }
        else {
            // Add Padding space
            Width+=Properties.TextSize;
        }
    }
    
    switch (LeftPos) {
        case DOCK_LEFT:
            LeftPos=0;
            break;
        case DOCK_RIGHT:
            LeftPos=Bounds.x+Bounds.width-Width;
            break;
        case DOCK_CENTER:
            LeftPos=(Bounds.width/2)-(Width/2);
            break;
        default:
            break;
    }

    DStatusBarItem Item(Rectangle{Bounds.x+LeftPos, Bounds.y, (float) Width, Bounds.height},this);
    if (!ItemText.empty()) {
        Item.SetText(ItemText.c_str());
    }
    Item.Properties.TextSize=Properties.TextSize;
    Item.Properties.TextSpacing=Properties.TextSpacing;
    Item.Properties.TextPadding=2;
    Items.emplace(ItemName,std::move(Item));
}

DGuiStatusBar::DStatusBarItem* DGuiStatusBar::GetItem(std::string ItemName) {
    if (Items.contains(ItemName)) {
        return &Items.at(ItemName);
    }
    else {
        return nullptr;
    }
}

void DGuiStatusBar::SetItemText(std::string ItemName, std::string ItemText) {
    if (Items.contains(ItemName)) {
        Items.at(ItemName).SetText(ItemText);
    }
}

/**
 * @brief Draw the status bar.
 */
void DGuiStatusBar::Draw()
{
    GuiStatusBar(Bounds, "");
    for (auto [Name,Item] : Items) {
        Item.Draws();
        DrawLineEx(Vector2{Item.Bounds.x,Item.Bounds.y},Vector2{Item.Bounds.x,Item.Bounds.y+Item.Bounds.height},Properties.BorderWidth,GetColor(Properties.BorderColor));
        DrawLineEx(Vector2{Item.Bounds.x+Item.Bounds.width,Item.Bounds.y},Vector2{Item.Bounds.x+Item.Bounds.width,Item.Bounds.y+Item.Bounds.height},Properties.BorderWidth,GetColor(Properties.BorderColor));
    }
}