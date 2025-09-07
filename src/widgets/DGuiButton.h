#ifndef DGuiButton_H
#define DGuiButton_H

#include <DGuiWidget.h>
#include <DText.h>

class DGuiButton : public DGuiWidget
{
    public:
        DGuiButton(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiButton(Rectangle WidgetBounds, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiButton(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiButton(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);

        void Update(void) override;
        void AutoWidth(void) override;
        void AutoHeight(void) override;
        void Draw() override;

        DText Text;

    private:
        void InitDefault(void);
        void FinalizeFromTree(DTools::DTree& WidgetTree);

        DText TempText;
};

#endif