#ifndef DGuiLabel_H
#define DGuiLabel_H

#include <raygui.h>
#include <DGuiWidget.h>
#include <DText.h>
#include "dpplib/DFilesystem.h"

class DGuiLabel : public DGuiWidget
{
    public:
        DGuiLabel(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget);
        DGuiLabel(Rectangle WidgetBounds, DGuiWidget *ParentWidget);
        DGuiLabel(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        explicit DGuiLabel(const DTools::fs::path& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        explicit DGuiLabel(DGuiWidget* ParentWidget);

        void SetText(std::string NewText, bool ForceAutoSize = false);
        void SetPrefix(std::string PrefixText, bool ForceUpdate);
        void SetSuffix(std::string SuffixText, bool ForceUpdate);
        void SetFontSize(int NewSize, bool ForceAutoSize);
        void SetTextSpacing(int NewSpacing, bool ForceAutoSize);
        void SetTextPadding(int NewPadding, bool ForceAutoSize);
        void SetTextAlign(std::string AlignHoriz, std::string AlignVert);
        void SetTextAlign(DTextAlign TextAlign);
        void Clear(void);

        const std::string& GetText(void);
        int GetFontSize(void);
        DTextAlign GetTextAlign(void);

        void Update(void) override;
        void AutoWidth(void) override;
        void AutoHeight(void) override;
        void Draw() override;

        DText Text;

    private:
        void InitDefault(void);
        void FinalizeFromTree(DTools::DTree& WidgetTree);

        DText TempText; // Used for backup and restore current raygui style
};

#endif