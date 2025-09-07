#include "DText.h"
#include <dpplib/DString.h>
#include <DGuiWidget.h>

#define DEFAULT_TEXT_PADDING 2
#define DEFAULT_TEXT_SPACING 2
#define DEFAULT_WIDGET_TYPE DLABEL

using namespace DTools;

DText::DText()
{
    WidgetType=DEFAULT_WIDGET_TYPE;
    SetDefault();
}

DText::DText(std::string Newtext)
{
    Text=Newtext;
    WidgetType=DEFAULT_WIDGET_TYPE;
    SetDefault();
}

void DText::SetWidgetType(DWidgetType Type)
{
    WidgetType=Type;
    SetDefault();
}

void DText::SetDefault(void) {
    TextColor=ColorToInt(BLACK);
    Padding=DEFAULT_TEXT_PADDING; 
    FontSize=GuiGetStyle(DEFAULT,TEXT_SIZE);
    Spacing=DEFAULT_TEXT_SPACING;
    Align.Horiz=(DTextAlignH) GuiGetStyle(WidgetType,TEXT_ALIGNMENT);
    Align.Vert=(DTextAlignV) GuiGetStyle(DEFAULT,TEXT_ALIGNMENT_VERTICAL);
    TextFont=GuiGetFont();
}

void DText::InitFromTree(DTree WidgetTree) //, DText *ParentText)
{
    // Text
    Text=WidgetTree.ReadString(DJsonTree::ITEM_TEXT,"");
    Prefix=WidgetTree.ReadString(DJsonTree::ITEM_PREFIX,"");
    Suffix=WidgetTree.ReadString(DJsonTree::ITEM_SUFFIX,"");

    // Text size
    SetFontSize(WidgetTree.ReadInteger(DJsonTree::ITEM_FONT_SIZE,GuiGetStyle(DEFAULT,TEXT_SIZE)));

    // Text align
    std::string StrAlignH=WidgetTree.ReadString(DJsonTree::ITEM_TEXT_ALIGN_H,"");
    std::string StrAlignV=WidgetTree.ReadString(DJsonTree::ITEM_TEXT_ALIGN_V,"");
    SetAlign(StrAlignH,StrAlignV);
    
    // Text color
    std::string StrValue=WidgetTree.ReadString(DJsonTree::ITEM_TEXT_COLOR,"");
    if (!StrValue.empty()) {
        TextColor=DGuiWidget::ColorStringToInt(StrValue);
    }

    // Text spacing
    SetSpacing(WidgetTree.ReadInteger(DJsonTree::ITEM_TEXT_SPACING,DEFAULT_TEXT_SPACING));

    // Text padding
    SetPadding(WidgetTree.ReadInteger(DJsonTree::ITEM_TEXT_PADDING,0));
}

void DText::SetText(std::string NewText)
{
    if (NewText == Text) {
        return;
    }
    Text=NewText;
}

const std::string& DText::GetText(void) const
{
    return Text;
}

void DText::SetPadding(int NewPadding)
{
    if (NewPadding <= 0) {
        Padding=DEFAULT_TEXT_PADDING;
    }
    else {
        Padding=NewPadding;
    }
}

void DText::SetSpacing(int NewSpacing)
{
    if (NewSpacing <= 0) {
        Spacing=DEFAULT_TEXT_SPACING;
    }
    else {
        Spacing=NewSpacing;
    }
}

void DText::SetAlign(std::string StrAlignH,std::string StrAlignV)
{
    SetAlignH(StrAlignH);
    SetAlignV(StrAlignV);
}

void DText::SetAlignH(std::string StrAlignH)
{
    if (DString::StartsWith(StrAlignH,DJsonTree::VALUE_LEFT)) {
        Align.Horiz=DTextAlignH::TEXT_ALIGN_HLEFT;
    }
    else if (DString::StartsWith(StrAlignH,DJsonTree::VALUE_CENTER)) {
        Align.Horiz=DTextAlignH::TEXT_ALIGN_HCENTER;
    }
    else if (DString::StartsWith(StrAlignH,DJsonTree::VALUE_RIGHT)) {
        Align.Horiz=DTextAlignH::TEXT_ALIGN_HRIGHT;
    }
}

void DText::SetAlignV(std::string StrAlignV)
{
    if (DString::StartsWith(StrAlignV,DJsonTree::VALUE_TOP)) {
        Align.Vert=DTextAlignV::TEXT_ALIGN_VTOP;
    }
    else if (DString::StartsWith(StrAlignV,DJsonTree::VALUE_CENTER)) {
        Align.Vert=DTextAlignV::TEXT_ALIGN_VCENTER;
    }
    else if (DString::StartsWith(StrAlignV,DJsonTree::VALUE_BOTTOM)) {
        Align.Vert=DTextAlignV::TEXT_ALIGN_VBOTTOM;
    }
}

void DText::SetAlign(DTextAlign TextAlign)
{
    Align=TextAlign;
}

void DText::SetFontSize(int NewSize)
{
    FontSize=NewSize;
}

void DText::SetPrefix(std::string PrefixText)
{
    Prefix=PrefixText;
}

void DText::SetSuffix(std::string SuffixText)
{
    Suffix=SuffixText;
}

void DText::SetColor(unsigned int Color)
{
    TextColor=Color;
}

int DText::GetFontSize(void) const
{
    return FontSize;
}

int DText::GetPadding(void) const
{
    return Padding;
}

int DText::GetSpacing(void) const
{
    return Spacing;
}

DTextAlign DText::GetAlign(void) const
{
    return Align;
}

int DText::GetColor(void) const
{
    return TextColor;
}

Font DText::GetFont(void) const
{
    return TextFont;
}

std::string DText::GetPrefix(void) const
{
    return Prefix;
}

std::string DText::GetSuffix(void) const
{
    return Suffix;
}

bool DText::IsEmpty(void) const
{
    return Text.empty();
}

void DText::Assign(const DText& Other)
{
    WidgetType=Other.WidgetType;
    Text=Other.Text;
    Align=Other.Align;
    Prefix=Other.Prefix;
    Suffix=Other.Suffix;
    TextColor=Other.TextColor;
    Padding=Other.Padding;
    FontSize=Other.FontSize;
    Spacing=Other.Spacing;
    TextFont=Other.TextFont;
}
