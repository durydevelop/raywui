#ifndef DGuiLed_H
#define DGuiLed_H

#include <DGuiWidget.h>
#include <DGuiContainer.h>

class DGuiLed : public DGuiContainer
{
    public:
        DGuiLed(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiLed(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);

        void On(void);
        void Off(void);
        void Set(bool SwitchOn);
        void SetCenter(int LeftPos, int TopPos);

        //void SetPos(int LeftPos, int TopPos) override;
        //void SetSize(int Width, int Height) override;
        void SetCaption(std::string CaptionText, int FontSize, DSide Side, int Offset) override;
        //bool UpdateAnchor(void) override;

        void Draw() override;

        uint OnColor;       /// Filling color during ON status
        uint OffColor;      /// Filling color during OFF status
        uint BorderColor;   /// Border color of the circle

    private:
    void InitDefault(void);
        void FinalizeFromTree(DTools::DTree& WidgetTree);
        
        Vector2 LedOffset;
        int Radius;
        uint CurrFilledColor;
        uint CurrBorderColor;
        bool SwitchedOn;
};

#endif