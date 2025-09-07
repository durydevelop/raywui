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

using namespace DTools;

const char TAG[14]="DGuiContainer";

DGuiContainer::DGuiContainer(int LeftPos, int TopPos, int ContainerWidth, int ContainerHeight, DGuiWidget *ParentWidget) : DGuiWidget(DCONTAINER,LeftPos,TopPos,ContainerWidth,ContainerHeight,ParentWidget)
{
}

DGuiContainer::DGuiContainer(Rectangle ContainerBounds, DGuiWidget *ParentWidget) : DGuiWidget(DCONTAINER,ContainerBounds,ParentWidget)
{
}

DGuiContainer::DGuiContainer(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(std::ref(WidgetTree),ParentWidget,EventCallback)
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
    Log::debug(TAG,"Adding...");
    for (auto Child : Children) {
        if (!AddWidget(&Child)) {
            /// @todo fatal exit
        }
    }
    Log::debug(TAG,"Added...");
}

void DGuiContainer::SetOnGuiEvent(OnGuiEventCallback Callback) {
    for (auto &[id,widget] : Children) {
        widget->SetOnGuiEvent(Callback);
    }
}

DGuiWidget* DGuiContainer::AddWidget(DGuiWidget *NewWidget)
{
    if (NewWidget) {
        int BoundLimitX=Bounds.x+Bounds.width;
        if ((NewWidget->Bounds.x+NewWidget->Bounds.width) > (BoundLimitX)) {
            // Widget width is out of bounds
            // Resize width
// TODO serve?            NewWidget->SetWidth(BoundLimitX-NewWidget->Bounds.x);
        }
        int BoundLimitY=Bounds.y+Bounds.height;
        if ((NewWidget->Bounds.y+NewWidget->Bounds.height) > (BoundLimitY)) {
            // Widget hetght out of container bound
            // Resize height
// TODO: serve?            NewWidget->SetHeight(BoundLimitY-NewWidget->Bounds.y);
        }
        Children.emplace(NewWidget->Name,NewWidget);
    }
    return NewWidget;
}

DGuiWidget* DGuiContainer::AddWidget(DTools::DTree *WidgetTree)
{
    DGuiWidget *Widget=New(*WidgetTree,this,OnWidgetEvent);
    return AddWidget(Widget);
}

int DGuiContainer::GetAllWidgets(std::vector<DGuiWidget*>& WidgetList)
{
    for (auto& [Id,Child] : Children) {
        if (Child->GetWidgetType() == DWidgetType::DCONTAINER) {
            DGuiContainer *c=(DGuiContainer *) Child;
            c->GetAllWidgets(WidgetList);
        }
        WidgetList.emplace_back(Child);
    }
    return WidgetList.size();
}

DGuiWidget* DGuiContainer::FindWidgetById(std::string ChildId) {
    for (auto& [Id,Child] : Children) {
        if (Child->GetWidgetType() == DWidgetType::DCONTAINER) {
            auto DeepChild=((DGuiContainer *)Child)->FindWidgetById(ChildId);
            if (DeepChild) {
                return DeepChild;
            }
        }

        if (Id == ChildId) {
            return Child;
        }
    }
    return nullptr;
}

DGuiWidget* DGuiContainer::FindWidgetByName(std::string ChildName) {
    for (auto& [Id,Child] : Children) {
        if (Child->GetWidgetType() == DWidgetType::DCONTAINER) {
            auto DeepChild=((DGuiContainer *)Child)->FindWidgetByName(ChildName);
            if (DeepChild) {
                return DeepChild;
            }
        }

        if (Child->Name == ChildName) {
            return Child;
        }
    }
    return nullptr;
}
/*
bool DGuiContainer::UpdateAnchor(void)
{
    DGuiWidget::UpdateAnchor();

    if (LabelInt.Label) {
        UpdateLabelInt();
    }
}
*/
/**
 * @brief Create LabelInt widget. LabelInt is an inside label and can only be anchored to on of the contained sides.
 * Differently from label, setting labelint, will increase parent container sizes.
 * 
 * @param LabelIntText   ->  Text of the labelint.
 * @param FontSize      ->  Font size
 * @param Side          ->  Specify the side inside the container to be ancored.
 * @param Offset        ->  Specify the distance from the widget to which is making the labelint, e.g.: if Side is SIDE_LEFT, Offset is the distance from right of labelint to the widget.
 */
