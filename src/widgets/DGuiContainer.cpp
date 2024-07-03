#include "DGuiContainer.h"
#include <DGuiApp.h>
#include <DGuiLabel.h>
#include <DGuiEdit.h>
#include <DGuiButton.h>
#include <DGuiStatusBar.h>
#include "raywui_log.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <dpplib/DPath.h>
#include <dpplib/DString.h>
#include <dpplib/DCsv.h>
#include <typeinfo>

#define DOT DPreferences::DEFAULT_TRANSLATOR
/*
const std::string VALUE_BOTTOM="Bottom";
const std::string VALUE_TOP="Top";
const std::string VALUE_LEFT="Left";
const std::string VALUE_RIGHT="Right";
const std::string VALUE_CENTER="Center";
*/
using namespace DTools;

const char TAG[14]="DGuiContainer";

DGuiContainer::DGuiContainer(int LeftPos, int TopPos, int ContainerWidth, int ContainerHeight, DGuiWidget *ParentWidget) : DGuiWidget(DCONTAINER,LeftPos,TopPos,ContainerWidth,ContainerHeight,ParentWidget)
{
    //Owner=AppOwner;
    //if (!LayoutFilename.empty()) {
    //    LoadFromFile(LayoutFilename);
    //}
}

DGuiContainer::DGuiContainer(Rectangle ContainerBounds, DGuiWidget *ParentWidget) : DGuiWidget(DCONTAINER,ContainerBounds,ParentWidget)
{
    
}

