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

    if (Caption.Label) {
        UpdateCaption();
    }
}
*/
/**
 * @brief Create Caption widget. Caption is an inside label and can only be anchored to on of the contained sides.
 * Differently from label, setting caption, will increase parent container sizes.
 * 
 * @param CaptionText   ->  Text of the caption.
 * @param FontSize      ->  Font size
 * @param Side          ->  Specify the side inside the container to be ancored.
 * @param Offset        ->  Specify the distance from the widget to which is making the caption, e.g.: if Side is SIDE_LEFT, Offset is the distance from right of caption to the widget.
 */
void DGuiContainer::SetCaption(std::string CaptionText, int FontSize, DSide Side, int Offset)
{
    if (CaptionText.empty()) {
        UnsetCaption();
        return;
    }

    if (!Caption.Label) {
        Caption.Label=new DGuiLabel(this);
    }

    Caption.Label->SetText(CaptionText,false);
    Caption.Label->SetTextSize(FontSize,true);
    Caption.Side=Side;
    Caption.Offset=Offset;

    UpdateCaption();
}

void DGuiContainer::UpdateCaption(void)
{
    if (!Caption.Label) {
        return;
    }

    switch (Caption.Side) {
        case SIDE_BOTTOM:
            // Add offset
            Caption.Label->SetHeight(Caption.Label->Bounds.height+Caption.Offset);
            // Align text to bottom
            Caption.Label->Properties.TextAlign.Vert=DTextAlignV::TEXT_ALIGN_VBOTTOM;
            // Set position
            Caption.Label->Bounds.x=(Bounds.width-Caption.Label->Bounds.width)/2; // Center horizzontally
            Caption.Label->Bounds.y=Bounds.height-Caption.Label->Bounds.height;
            // Increase parent height
            Bounds.height+=Caption.Label->Bounds.height;
            break;
        case SIDE_TOP:
            // Add offset
            Caption.Label->SetHeight(Caption.Label->Bounds.height+Caption.Offset);
            // Align text to top
            Caption.Label->Properties.TextAlign.Vert=DTextAlignV::TEXT_ALIGN_VTOP;
            // Set position
            Caption.Label->Bounds.x=(Bounds.width-Caption.Label->Bounds.width)/2; // Center horizzontally
            Caption.Label->Bounds.y=0;
            // Increase parent height
            Bounds.height+=Caption.Label->Bounds.height;
            break;
        case SIDE_LEFT:
            // Add offset
            Caption.Label->SetWidth(Caption.Label->Bounds.width+Caption.Offset);
            // Align text to left
            Caption.Label->Properties.TextAlign.Horiz=DTextAlignH::TEXT_ALIGN_HLEFT;
            // Set position
            Caption.Label->Bounds.x=0;
            Caption.Label->Bounds.y=(Bounds.height-Caption.Label->Bounds.height)/2; // Center vertically
            // Increase parent width
            Bounds.width+=Caption.Label->Bounds.width;
            break;
        case SIDE_RIGHT:
            // Add offset
            Caption.Label->SetWidth(Caption.Label->Bounds.width+Caption.Offset);
            // Align text to right
            Caption.Label->Properties.TextAlign.Horiz=DTextAlignH::TEXT_ALIGN_HRIGHT;
            // Set position
            Caption.Label->Bounds.x=Bounds.width;
            Caption.Label->Bounds.y=(Bounds.height-Caption.Label->Bounds.height)/2; // Center vertically
            // Increase parent width
            Bounds.width+=Caption.Label->Bounds.width;
            break;
        default:
            break;
    }

    //Caption.Label->SetBorderVisible(true);
}

void DGuiContainer::UnsetCaption(void)
{
    if (!Caption.Label) {
        return;
    }

    switch (Caption.Side) {
        case SIDE_BOTTOM:
            // Resize parent height
            Bounds.height-=Caption.Label->Bounds.height;
            break;
        case SIDE_TOP:
            // Resize parent height
            Bounds.height-=Caption.Label->Bounds.height;
            break;
        case SIDE_LEFT:
            // Resize parent width
            Bounds.width-=Caption.Label->Bounds.width;
            break;
        case SIDE_RIGHT:
            // Resize parent width
            Bounds.width-=Caption.Label->Bounds.width;
            break;
        default:
            break;
    }

    delete Caption.Label;
    Caption.Label=nullptr;
}

void DGuiContainer::Draw(void) {
    if (Caption.Label) {
        Caption.Label->Draws();
    }

    for (auto [Name,Widget] : Children) {
        //Log(DLOG_DEBUG,"Draw %s",Name.c_str());
        Widget->Draws();
    }
}