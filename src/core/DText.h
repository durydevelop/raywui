#ifndef DText_H
#define DText_H

#include <string>
#include "DGuiCommon.h"
#include <dpplib/DTree.h>

class DText {
    public:
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

        int GetFontSize(void);
        bool IsEmpty(void);

        std::string Text;
        std::string Prefix;
        std::string Suffix;
        unsigned int TextColor;
        int Padding;
        int FontSize;
        int Spacing;
        DTextAlign Align;
        Font TextFont;

    private:
        DWidgetType WidgetType;
};

#endif