DGuiContainer::DGuiContainer(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(WidgetTree,ParentWidget,EventCallback)
{
    FinalizeFromTree(WidgetTree);
    Ready=(Type == DCONTAINER);
}

DGuiContainer::DGuiContainer(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiContainer(std::move(ExtractDTree(LayoutFilename)),ParentWidget,EventCallback)
{
}

DGuiContainer::~DGuiContainer()
{
    Log::debug(TAG,"~DGuiContainer() %s %d children",Name.c_str(),Children.size());
    for (auto [id,widget] : Children) {
        //Log(DLOG_DEBUG,"delete %s",widget->Name.c_str());
        delete widget;
    }
}

void DGuiContainer::FinalizeFromTree(DTools::DTree& WidgetTree)
{
    //DGuiWidget::InitFromTree(WidgetTree);
    // ** Read class specific properties **
    // Widgets
    std::vector<DTree> Children=WidgetTree.ReadArrayTrees(DJsonTree::SEC_CHILDREN);
    Log::debug(TAG,"Container %s have %d widgets",Name.c_str(),Children.size());
    for (auto Child : Children) {
        AddWidget(&Child);
    }
}

void DGuiContainer::SetOnGuiEvent(OnGuiEventCallback Callback) {
    for (auto &[id,widget] : Children) {
        widget->SetOnGuiEvent(Callback);
    }
}
/*
DGuiWidget* DGuiContainer::AddWidget(DWidgetType WidgetType, int LeftPos, int TopPos, int WidgetWidth, int WidgetHeight, std::string Text)
{
    auto Widget=DGuiWidget::New(WidgetType,LeftPos,TopPos,WidgetWidth,WidgetHeight,Text, this);
    return (AddWidget(Widget));
}

DGuiWidget* DGuiContainer::AddWidget(DWidgetType WidgetType, Rectangle WidgetBounds, std::string Text)
{
    auto Widget=DGuiWidget::New(WidgetType,WidgetBounds.x,WidgetBounds.y,WidgetBounds.width,WidgetBounds.height,Text,this);
    return (AddWidget(Widget));
}
*/
DGuiWidget* DGuiContainer::AddWidget(DGuiWidget *Widget)
{
    if (Widget) {
        auto p=std::make_pair(Widget->GetId(),Widget);
        Children.emplace_back(p);
    }
    return Widget;
}

DGuiWidget* DGuiContainer::AddWidget(DTools::DTree *WidgetTree)
{
    DGuiWidget *Widget=New(*WidgetTree,Parent,OnWidgetEvent);
    return AddWidget(Widget);
}

DGuiWidget* DGuiContainer::GetChildFromId(std::string ChildId) {
    for (auto [Id,Child] : Children) {
        if (Id == ChildId) {
            return Child;
        }
    }
    return nullptr;
}

DGuiWidget* DGuiContainer::GetChildFromName(std::string ChildName) {
    for (auto [Id,Child] : Children) {
        if (Child->Name == ChildName) {
            return Child;
        }
    }
    return nullptr;
}

/**
 * @brief Load all gui widgets from file, can be a json formatted or rgl.
 * If file is an rgl saved from rGui app it will be converted in json.
 * @param Filename  ->  file to load.
 * @return true if all widget are loaded.
 * @return false if an error occours or not all widgets are loaded.
 * 
 */
/*
bool DGuiContainer::LoadFromFile(std::string Filename) {
    std::string JsonFilename;
    if (DString::CmpNoCase(DPath::GetExt(Filename),"rgl")) {
        // Raygui rgl format, convert to json
        JsonFilename=DPath::ChangeExt(Filename,"json",false);
        if (DPath::Exists_StdFs(JsonFilename.c_str())) {
            Log(DLOG_WARNING,"found a Json version of same layout, assume to use it");
        }
        else {
            if (!DPath::Exists(Filename)) {
                Log(DLOG_ERROR,"Layout file does not exist: %s",Filename.c_str());
                return false;
            }
            Log(DLOG_DEBUG,"rgl file need to be converted to json");
            JsonFilename=ConvertRglFile(Filename);
        }
    }
    else {
        JsonFilename=Filename;
    }

    if (JsonFilename.empty()) {
        Log(DLOG_ERROR,"Not valid layout file...");
        return false;
    }

    DTools::DPreferences Json(JsonFilename,false);
    if (!Json.IsReady()) {
        Log(DLOG_ERROR,"Error parsing %s: %s",JsonFilename.c_str(), Json.GetLastStatus().c_str());
        return false;
    }

    DTree dt=Json.GetTree();
    return LoadFromTree(std::move(dt));
}
*/
/**
 * @brief 
 * 
 * @param JsonTree 
 * @return true 
 * @return false 
 * 
 * Container json tree:
    {
        "Name": "FormMain",
        "Width": 800,
        "Height": 400,

        "Controls": {
            "Widget01": {
                ...
            },
            "Widget02": {
                ...
            }
            ...
        }
    }
 */
/*
bool DGuiContainer::LoadFromTree(DTree JsonTree) {
    // Name
    Name=JsonTree.ReadString(DJsonWidget::ITEM_NAME,"");
    if (Name.empty()) {
        Log(DLOG_ERROR,"Container must have a name");
    }
    
    // Size
    Bounds.width=JsonTree.ReadInteger(DJsonWidget::ITEM_WIDTH,DEFAULT_WIDTH);
    Bounds.height=JsonTree.ReadInteger(DJsonWidget::ITEM_HEIGHT,DEFAULT_HEIGHT);

    // Widgets
    std::vector<std::string> WidgetsList;
    JsonTree.ReadNames(DJsonWidget::SEC_CONTROLS,WidgetsList);
    bool ret=true;
    for (std::string& WidgetName : WidgetsList) {
        std::string WidgetTreeName=DJsonWidget::SEC_CONTROLS+DOT+WidgetName;
        DTree WidgetTree=JsonTree.GetTree(WidgetTreeName);
        DGuiWidget *Widget=NewWidget(WidgetTree);
        Widget->Name=WidgetName;
        if (!AddWidget(Widget,this)) {
            ret=false;
        }
    }

    return ret;
}
*/
void DGuiContainer::Draw(void) {
    for (auto [Name,Widget] : Children) {
        //Log(DLOG_DEBUG,"Draw %s",Name.c_str());
        Widget->Draws();
    }
}