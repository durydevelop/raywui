#ifndef DGuiToggleSwitch_H
#define DGuiToggleSwitch_H

#include <DGuiToggleSlide.h>

class DGuiToggleSwitch : public DGuiToggleSlide
{
    public:
        using DGuiToggleSlide::DGuiToggleSlide;
        DGuiToggleSwitch();
        DGuiToggleSwitch(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiToggleSwitch(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        bool IsChecked(void);
        void SetChecked(bool Checked);

    private:
        void InitDefault(void);
        bool Checked;

};

#endif