#ifndef DGuiContainer_H
#define DGuiContainer_H

#include <DGuiWidget.h>
#include <DGuiLabel.h>
#include <dpplib/DPreferences.h>
/*
class DGuiApp;

struct DAnchor {
    static const size_t ITEMS_COUNT=6;

    size_t Id=0;
    std::string Name;
    int x=0;
    int y=0;
    bool Enabled=false;
};

struct DRefWindow {
    static const size_t ITEMS_COUNT=5;

    int x=0;
    int y=0;
    int with=0;
    int hetight=0;
};
*/
class DGuiContainer : public DGuiWidget {
    public:
        struct DCaption {
            DGuiLabel *Label=nullptr;
            DSide Side=SIDE_LEFT;
            int Offset=5;
        };

        DGuiContainer(int LeftPos, int TopPos, int ContainerWidth, int ContainerHeight, DGuiWidget *ParentWidget);
        DGuiContainer(Rectangle ContainerBounds, DGuiWidget *ParentWidget);
        DGuiContainer(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiContainer(const std::string& LayoutFilename, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        ~DGuiContainer();

        virtual void SetCaption(std::string CaptionText, int FontSize, DSide Side, int Offset);
        void UpdateCaption(void);
        void UnsetCaption(void);

        DGuiWidget* AddWidget(DGuiWidget *Widget);
        DGuiWidget* AddWidget(DTools::DTree *WidgetTree);

        DGuiWidget* FindWidgetById(std::string ChildId);


        void SetOnGuiEvent(OnGuiEventCallback Callback) override;
        DGuiWidget* FindWidgetByName(std::string ChildName) override;
        void Draw(void) override;

        // Using vector to handle Z-Order
        //std::vector<std::pair<std::string, DGuiWidget*>> Children;
        std::map<std::string, DGuiWidget*> Children;

    protected:
       DCaption Caption;

    private:
        void FinalizeFromTree(DTools::DTree& WidgetTree);
        
};

#endif