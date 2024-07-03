#ifndef DGuiNewWidget_H
#define DGuiNewWidget_H

#include <DGuiWidget.h>

class DGuiNewWidget : public DGuiWidget
{
    public:
        DGuiNewWidget(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiNewWidget(Rectangle WidgetBounds, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiNewWidget(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiNewWidget(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);

        void Draw() override;

    private:
        void InitDefault(void);
        void FinalizeFromTree(DTools::DTree& WidgetTree);
};

#endif