#ifndef DGuiImage_H
#define DGuiImage_H

#include <DGuiWidget.h>

class DGuiImage : public DGuiWidget
{
    public:
        DGuiImage(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiImage(Rectangle WidgetBounds, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiImage(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiImage(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);

        bool LoadFromFile(const std::string& Filename);
        void SetScale(float ScaleFactor);

        void Draw() override;

    private:
    void InitDefault(void);
        void FinalizeFromTree(DTools::DTree& WidgetTree);
        
        Texture2D imageTexture;
        std::string Filename;
        float ScaleValue;
        float RotationValue;
        Color BgColor;
        
};

#endif