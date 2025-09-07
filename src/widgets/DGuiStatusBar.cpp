#include "DGuiStatusBar.h"
//#define RAYGUI_IMPLEMENTATION
//#include <raygui.h>
#include "raywui_log.h"
//#include <DGuiContainer.h>
#include <dpplib/DPreferences.h>

const char TAG[14]="DGuiStatusBar";

DGuiStatusBar::DGuiStatusBar(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget) : DGuiWidget(DSTATUSBAR,LeftPos,TopPos,ControlWidth,ControlHeight,ParentWidget)
{
    InitDefault();
}

DGuiStatusBar::DGuiStatusBar(Rectangle WidgetBounds, DGuiWidget *ParentWidget) : DGuiWidget(DSTATUSBAR,WidgetBounds,ParentWidget)
{
    InitDefault();
}

DGuiStatusBar::DGuiStatusBar(DDocking DockingPos, int OtherSize, DGuiWidget *ParentWidget) : DGuiWidget(DSTATUSBAR,DockingPos,OtherSize,ParentWidget)
{
    InitDefault();
}

DGuiStatusBar::DGuiStatusBar(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(std::ref(WidgetTree),ParentWidget,EventCallback)
{
    InitDefault();
    FinalizeFromTree(WidgetTree);
    Ready=(Type == DSTATUSBAR);
}

DGuiStatusBar::DGuiStatusBar(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiStatusBar(std::move(ExtractDTree(LayoutFilename)),ParentWidget,EventCallback)
{

}

void DGuiStatusBar::InitDefault(void)
{

}

/**
 * @brief 
 * 
 * @param WidgetTree 
 * 
 * Only some Proprerties are allowed in DStatusBarItem
    "Left": 200,
    "Width": 300,
    or
    "Docking": {
                "Side": "Left",
                "Size": 200
            },

    "Text":"...",
    "TextAlignH": "Left"
 */
void DGuiStatusBar::FinalizeFromTree(DTools::DTree& WidgetTree)
{
    // ** Read class specific properties **
    // Text used for style
    Text.InitFromTree(WidgetTree);
    // StatusBar items        
    DTools::DTree SubItems=WidgetTree.GetTree(DJsonTree::SEC_ITEMS);
    std::vector<std::string> Items;
    SubItems.ReadNames(Items);
    Log::debug(TAG,"StatusBar have %d items",Items.size());
    // Read items
    for (std::string& ItemName : Items) {
        // Only "Left" or 
        int Left;
        int Width;
        std::string DockingSide=SubItems.ReadString(ItemName+"."+DJsonTree::ITEM_DOCKING,DJsonTree::ITEM_SIDE,"");
        if (DockingSide == DJsonTree::VALUE_LEFT) {
            //Log::debug(TAG,"Statusbar item docked to the left");
            Left=DOCK_LEFT;
            Width=SubItems.ReadInteger(ItemName+"."+DJsonTree::ITEM_DOCKING,DJsonTree::ITEM_SIZE,DSizeMode::SIZE_AUTO);
        }
        else if (DockingSide == DJsonTree::VALUE_RIGHT) {
            //Log::debug(TAG,"Statusbar item docked to the right");
            Left=DOCK_RIGHT;
            Width=SubItems.ReadInteger(ItemName+"."+DJsonTree::ITEM_DOCKING,DJsonTree::ITEM_SIZE,DSizeMode::SIZE_AUTO);
        }
        else if (DockingSide == DJsonTree::VALUE_CENTER) {
            //Log::debug(TAG,"Statusbar item docked to center");
            Left=DOCK_CENTER;
            Width=SubItems.ReadInteger(ItemName+"."+DJsonTree::ITEM_DOCKING,DJsonTree::ITEM_SIZE,DSizeMode::SIZE_AUTO);
        }
        else {
            Left=SubItems.ReadInteger(ItemName,DJsonTree::ITEM_LEFT,DOCK_LEFT);
            if (Left < DOCK_CENTER) {
                //Log::warning(TAG,"Left value of %d is not supported, set to 0");
                Left=0;
            }
            Width=SubItems.ReadInteger(ItemName,DJsonTree::ITEM_WIDTH,DSizeMode::SIZE_AUTO);
        }

        // Text
        std::string Text=SubItems.ReadString(ItemName,DJsonTree::ITEM_TEXT,"");

        // Text align
        std::string StrAlignH=SubItems.ReadString(ItemName,DJsonTree::ITEM_TEXT_ALIGN_H,"");
        
        // Add Item
        AddItem(ItemName,Left,Width,Text,StrAlignH);
    }
}

/**
 * @brief 
 * 
 * @param ItemName 
 * @param LeftPos
 * @param Width 
 * @param ItemText 
 */
void DGuiStatusBar::AddItem(std::string ItemName, int LeftPos, int ItemWidth, std::string ItemText, std::string StrAlignH) {
    DStatusBarItem Item(this);
    // Copy text style
    Item.Text.Assign(Text);
    // Set textalign
    Item.Text.SetAlignH(StrAlignH);

    // Set item text
    if (!ItemText.empty()) {
        Item.SetText(ItemText,false);
    }
    // Set Bounds
    Item.SetWidth(ItemWidth);
    Item.SetHeight(Bounds.height);
    Item.SetPos(LeftPos,0);
    
    
    

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
        Items.at(ItemName).SetText(ItemText,false);
    }
}

/**
 * @brief Draw the status bar.
 */
void DGuiStatusBar::Draw()
{
    Rectangle AbsBounds=GetAbsBounds();
    GuiStatusBar(AbsBounds, "");
    for (auto [Name,Item] : Items) {
        Item.Draws();
        // Vertical line separators
        Rectangle ItemBounds=Item.GetAbsBounds();
        DrawLineEx(Vector2{ItemBounds.x,ItemBounds.y},Vector2{ItemBounds.x,ItemBounds.y+ItemBounds.height},Properties.BorderWidth,GetColor(Properties.BorderColor));
        DrawLineEx(Vector2{ItemBounds.x+ItemBounds.width,ItemBounds.y},Vector2{ItemBounds.x+ItemBounds.width,ItemBounds.y+ItemBounds.height},Properties.BorderWidth,GetColor(Properties.BorderColor));
    }
}