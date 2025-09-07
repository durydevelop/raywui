#ifndef DText_H
#define DText_H

#include <string>
#include "DGuiCommon.h"
#include <dpplib/DTree.h>

class DText {
    public:
        friend class DGuiWidget;
        DText();
        DText(std::string NewText);
        void InitFromTree(DTools::DTree TextTree); //, DText *ParentText);
        void SetWidgetType(DWidgetType Type);
        void SetDefault(void);
        void SetPadding(int NewPadding);
        void SetFontSize(int NewSize);
        void SetSpacing(int NewSpacing);
        void SetAlignH(std::string StrAlignH);
        void SetAlignV(std::string StrAlignV);
        void SetAlign(std::string StrAlignH,std::string StrAlignV);
        void SetAlign(DTextAlign TextAlign);
        void SetText(std::string NewText);
        void SetPrefix(std::string PrefixText);
        void SetSuffix(std::string SuffixText);
        void SetColor(unsigned int Color);

        const std::string& GetText(void) const;
        int GetFontSize(void) const;
        int GetPadding(void) const;
        int GetSpacing(void) const;
        DTextAlign GetAlign(void) const;
        int GetColor(void) const;
        Font GetFont(void) const;
        std::string GetPrefix(void) const;
        std::string GetSuffix(void) const;

        bool IsEmpty(void) const;

        void Assign(const DText& Other);

    private:
        DWidgetType WidgetType;
        std::string Text;
        DTextAlign Align;
        std::string Prefix;
        std::string Suffix;
        unsigned int TextColor;
        int Padding;
        int FontSize;
        int Spacing;
        Font TextFont;
};

#endif