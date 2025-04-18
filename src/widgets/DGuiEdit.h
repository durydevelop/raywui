#ifndef DGuiEdit_H
#define DGuiEdit_H

#include <DGuiWidget.h>
#include <raygui.h>
#include <DText.h>

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
        void SetFocus(bool Enabled);
        void UpdateSize(void);
        Rectangle GetTextBounds(void);

        void Draw() override;
        
        const std::string& GetText(void);
        void SetText(std::string NewText, bool ForceAutoSize);
        char* GetTextPtr(void);
        bool IsEmpty(void);
        int GetTextWidth(std::string TextStr);

    private:
        void InitDefault(void);
        void FinalizeFromTree(DTools::DTree& WidgetTree);
        int DrawTextBox(Rectangle bounds, char *mainBuff, char *shadowBuff, int textSize);

        DText Text;
        DText TempText;
        size_t MaxTextLenght;   /// Max lenght of edit text
        bool ReadOnly;          /// Read-only mode
        bool PasswordMode;      /// When true viewBuff is masked with '*'.
        char *ViewBuff;         /// Buffer used for ui, if PasswordMode is false it is used also for realtext.
        char *HideBuff;         /// In password mode used to store text.
        int ShadowCursorIndex;  /// In password mode used to keep trace of current cursor position.
        int CursorIndex;
        int AutoCursorCooldownCounter;
        int AutoCursorDelayCounter;
        bool Pressed;
        //bool EditMode;
};

#endif