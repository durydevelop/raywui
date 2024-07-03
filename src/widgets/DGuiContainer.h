#ifndef DGuiContainer_H
#define DGuiContainer_H

#include <DGuiWidget.h>
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
        DGuiContainer(int LeftPos, int TopPos, int ContainerWidth, int ContainerHeight, DGuiWidget *ParentWidget);
        DGuiContainer(Rectangle ContainerBounds, DGuiWidget *ParentWidget);
        DGuiContainer(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiContainer(const std::string& LayoutFilename, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        ~DGuiContainer();

        DGuiWidget* AddWidget(DGuiWidget *Widget);
//        DGuiWidget* AddWidget(DWidgetType WidgetType, int LeftPos, int TopPos, int WidgetWidth, int WidgetHeight, std::string Text);
//        DGuiWidget* AddWidget(DWidgetType WidgetType, Rectangle WidgetBounds, std::string Text);
        DGuiWidget* AddWidget(DTools::DTree *WidgetTree);
        void SetOnGuiEvent(OnGuiEventCallback Callback) override;
        //bool LoadFromFile(std::string Filename);
        //bool LoadFromTree(DTools::DTree Json);

        DGuiWidget* GetChildFromId(std::string ChildId);
        DGuiWidget* GetChildFromName(std::string ChildName);

        void Draw(void) override;

    private:
        void FinalizeFromTree(DTools::DTree& WidgetTree);
        //std::map<std::string, DGuiWidget*> Children;
        std::vector<std::pair<std::string, DGuiWidget*>> Children;
};

#endif