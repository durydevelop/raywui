#ifndef DGuiLabel_H
#define DGuiLabel_H

#include <raygui.h>
#include <DGuiWidget.h>

class DGuiLabel : public DGuiWidget
{
    public:
        DGuiLabel(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget);
        DGuiLabel(Rectangle WidgetBounds, DGuiWidget *ParentWidget);
        DGuiLabel(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiLabel(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);

        void SetPrefix(std::string PrefixText);
        void SetSuffix(std::string SuffixText);

        void ClearText(void);

        void Draw() override;

    private:
        void InitDefault(void);
        std::string TextPrefix;
        std::string TextSuffix;
};

#endif