void DGuiContainer::SetLabelInt(std::string LabelIntText, int FontSize, DSide Side, int Offset)
{
    if (LabelIntText.empty()) {
        UnsetLabelInt();
        return;
    }

    if (!LabelInt.Label) {
        LabelInt.Label=new DGuiLabel(this);
    }

    LabelInt.Label->SetText(LabelIntText,false);
    LabelInt.Label->SetFontSize(FontSize,true);
    LabelInt.Side=Side;
    LabelInt.OffsetX=Offset;
    LabelInt.OffsetY=Offset;

    UpdateLabelInt();
}

void DGuiContainer::UpdateLabelInt(void)
{
    if (!LabelInt.Label) {
        return;
    }

    switch (LabelInt.Side) {
        case SIDE_BOTTOM:
            // Add offset
            LabelInt.Label->SetHeight(LabelInt.Label->Bounds.height+LabelInt.OffsetY);
            // Align text to bottom
            LabelInt.Label->SetTextAlign({DTextAlignH::TEXT_ALIGN_HCENTER,DTextAlignV::TEXT_ALIGN_VBOTTOM});
            // Set position
            LabelInt.Label->Bounds.x=(Bounds.width-LabelInt.Label->Bounds.width)/2; // Center horizzontally
            LabelInt.Label->Bounds.y=Bounds.height-LabelInt.Label->Bounds.height;
            // Increase parent height
            Bounds.height+=LabelInt.Label->Bounds.height;
            break;
        case SIDE_TOP:
            // Add offset
            LabelInt.Label->SetHeight(LabelInt.Label->Bounds.height+LabelInt.OffsetY);
            // Align text to top
            LabelInt.Label->SetTextAlign({DTextAlignH::TEXT_ALIGN_HCENTER,DTextAlignV::TEXT_ALIGN_VTOP});
            // Set position
            LabelInt.Label->Bounds.x=(Bounds.width-LabelInt.Label->Bounds.width)/2; // Center horizzontally
            LabelInt.Label->Bounds.y=0;
            // Increase parent height
            Bounds.height+=LabelInt.Label->Bounds.height;
            break;
        case SIDE_LEFT:
            // Add offset
            LabelInt.Label->SetWidth(LabelInt.Label->Bounds.width+LabelInt.OffsetX);
            // Align text to left
            LabelInt.Label->SetTextAlign({DTextAlignH::TEXT_ALIGN_HLEFT,DTextAlignV::TEXT_ALIGN_VCENTER});
            // Set position
            LabelInt.Label->Bounds.x=0;
            LabelInt.Label->Bounds.y=(Bounds.height-LabelInt.Label->Bounds.height)/2; // Center vertically
            // Increase parent width
            Bounds.width+=LabelInt.Label->Bounds.width;
            break;
        case SIDE_RIGHT:
            // Add offset
            LabelInt.Label->SetWidth(LabelInt.Label->Bounds.width+LabelInt.OffsetX);
            // Align text to right
            LabelInt.Label->SetTextAlign({DTextAlignH::TEXT_ALIGN_HRIGHT,DTextAlignV::TEXT_ALIGN_VCENTER});
            // Set position
            LabelInt.Label->Bounds.x=Bounds.width;
            LabelInt.Label->Bounds.y=(Bounds.height-LabelInt.Label->Bounds.height)/2; // Center vertically
            // Increase parent width
            Bounds.width+=LabelInt.Label->Bounds.width;
            break;
        default:
            break;
    }

    //LabelInt.Label->SetBorderVisible(true);
}

void DGuiContainer::SetTextInt(std::string NewText)
{
    LabelInt.Label->SetText(NewText);
}

void DGuiContainer::UnsetLabelInt(void)
{
    if (!LabelInt.Label) {
        return;
    }

    switch (LabelInt.Side) {
        case SIDE_BOTTOM:
            // Resize parent height
            Bounds.height-=LabelInt.Label->Bounds.height;
            break;
        case SIDE_TOP:
            // Resize parent height
            Bounds.height-=LabelInt.Label->Bounds.height;
            break;
        case SIDE_LEFT:
            // Resize parent width
            Bounds.width-=LabelInt.Label->Bounds.width;
            break;
        case SIDE_RIGHT:
            // Resize parent width
            Bounds.width-=LabelInt.Label->Bounds.width;
            break;
        default:
            break;
    }

    delete LabelInt.Label;
    LabelInt.Label=nullptr;
}

void DGuiContainer::SetDebugView(bool Enabled)
{
    DGuiWidget::SetDebugView(Enabled);

    if (LabelInt.Label) {
        LabelInt.Label->SetDebugView(Enabled);
    }
}

void DGuiContainer::Draw(void) {
    if (LabelInt.Label) {
        LabelInt.Label->Draws();
    }

    for (auto [Name,Widget] : Children) {
        //Log(DLOG_DEBUG,"Draw %s",Name.c_str());
        Widget->Draws();
    }
}