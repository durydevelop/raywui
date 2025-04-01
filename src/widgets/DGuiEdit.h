#ifndef DGuiEdit_H
#define DGuiEdit_H

#include <DGuiWidget.h>
#include <raygui.h>

class DGuiEdit : public DGuiWidget
{
    public:
        DGuiEdit(int LeftPos, int TopPos, int ControlWidth, int ControlHeight, DGuiWidget *ParentWidget);
        DGuiEdit(Rectangle WidgetBounds, DGuiWidget *ParentWidget);
        DGuiEdit(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiEdit(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        ~DGuiEdit();

        void SetMaxTextLenght(size_t Lenght);
        size_t GetMaxTextLenght(void);
        void ClearText(void);

        void SetReadOnly(bool Enabled);
        bool GetReadOnly(void);
        void SetPasswordMode(bool Enabled);
        bool GetPasswordMode(void);
        void SetFocus(void);
        void UpdateSize(void);
        Rectangle GetTextBounds(void);

        void Draw() override;
        
        const std::string& GetText(void) override;
        void SetText(std::string NewText, bool ForceAutoSize) override;
        char* GetTextPtr(void);

        bool IsEmpty(void);

    private:
        void InitDefault(void);
        void FinalizeFromTree(DTools::DTree& WidgetTree);
        //int DrawTextBox(Rectangle bounds, char *mainBuff, char *shadowBuff, int textSize, bool editMode);

        size_t MaxTextLenght;   /// Max lenght of edit text
        bool ReadOnly;          /// Read-only mode
        bool PasswordMode;      /// When true viewBuff is masked with '*'.
        char *ViewBuff;         /// Buffer used for ui, if PasswordMode is false it is used also for realtext.
        char *HideBuff;         /// In password mode used to store text.
        int TextBoxShadowCursorIndex;  /// In password mode used to keep trace of current cursor position.
        bool EditMode;
        //DWidgetEvent ResultEvent;

        //bool DrawTextBox(Rectangle bounds, char *mainBuff, char *shadowBuff, int textSize, bool editMode, DWidgetEvent& EventData);
};

#endif