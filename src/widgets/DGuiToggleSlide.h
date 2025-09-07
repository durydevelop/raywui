#ifndef DGuiToggleSlide_H
#define DGuiToggleSlide_H

#include <DGuiWidget.h>
#include <DText.h>

class DGuiToggleSlide : public DGuiWidget
{
    public:
        DGuiToggleSlide(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiToggleSlide(Rectangle WidgetBounds, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiToggleSlide(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiToggleSlide(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);

        void SetSliderPadding(int Padding);
        void SetItemIndex(int Index);
        int GetItemIndex(void);

        void Draw() override;

    protected:
        void InitDefault(void);
        void AddItem(const std::string &ItemText);
        void FinalizeFromTree(DTools::DTree& WidgetTree);

        int ItemIndex;                  /// Current selected item index
        DText Text;                     /// Text params

    private:
        int DrawToggleSlider(Rectangle bounds, int *active);

        std::vector<std::string> TextItems; /// List of items text
        float TextBoundsWidth;              /// With of total concat text of the slider
        int SliderPadding;                  /// Padding of the slider
        DText TempText; // Used for backup and restore current raygui style
};

#endif