#include "DGuiImage.h"
#include "raywui_log.h"

const char TAG[10] = "DGuiImage";

DGuiImage::DGuiImage(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(DBUTTON,LeftPos,TopPos,ControlWidth,ControlHeight,ParentWidget,EventCallback)
{
    InitDefault();
}
        
DGuiImage::DGuiImage(Rectangle WidgetBounds, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(DBUTTON,WidgetBounds,ParentWidget,EventCallback)
{
    InitDefault();
}

DGuiImage::DGuiImage(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(WidgetTree,ParentWidget,EventCallback)
{
    InitDefault();
    FinalizeFromTree(WidgetTree);
    Ready=(Type == DIMAGE);
}

DGuiImage::DGuiImage(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiImage(std::move(ExtractDTree(LayoutFilename)),ParentWidget,EventCallback)
{
}

bool DGuiImage::LoadFromFile(const std::string& Filename)
{
    // Load image
    imageTexture=LoadTexture(Filename.c_str());
    if (!IsTextureReady(imageTexture)) {
        LastError=Filename+" is not a valid image";
        Log::error(TAG,LastError.c_str());
        return false;
    }
    
    // Update scale
    Bounds.width=(float)imageTexture.width*ScaleValue;
    Bounds.height=(float)imageTexture.height*ScaleValue;

    return true;
}

void DGuiImage::SetScale(float ScaleFactor)
{
    ScaleValue=ScaleFactor;
    if (IsTextureReady(imageTexture)) {
        Bounds.width=(float)imageTexture.width*ScaleValue;
        Bounds.height=(float)imageTexture.height*ScaleValue;
    }

    UpdateLabel();
}

void DGuiImage::InitDefault(void)
{
    /// @todo Label rotation?
    RotationValue=0;
    ScaleValue=1.0;
    Properties.BackGroundColor=ColorToInt(WHITE);
    //DEFAULT_SIDE_SIZE=50;
    //DEFAULT_WIDTH=50;
    //DEFAULT_HEIGHT=20;
}

void DGuiImage::FinalizeFromTree(DTools::DTree& WidgetTree)
{
    // ** Read class specific properties **
    RotationValue=WidgetTree.ReadFloat(DJsonTree::ITEM_ROTATION,0);
    ScaleValue=WidgetTree.ReadFloat(DJsonTree::ITEM_SCALE,1.0);
    Filename=WidgetTree.ReadString(DJsonTree::ITEM_FILENAME,"");
    if (!Filename.empty()) {
        LoadFromFile(Filename);
    }
    UpdateLabel();
}

/**
 * @brief Draw the image.
 * Override method
 * @return the #Result value of this widget.
 */
void DGuiImage::Draw()
{
    DrawTextureEx(imageTexture,Vector2(Bounds.x,Bounds.y),RotationValue,ScaleValue,GetColor(Properties.BackGroundColor));
}
