#ifndef DGuiToggleSlide_H
#define DGuiToggleSlide_H

#include <DGuiWidget.h>

class DGuiToggleSlide : public DGuiWidget
{
    public:
        DGuiToggleSlide(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiToggleSlide(Rectangle WidgetBounds, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiToggleSlide(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiToggleSlide(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);

        void SetSliderPadding(int Padding);

        void Draw() override;

    protected:
        void InitDefault(void);
        void AddItem(const std::string &Text);
        void SetItemIndex(int Index);
        int GetItemIndex(void);

    private:
        void FinalizeFromTree(DTools::DTree& WidgetTree);
        int DrawToggleSlider(Rectangle bounds, int *active);

        std::vector<std::string> Items; /// List of items text
        int ItemIndex;                  /// Current selected item index
        float TextBoundsWidth;          /// With of total concat text of the slider
        int SliderPadding;              /// Padding of the slider
};

#endif