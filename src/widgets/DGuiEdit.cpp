#include "DGuiEdit.h"
#include "raywui_log.h"
#include <string.h>
#include <DGuiLabel.h>

#define DEFAULT_MAX_TEXT_LENGHT 128

const char TAG[9]="DGuiEdit";

DGuiEdit::DGuiEdit(int LeftPos, int TopPos, int WidgetWidth, int WidgetHeight, DGuiWidget *ParentWidget) : DGuiWidget(DEDIT,LeftPos,TopPos,WidgetWidth,WidgetHeight,ParentWidget)
{
    InitDefault();
}

DGuiEdit::DGuiEdit(Rectangle WidgetBounds, DGuiWidget *ParentWidget) : DGuiWidget(DEDIT,WidgetBounds,ParentWidget)
{
    InitDefault();
}

DGuiEdit::DGuiEdit(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(std::ref(WidgetTree),ParentWidget,EventCallback)
{
    InitDefault();
    FinalizeFromTree(WidgetTree);
    Ready=(Type == DEDIT);
}

DGuiEdit::DGuiEdit(const std::string& LayoutFilename, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiEdit(std::move(ExtractDTree(LayoutFilename)),ParentWidget,EventCallback)
{
}

DGuiEdit::~DGuiEdit()
{
    Log::debug(TAG,"~DGuiEdit() %s",Name.c_str());
    delete ViewBuff;
    if (HideBuff) {
        delete HideBuff;
    }
}

void DGuiEdit::InitDefault(void)
{
    Focus.Enable=true;
    ReadOnly=false;
    PasswordMode=false;
    MaxTextLenght=DEFAULT_MAX_TEXT_LENGHT;
    ViewBuff=new char[MaxTextLenght+1];
    memset(ViewBuff,'\0',MaxTextLenght+1);
    HideBuff=nullptr;
    CursorIndex=0;
    ShadowCursorIndex=0;
    AutoCursorCooldownCounter=0;
    AutoCursorDelayCounter=0;
    DEFAULT_SIDE_SIZE=50;
    DEFAULT_WIDTH=50;
    DEFAULT_HEIGHT=20;
}

void DGuiEdit::FinalizeFromTree(DTools::DTree& WidgetTree)
{
    // Init text
    Text.InitFromTree(WidgetTree);
/*
    if (LabelExt.Label) {
        if (LabelExt.Label->GetFontSize() < 0) {
            // Label font size from parent
            LabelExt.Label->SetFontSize(Text.GetFontSize(),false);
        }
    }
*/
    // ** Read class specific properties **
    // ReadOnly
    bool ro=WidgetTree.ReadBool(DJsonTree::ITEM_READ_ONLY,false);
    SetReadOnly(ro);
    // PasswordMode
    bool pm=WidgetTree.ReadBool(DJsonTree::ITEM_PASSWORD_MODE,false);
    SetPasswordMode(pm);
    // MaxTextLenght
    SetMaxTextLenght(WidgetTree.ReadInteger(DJsonTree::ITEM_MAX_TEXT_LENGHT,0));

    UpdateSize();
}

/**
 * @brief Set max text lenght size.
 * String lenght greater of, will be truncated.
 * 
 * @param Lenght    ->  New max text lenght size. If set to 0, default value is assigned.
 */
void DGuiEdit::SetMaxTextLenght(size_t NewLenght)
{
    if ((NewLenght == 0)) {
        NewLenght=DEFAULT_MAX_TEXT_LENGHT;
    }

    if (NewLenght == MaxTextLenght) {
        return;
    }

    MaxTextLenght=NewLenght;

    if (PasswordMode) {
        // In password mode real text buffer is HideBuff
        // Store
        Text.Text.assign(HideBuff);
        // reallocate buffer
        if (HideBuff) {
            delete HideBuff;
        }
        HideBuff=new char[MaxTextLenght+1];
        memset(HideBuff,'\0',MaxTextLenght+1);
        // re-assign
        strcpy(HideBuff,Text.Text.c_str());

        // ViewBuff is used for masked view
        delete ViewBuff;
        ViewBuff=new char[MaxTextLenght+1];
        memset(ViewBuff,'\0',MaxTextLenght+1);
    }
    else {
        // NO password mode (only ViewBuff is used)
        // Store
        Text.Text.assign(ViewBuff);
        // reallocate buffer
        delete ViewBuff;
        ViewBuff=new char[MaxTextLenght+1];
        memset(ViewBuff,'\0',MaxTextLenght+1);
        // re-assign
        strcpy(ViewBuff,Text.Text.c_str());
    }
}

size_t DGuiEdit::GetMaxTextLenght(void)
{
    return MaxTextLenght;
}

void DGuiEdit::SetReadOnly(bool Enabled)
{
    ReadOnly=Enabled;
}

bool DGuiEdit::GetReadOnly(void)
{
    return ReadOnly;
}

void DGuiEdit::SetPasswordMode(bool Enabled)
{
    if (PasswordMode == Enabled) {
        return;
    }

    if (Enabled) {
        // From disable to enable
        if (HideBuff) {
            // to be sure...
            delete HideBuff;
        }
        HideBuff=new char[MaxTextLenght+1];
        // Copy real text in HideBuff
        strcpy(HideBuff,ViewBuff);
        // set cursor index
//        TextBoxShadowCursorIndex=GetGuiTextBoxCursorIndex();
        // Set ViewBuff with '*'
        memset(ViewBuff,'*',strlen(HideBuff));
        // Ri-calculate index position in ViewBuff
        int prevCodepointSize = 0;
        int index=0;
        // Move backward text from cursor position
        for (int i = (ShadowCursorIndex - prevCodepointSize); i < strlen(HideBuff); i++) {
            GetCodepointPrevious(HideBuff + ShadowCursorIndex, &prevCodepointSize);
            index++;
        }
        SetGuiTextBoxCursorIndex(index);
    }
    else {
        // From enable to disable
        if (HideBuff) { //...to be sure
            // Copy real text in ViewBuff
            strcpy(ViewBuff,HideBuff);
            // delete highBuff
            delete HideBuff;
        }
        HideBuff=nullptr;
//        SetGuiTextBoxCursorIndex(TextBoxShadowCursorIndex);
        CursorIndex=ShadowCursorIndex;
        ShadowCursorIndex=0;
    }

    PasswordMode=Enabled;
}

bool DGuiEdit::GetPasswordMode(void)
{
    return PasswordMode;
}

void DGuiEdit::SetFocus(bool Enabled)
{
    //EditMode=Enabled;
}

void DGuiEdit::ClearText(void)
{
    //strcpy(ViewBuff,"");
    memset(ViewBuff,'\0',MaxTextLenght+1);
    if (HideBuff) {
        memset(HideBuff,'\0',MaxTextLenght+1);
        ShadowCursorIndex=0;
    }
    Text.Text.clear();
}

const std::string& DGuiEdit::GetText(void)
{
    if (HideBuff) {
        // Password mode
        Text.Text.assign(HideBuff);    
    }
    else {
        // Normal mode
        Text.Text.assign(ViewBuff);
    }
    
    return Text.Text;
}

/**
 * @brief Get the direct pointer to text buffer.
 * 
 * @return char* 
 */
char* DGuiEdit::GetTextPtr(void) {
    if (HideBuff) {
        // Password mode
        return HideBuff;
    }
    else {
        // Normal mode
        return ViewBuff;
    }
}

void DGuiEdit::SetText(std::string NewText, bool ForceAutoSize)
{
    if (HideBuff) {
        // Password mode
        if (strcmp(HideBuff,NewText.c_str()) == 0) {
            // Same
            return;
        }
        strcpy(HideBuff,NewText.c_str());
        Text.Text.assign(HideBuff);
    }
    else {
        // NormalMode
        if (strcmp(ViewBuff,NewText.c_str()) == 0) {
            // Same
            return;
        }
        strcpy(ViewBuff,NewText.c_str());
        Text.Text.assign(ViewBuff);
    }

    if (ForceAutoSize) {
        UpdateSize();
    }
}

void DGuiEdit::UpdateSize(void)
{
    // Expand due to the padding and border
    if (Text.Text.empty()) {
        return;
    }
    int TextOffset=Properties.BorderWidth+Text.Padding;
    SetWidth(GetTextBounds().width+(TextOffset*2));
    SetHeight(Text.FontSize+(TextOffset*2));
}

Rectangle DGuiEdit::GetTextBounds(void)
{
    // Measure text
    /// @todo UpdateTextWith() when text changes
    int TextWidth=GetTextWidth(Text.Text);
    
    // Calculate text bounds
    int TextOffset=Properties.BorderWidth+Text.Padding;
    Rectangle AbsBounds=GetAbsBounds();
    Rectangle TextBounds;
    TextBounds.x=AbsBounds.x+TextOffset;
    TextBounds.y=AbsBounds.y+TextOffset;
    TextBounds.width=TextWidth;
    TextBounds.height=Text.FontSize;

    return TextBounds;
}

bool DGuiEdit::IsEmpty(void) {
    if (HideBuff) {
        // Password mode
        return (strlen(HideBuff) == 0);
    }
    else {
        // Normal mode
        return (strlen(ViewBuff) == 0);
    }
}
int DGuiEdit::GetTextWidth(std::string TextStr)
{
    return DGuiWidget::GetTextWidth(TextStr,Text.TextFont,Text.FontSize,Text.Spacing);
}

/**
 * @brief Draw the Edit TextBox.
 */
void DGuiEdit::Draw()
{
    // Store current global raygui styles
    BackupCurrentTextStyle(TempText);
    // Set global raygui style from this->Properties
    UpdateCurrentTextStyle(Text);

    bool CurrReadOnly=GuiGetStyle(Type,TEXT_READONLY);
    GuiSetStyle(Type,TEXT_READONLY,ReadOnly);

    int eventData;
    int ret=DrawTextBox(GetAbsBounds(),ViewBuff,HideBuff,MaxTextLenght);
    if (ret) {
        if (Focus.Enabled) {
            /// @todo switch case
            if (ret == EDIT_END) {
                SendEvent(DWidgetEvent({EDIT_END,0}));
                
            }
            else if (ret == KEY_ENTER_PRESSED) {
                SendEvent(DWidgetEvent({KEY_PRESSED,KEY_ENTER}));
            }
            else if (ret == KEY_TAB_PRESSED) {
                SendEvent(DWidgetEvent({KEY_PRESSED,KEY_TAB}));
            }
        }
        //EditMode=!EditMode;
    }

    GuiSetStyle(Type,TEXT_READONLY,CurrReadOnly);

    // Restore previous saved global raygui styles
    RestoreCurrentTextStyle(TempText);
}

int DGuiEdit::DrawTextBox(Rectangle bounds, char *mainBuff, char *shadowBuff, int textSize)
{
    #if !defined(RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN)
        #define RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN  40        // Frames to wait for autocursor movement
    #endif
    #if !defined(RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY)
        #define RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY      1        // Frames delay for autocursor movement
    #endif

    int result = 0;
    //GuiState state = guiState;

    bool multiline = false;     // TODO: Consider multiline text input
    int wrapMode = GuiGetStyle(DEFAULT, TEXT_WRAP_MODE);

    Rectangle textBounds = GetTextBounds();
    int textWidth = GetTextWidth(mainBuff) - GetTextWidth(mainBuff + CursorIndex);
    int textIndexOffset = 0;    // Text index offset to start drawing in the box

    // Cursor rectangle
    // NOTE: Position X value should be updated
    Rectangle cursor = {
        textBounds.x + textWidth + GuiGetStyle(DEFAULT, TEXT_SPACING),
        textBounds.y + textBounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE),
        2,
        (float)GuiGetStyle(DEFAULT, TEXT_SIZE)*2
    };

    if (cursor.height >= bounds.height) cursor.height = bounds.height - GuiGetStyle(TEXTBOX, BORDER_WIDTH)*2;
    if (cursor.y < (bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH))) cursor.y = bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH);

    // Mouse cursor rectangle
    // NOTE: Initialized outside of screen
    Rectangle mouseCursor = cursor;
    mouseCursor.x = -1;
    mouseCursor.width = 1;

    // Auto-cursor movement logic
    // NOTE: Cursor moves automatically when key down after some time
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_BACKSPACE) || IsKeyDown(KEY_DELETE)) AutoCursorCooldownCounter++;
    else
    {
        AutoCursorCooldownCounter = 0;      // GLOBAL: Cursor cooldown counter
        AutoCursorDelayCounter = 0;         // GLOBAL: Cursor delay counter
    }

    // Blink-cursor frame counter
    //if (!autoCursorMode) blinkCursorFrameCounter++;
    //else blinkCursorFrameCounter = 0;

    // Update control
    //--------------------------------------------------------------------
    // WARNING: Text editing is only supported under certain conditions:
    if ((Properties.Enabled) && // Control enabled
        !ReadOnly &&            // TextBox not on read-only mode
//        !guiLocked &&                               // Gui not locked
//        !guiControlExclusiveMode &&                       // No gui slider on dragging
        (wrapMode == TEXT_WRAP_NONE))               // No wrap mode
    {
        Vector2 mousePosition = GetMousePosition();

        if (Modify.Enabled)
        {
//            state = STATE_PRESSED;

            // If text does not fit in the textbox and current cursor position is out of bounds,
            // we add an index offset to text for drawing only what requires depending on cursor
/*
            while (textWidth >= textBounds.width)
            {
                int nextCodepointSize = 0;
                GetCodepointNext(mainBuff + textIndexOffset, &nextCodepointSize);

                textIndexOffset += nextCodepointSize;

                textWidth = GetTextWidth(mainBuff + textIndexOffset) - GetTextWidth(mainBuff + CursorIndex);
            }
*/
            // Get current text length
            int textLength = (int)strlen(mainBuff);
            int shadowLength = shadowBuff ? (int)strlen(shadowBuff) : 0; // Does not care in normal mode

            int codepoint = GetCharPressed();       // Get Unicode codepoint
            if (multiline && IsKeyPressed(KEY_ENTER)) codepoint = (int)'\n';

            if (CursorIndex > textLength)
            {
                CursorIndex = textLength;
                ShadowCursorIndex = shadowBuff ? shadowLength : 0;
            }

            // Encode codepobbint as UTF-8
            int codepointSize = 0;
            const char *charEncoded = CodepointToUTF8(codepoint, &codepointSize);

            // Add codepoint to text, at current cursor position
            // NOTE: Make sure we do not overflow buffer size
            if (((multiline && (codepoint == (int)'\n')) || (codepoint >= 32)) && ((textLength + codepointSize) < textSize))
            {
                if (!shadowBuff) // Normal mode
                { 
                    // Move forward data from cursor position
                    for (int i = (textLength + codepointSize); i > CursorIndex; i--) mainBuff[i] = mainBuff[i - codepointSize];

                    // Add new codepoint in current cursor position
                    for (int i = 0; i < codepointSize; i++) mainBuff[CursorIndex + i] = charEncoded[i];

                    CursorIndex += codepointSize;
                    textLength += codepointSize;
                    
                    // Make sure text last character is EOL
                    mainBuff[textLength] = '\0';
                }
                else // Password mode
                {
                    // ** shadowBuff **
                    // Move forward data from cursor position
                    for (int i = (shadowLength + codepointSize); i > ShadowCursorIndex; i--) shadowBuff[i] = shadowBuff[i - codepointSize];

                    // Add new codepoint in current cursor position
                    for (int i = 0; i < codepointSize; i++) shadowBuff[ShadowCursorIndex + i] = charEncoded[i];

                    ShadowCursorIndex += codepointSize;
                    shadowLength += codepointSize;
                    
                    // Make sure text last character is EOL
                    shadowBuff[shadowLength] = '\0';

                    // ** mainBuff **
                    // Only '*' for view, not need to move data (are all '*')
                    mainBuff[CursorIndex] = '*';
                    CursorIndex++;
                    textLength++;
                    // Make sure text last character is EOL
                    mainBuff[textLength] = '\0';
                }
            }

            // Move cursor to start
            if ((textLength > 0) && IsKeyPressed(KEY_HOME))
            {
                CursorIndex = 0;
                ShadowCursorIndex = 0;
            }

            // Move cursor to end
            if ((textLength > CursorIndex) && IsKeyPressed(KEY_END))
            {
                CursorIndex = textLength;
                ShadowCursorIndex = shadowBuff ? shadowLength : 0;
            }

            // Delete codepoint from text, after current cursor position
            if ((textLength > CursorIndex) && (IsKeyPressed(KEY_DELETE) || (IsKeyDown(KEY_DELETE) && (AutoCursorCooldownCounter >= RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN))))
            {
                AutoCursorDelayCounter++;

                if (IsKeyPressed(KEY_DELETE) || (AutoCursorDelayCounter%RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY) == 0)      // Delay every movement some frames
                {
                    if (!shadowBuff) // Normal mode
                    {
                        int nextCodepointSize = 0;
                        GetCodepointNext(mainBuff + CursorIndex, &nextCodepointSize);

                        // Move backward text from cursor position
                        for (int i = CursorIndex; i < textLength; i++) mainBuff[i] = mainBuff[i + nextCodepointSize];

                        textLength -= codepointSize;

                        // Make sure text last character is EOL
                        mainBuff[textLength] = '\0';
                    }
                    else // Password mode
                    {
                        // ** shadowBuff **
                        int nextCodepointSize = 0;
                        GetCodepointNext(shadowBuff + ShadowCursorIndex, &nextCodepointSize);

                        // Move backward text from cursor position
                        for (int i = ShadowCursorIndex; i < shadowLength; i++) shadowBuff[i] = shadowBuff[i + nextCodepointSize];

                        shadowLength -= codepointSize;

                        // Make sure text last character is EOL
                        shadowBuff[shadowLength] = '\0';

                        // ** mainBuff **
                        // only cut 1 byte
                        CursorIndex--;
                        textLength--;
                        // Make sure text last character is EOL
                        mainBuff[textLength] = '\0';
                    }
                }
            }

            // Delete codepoint from text, before current cursor position
            if ((textLength > 0) && (IsKeyPressed(KEY_BACKSPACE) || (IsKeyDown(KEY_BACKSPACE) && (AutoCursorCooldownCounter >= RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN))))
            {
                AutoCursorDelayCounter++;

                if (IsKeyPressed(KEY_BACKSPACE) || (AutoCursorDelayCounter%RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY) == 0)      // Delay every movement some frames
                {
                    if (!shadowBuff) // Normal mode
                    {
                        int prevCodepointSize = 0;
                        GetCodepointPrevious(mainBuff + CursorIndex, &prevCodepointSize);

                        // Move backward text from cursor position
                        for (int i = (CursorIndex - prevCodepointSize); i < textLength; i++) mainBuff[i] = mainBuff[i + prevCodepointSize];

                        // Prevent cursor index from decrementing past 0
                        if (CursorIndex > 0)
                        {
                            CursorIndex -= codepointSize;
                            textLength -= codepointSize;
                        }

                        // Make sure text last character is EOL
                        mainBuff[textLength] = '\0';
                    }
                    else // Password Mode
                    {
                        // ** shadowBuff **
                        int prevCodepointSize = 0;
                        GetCodepointPrevious(shadowBuff + ShadowCursorIndex, &prevCodepointSize);

                        // Move backward text from cursor position
                        for (int i = (ShadowCursorIndex - prevCodepointSize); i < shadowLength; i++) shadowBuff[i] = shadowBuff[i + prevCodepointSize];

                        // Prevent cursor index from decrementing past 0
                        if (ShadowCursorIndex > 0)
                        {
                            ShadowCursorIndex -= codepointSize;
                            shadowLength -= codepointSize;
                        }

                        // Make sure text last character is EOL
                        shadowBuff[textLength] = '\0';

                        // ** mainBuff **
                        // only cut 1 byte
                        CursorIndex--;
                        textLength--;
                        // Make sure text last character is EOL
                        mainBuff[textLength] = '\0';
                    }
                }
            }

            // Move cursor position with keys
            if (IsKeyPressed(KEY_LEFT) || (IsKeyDown(KEY_LEFT) && (AutoCursorCooldownCounter > RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN)))
            {
                AutoCursorDelayCounter++;

                if (IsKeyPressed(KEY_LEFT) || (AutoCursorDelayCounter%RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY) == 0)      // Delay every movement some frames
                {
                    if (!shadowBuff) // Normal mode
                    {
                        int prevCodepointSize = 0;
                        GetCodepointPrevious(mainBuff + CursorIndex, &prevCodepointSize);

                        if (CursorIndex >= prevCodepointSize) CursorIndex -= prevCodepointSize;
                    }
                    else // Password mode
                    {
                        // ** shadowBuff **
                        int prevCodepointSize = 0;
                        GetCodepointPrevious(shadowBuff + ShadowCursorIndex, &prevCodepointSize);

                        if (ShadowCursorIndex >= prevCodepointSize) ShadowCursorIndex -= prevCodepointSize;

                        // ** mainBuff **
                        if (CursorIndex >= 0) CursorIndex --;
                    }
                }
            }
            else if (IsKeyPressed(KEY_RIGHT) || (IsKeyDown(KEY_RIGHT) && (AutoCursorCooldownCounter > RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN)))
            {
                AutoCursorDelayCounter++;

                if (IsKeyPressed(KEY_RIGHT) || (AutoCursorDelayCounter%RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY) == 0)      // Delay every movement some frames
                {
                    if (!shadowBuff) // Normal mode
                    {
                        int nextCodepointSize = 0;
                        GetCodepointNext(mainBuff + CursorIndex, &nextCodepointSize);

                        if ((CursorIndex + nextCodepointSize) <= textLength) CursorIndex += nextCodepointSize;
                    }
                    else // Password mode
                    {
                        // ** ShadowBuff **
                        int nextCodepointSize = 0;
                        GetCodepointNext(shadowBuff + ShadowCursorIndex, &nextCodepointSize);

                        if ((ShadowCursorIndex + nextCodepointSize) <= shadowLength) ShadowCursorIndex += nextCodepointSize;

                        // ** mainBuff **
                        if ((CursorIndex + 1) <= textLength) CursorIndex ++;
                    }
                }
            }

            if (CheckCollisionPointRec(mousePosition, bounds)) {
                // Mouse hover widget
                float scaleFactor = (float)Text.FontSize/(float)Text.TextFont.baseSize;
                int codepointIndex = 0;
                float glyphWidth = 0.0f;
                float widthToMouseX = 0;
                int mouseCursorIndex = 0;

                for (int i = textIndexOffset; i < textLength; i++)
                {
                    codepoint = GetCodepointNext(&mainBuff[i], &codepointSize);
                    codepointIndex = GetGlyphIndex(Text.TextFont, codepoint);

                    if (Text.TextFont.glyphs[codepointIndex].advanceX == 0) glyphWidth = ((float)Text.TextFont.recs[codepointIndex].width*scaleFactor);
                    else glyphWidth = ((float)Text.TextFont.glyphs[codepointIndex].advanceX*scaleFactor);

                    if (mousePosition.x <= (textBounds.x + (widthToMouseX + glyphWidth/2)))
                    {
                        mouseCursor.x = textBounds.x + widthToMouseX;
                        mouseCursorIndex = i;
                        break;
                    }

                    widthToMouseX += (glyphWidth + (float)GuiGetStyle(DEFAULT, TEXT_SPACING));
                }

                // Check if mouse cursor is at the last position
                int textEndWidth = GetTextWidth(mainBuff + textIndexOffset);
                if (GetMousePosition().x >= (textBounds.x + textEndWidth - glyphWidth/2))
                {
                    mouseCursor.x = textBounds.x + textEndWidth;
                    mouseCursorIndex = (int)strlen(mainBuff);
                }

                // Place cursor at required index on mouse click
                if ((mouseCursor.x >= 0) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    cursor.x = mouseCursor.x;
                    CursorIndex = mouseCursorIndex;
                    if (shadowBuff) // Password mode
                    {
                        // Calculate index position in shadowBuff
                        int nextCodepointSize = 0;
                        int shadowIndex=0;
                        for (int i = 0; i < mouseCursorIndex; i++){
                            GetCodepointNext(mainBuff + CursorIndex, &nextCodepointSize);
                            shadowIndex+=nextCodepointSize;
                        }
                        ShadowCursorIndex = shadowIndex;
                    }
                }
            }
//            else mouseCursor.x = -1;

            // Recalculate cursor position.y depending on textBoxCursorIndex
            cursor.x = bounds.x + GuiGetStyle(TEXTBOX, TEXT_PADDING) + GetTextWidth(mainBuff + textIndexOffset) - GetTextWidth(mainBuff + CursorIndex) + GuiGetStyle(DEFAULT, TEXT_SPACING);
            //if (multiline) cursor.y = GetTextLines()

            // Finish text editing on ENTER or mouse click outside bounds
            if (IsKeyPressed(KEY_ENTER) && !multiline)
            {
                CursorIndex = 0;     // GLOBAL: Reset the shared cursor index
                ShadowCursorIndex = 0;
                result = KEY_ENTER_PRESSED;
            }
            if (IsKeyPressed(KEY_TAB) && !multiline)
            {
                CursorIndex = 0;     // GLOBAL: Reset the shared cursor index
                ShadowCursorIndex = 0;
                result = KEY_TAB_PRESSED;
            }
            else if (!CheckCollisionPointRec(mousePosition, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                CursorIndex = 0;     // GLOBAL: Reset the shared cursor index
                ShadowCursorIndex = 0;
                result = TEXT_EDIT_END;
            }
        }
        else
        {
            if (CheckCollisionPointRec(mousePosition, bounds))
            {
//                state = STATE_FOCUSED;

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    CursorIndex = (int)strlen(mainBuff);   // GLOBAL: Place cursor index to the end of current text
                    ShadowCursorIndex = shadowBuff ? (int)strlen(shadowBuff) : 0;
                    result = MOUSE_LEFT_BUTTON_PRESSED;
                    Modify.Enabled=true;
                }
                else {
                    Modify.Enabled=false;
                }
            }
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (Modify.Enabled)
    {
        RayGuiDrawRectangle(bounds, Properties.BorderWidth, GetColor(Properties.BorderColor), GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_PRESSED)));
    }
    else if (!Properties.Enabled)
    {
        RayGuiDrawRectangle(bounds, Properties.BorderWidth, GetColor(Properties.BorderColor), GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)));
    }
    else RayGuiDrawRectangle(bounds, Properties.BorderWidth, GetColor(Properties.BorderColor), BLANK);

    // Draw text considering index offset if required
    // NOTE: Text index offset depends on cursor position
    RayGuiDrawText(mainBuff + textIndexOffset, textBounds, Text.Align, GetColor(Text.TextColor));

    // Draw cursor
    if (Modify.Enabled && !ReadOnly)
    {
        //if (autoCursorMode || ((blinkCursorFrameCounter/40)%2 == 0))
        RayGuiDrawRectangle(cursor, 0, BLANK, GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED)));

        // Draw mouse position cursor (if required)
        if (mouseCursor.x >= 0) RayGuiDrawRectangle(mouseCursor, 0, BLANK, GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED)));
    }
    else if (Focus.Enabled) {
//        GuiTooltip(bounds);
    }
    //--------------------------------------------------------------------

    return result;
}
/*
// Text Box control with mask text (current raygui.h my implementation)
// NOTE: Returns true on ENTER pressed (useful for data validation)
bool DrawTextBox(Rectangle bounds, char *mainBuff, char *shadowBuff, int textSize, bool editMode, DWidgetEvent& EditEvent)
{
    #if !defined(RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN)
        #define RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN  40        // Frames to wait for autocursor movement
    #endif
    #if !defined(RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY)
        #define RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY      1        // Frames delay for autocursor movement
    #endif

    bool ret=false;
    GuiState state = guiState;

    bool multiline = false;     // TODO: Consider multiline text input
    int wrapMode = GuiGetStyle(DEFAULT, TEXT_WRAP_MODE);

    Rectangle textBounds = GetTextBounds(TEXTBOX, bounds);
    int textWidth = GetTextWidth(mainBuff) - GetTextWidth(mainBuff + textBoxCursorIndex);
    int textIndexOffset = 0;    // Text index offset to start drawing in the box

    // Cursor rectangle
    // NOTE: Position X value should be updated
    Rectangle cursor = {
        textBounds.x + textWidth + GuiGetStyle(DEFAULT, TEXT_SPACING),
        textBounds.y + textBounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE),
        2,
        (float)GuiGetStyle(DEFAULT, TEXT_SIZE)*2
    };

    if (cursor.height >= bounds.height) cursor.height = bounds.height - GuiGetStyle(TEXTBOX, BORDER_WIDTH)*2;
    if (cursor.y < (bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH))) cursor.y = bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH);

    // Mouse cursor rectangle
    // NOTE: Initialized outside of screen
    Rectangle mouseCursor = cursor;
    mouseCursor.x = -1;
    mouseCursor.width = 1;

    // Auto-cursor movement logic
    // NOTE: Cursor moves automatically when key down after some time
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_BACKSPACE) || IsKeyDown(KEY_DELETE)) autoCursorCooldownCounter++;
    else
    {
        autoCursorCooldownCounter = 0;      // GLOBAL: Cursor cooldown counter
        autoCursorDelayCounter = 0;         // GLOBAL: Cursor delay counter
    }

    // Blink-cursor frame counter
    //if (!autoCursorMode) blinkCursorFrameCounter++;
    //else blinkCursorFrameCounter = 0;

    // Update control
    //--------------------------------------------------------------------
    // WARNING: Text editing is only supported under certain conditions:
    if ((state != STATE_DISABLED) &&                // Control not disabled
        !GuiGetStyle(TEXTBOX, TEXT_READONLY) &&     // TextBox not on read-only mode
        !guiLocked &&                               // Gui not locked
        !guiControlExclusiveMode &&                       // No gui slider on dragging
        (wrapMode == TEXT_WRAP_NONE))               // No wrap mode
    {
        Vector2 mousePosition = GetMousePosition();

        if (editMode)
        {
            state = STATE_PRESSED;

            // If text does not fit in the textbox and current cursor position is out of bounds,
            // we add an index offset to text for drawing only what requires depending on cursor
            while (textWidth >= textBounds.width)
            {
                int nextCodepointSize = 0;
                GetCodepointNext(mainBuff + textIndexOffset, &nextCodepointSize);

                textIndexOffset += nextCodepointSize;

                textWidth = GetTextWidth(mainBuff + textIndexOffset) - GetTextWidth(mainBuff + textBoxCursorIndex);
            }

            // Get current text length
            int textLength = (int)strlen(mainBuff);
            int shadowLength = shadowBuff ? (int)strlen(shadowBuff) : 0; // Does not care in normal mode

            int codepoint = GetCharPressed();       // Get Unicode codepoint
            if (multiline && IsKeyPressed(KEY_ENTER)) codepoint = (int)'\n';

            if (textBoxCursorIndex > textLength)
            {
                textBoxCursorIndex = textLength;
                TextBoxShadowCursorIndex = shadowBuff ? shadowLength : 0;
            }

            // Encode codepobbint as UTF-8
            int codepointSize = 0;
            const char *charEncoded = CodepointToUTF8(codepoint, &codepointSize);

            // Add codepoint to text, at current cursor position
            // NOTE: Make sure we do not overflow buffer size
            if (((multiline && (codepoint == (int)'\n')) || (codepoint >= 32)) && ((textLength + codepointSize) < textSize))
            {
                if (!shadowBuff) // Normal mode
                { 
                    // Move forward data from cursor position
                    for (int i = (textLength + codepointSize); i > textBoxCursorIndex; i--) mainBuff[i] = mainBuff[i - codepointSize];

                    // Add new codepoint in current cursor position
                    for (int i = 0; i < codepointSize; i++) mainBuff[textBoxCursorIndex + i] = charEncoded[i];

                    textBoxCursorIndex += codepointSize;
                    textLength += codepointSize;
                    
                    // Make sure text last character is EOL
                    mainBuff[textLength] = '\0';
                }
                else // Password mode
                {
                    // ** shadowBuff **
                    // Move forward data from cursor position
                    for (int i = (shadowLength + codepointSize); i > TextBoxShadowCursorIndex; i--) shadowBuff[i] = shadowBuff[i - codepointSize];

                    // Add new codepoint in current cursor position
                    for (int i = 0; i < codepointSize; i++) shadowBuff[TextBoxShadowCursorIndex + i] = charEncoded[i];

                    TextBoxShadowCursorIndex += codepointSize;
                    shadowLength += codepointSize;
                    
                    // Make sure text last character is EOL
                    shadowBuff[shadowLength] = '\0';

                    // ** mainBuff **
                    // Only '*' for view, not need to move data (are all '*')
                    mainBuff[textBoxCursorIndex] = '*';
                    textBoxCursorIndex++;
                    textLength++;
                    // Make sure text last character is EOL
                    mainBuff[textLength] = '\0';
                }
            }

            // Move cursor to start
            if ((textLength > 0) && IsKeyPressed(KEY_HOME))
            {
                textBoxCursorIndex = 0;
                TextBoxShadowCursorIndex = 0;
            }

            // Move cursor to end
            if ((textLength > textBoxCursorIndex) && IsKeyPressed(KEY_END))
            {
                textBoxCursorIndex = textLength;
                TextBoxShadowCursorIndex = shadowBuff ? shadowLength : 0;
            }

            // Delete codepoint from text, after current cursor position
            if ((textLength > textBoxCursorIndex) && (IsKeyPressed(KEY_DELETE) || (IsKeyDown(KEY_DELETE) && (autoCursorCooldownCounter >= RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN))))
            {
                autoCursorDelayCounter++;

                if (IsKeyPressed(KEY_DELETE) || (autoCursorDelayCounter%RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY) == 0)      // Delay every movement some frames
                {
                    if (!shadowBuff) // Normal mode
                    {
                        int nextCodepointSize = 0;
                        GetCodepointNext(mainBuff + textBoxCursorIndex, &nextCodepointSize);

                        // Move backward text from cursor position
                        for (int i = textBoxCursorIndex; i < textLength; i++) mainBuff[i] = mainBuff[i + nextCodepointSize];

                        textLength -= codepointSize;

                        // Make sure text last character is EOL
                        mainBuff[textLength] = '\0';
                    }
                    else // Password mode
                    {
                        // ** shadowBuff **
                        int nextCodepointSize = 0;
                        GetCodepointNext(shadowBuff + TextBoxShadowCursorIndex, &nextCodepointSize);

                        // Move backward text from cursor position
                        for (int i = TextBoxShadowCursorIndex; i < shadowLength; i++) shadowBuff[i] = shadowBuff[i + nextCodepointSize];

                        shadowLength -= codepointSize;

                        // Make sure text last character is EOL
                        shadowBuff[shadowLength] = '\0';

                        // ** mainBuff **
                        // only cut 1 byte
                        textBoxCursorIndex--;
                        textLength--;
                        // Make sure text last character is EOL
                        mainBuff[textLength] = '\0';
                    }
                }
            }

            // Delete codepoint from text, before current cursor position
            if ((textLength > 0) && (IsKeyPressed(KEY_BACKSPACE) || (IsKeyDown(KEY_BACKSPACE) && (autoCursorCooldownCounter >= RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN))))
            {
                autoCursorDelayCounter++;

                if (IsKeyPressed(KEY_BACKSPACE) || (autoCursorDelayCounter%RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY) == 0)      // Delay every movement some frames
                {
                    if (!shadowBuff) // Normal mode
                    {
                        int prevCodepointSize = 0;
                        GetCodepointPrevious(mainBuff + textBoxCursorIndex, &prevCodepointSize);

                        // Move backward text from cursor position
                        for (int i = (textBoxCursorIndex - prevCodepointSize); i < textLength; i++) mainBuff[i] = mainBuff[i + prevCodepointSize];

                        // Prevent cursor index from decrementing past 0
                        if (textBoxCursorIndex > 0)
                        {
                            textBoxCursorIndex -= codepointSize;
                            textLength -= codepointSize;
                        }

                        // Make sure text last character is EOL
                        mainBuff[textLength] = '\0';
                    }
                    else // Password Mode
                    {
                        // ** shadowBuff **
                        int prevCodepointSize = 0;
                        GetCodepointPrevious(shadowBuff + TextBoxShadowCursorIndex, &prevCodepointSize);

                        // Move backward text from cursor position
                        for (int i = (TextBoxShadowCursorIndex - prevCodepointSize); i < shadowLength; i++) shadowBuff[i] = shadowBuff[i + prevCodepointSize];

                        // Prevent cursor index from decrementing past 0
                        if (TextBoxShadowCursorIndex > 0)
                        {
                            TextBoxShadowCursorIndex -= codepointSize;
                            shadowLength -= codepointSize;
                        }

                        // Make sure text last character is EOL
                        shadowBuff[textLength] = '\0';

                        // ** mainBuff **
                        // only cut 1 byte
                        textBoxCursorIndex--;
                        textLength--;
                        // Make sure text last character is EOL
                        mainBuff[textLength] = '\0';
                    }
                }
            }

            // Move cursor position with keys
            if (IsKeyPressed(KEY_LEFT) || (IsKeyDown(KEY_LEFT) && (autoCursorCooldownCounter > RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN)))
            {
                autoCursorDelayCounter++;

                if (IsKeyPressed(KEY_LEFT) || (autoCursorDelayCounter%RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY) == 0)      // Delay every movement some frames
                {
                    if (!shadowBuff) // Normal mode
                    {
                        int prevCodepointSize = 0;
                        GetCodepointPrevious(mainBuff + textBoxCursorIndex, &prevCodepointSize);

                        if (textBoxCursorIndex >= prevCodepointSize) textBoxCursorIndex -= prevCodepointSize;
                    }
                    else // Password mode
                    {
                        // ** shadowBuff **
                        int prevCodepointSize = 0;
                        GetCodepointPrevious(shadowBuff + TextBoxShadowCursorIndex, &prevCodepointSize);

                        if (TextBoxShadowCursorIndex >= prevCodepointSize) TextBoxShadowCursorIndex -= prevCodepointSize;

                        // ** mainBuff **
                        if (textBoxCursorIndex >= 0) textBoxCursorIndex --;
                    }
                }
            }
            else if (IsKeyPressed(KEY_RIGHT) || (IsKeyDown(KEY_RIGHT) && (autoCursorCooldownCounter > RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN)))
            {
                autoCursorDelayCounter++;

                if (IsKeyPressed(KEY_RIGHT) || (autoCursorDelayCounter%RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY) == 0)      // Delay every movement some frames
                {
                    if (!shadowBuff) // Normal mode
                    {
                        int nextCodepointSize = 0;
                        GetCodepointNext(mainBuff + textBoxCursorIndex, &nextCodepointSize);

                        if ((textBoxCursorIndex + nextCodepointSize) <= textLength) textBoxCursorIndex += nextCodepointSize;
                    }
                    else // Password mode
                    {
                        // ** ShadowBuff **
                        int nextCodepointSize = 0;
                        GetCodepointNext(shadowBuff + TextBoxShadowCursorIndex, &nextCodepointSize);

                        if ((TextBoxShadowCursorIndex + nextCodepointSize) <= shadowLength) TextBoxShadowCursorIndex += nextCodepointSize;

                        // ** mainBuff **
                        if ((textBoxCursorIndex + 1) <= textLength) textBoxCursorIndex ++;
                    }
                }
            }

            // Move cursor position with mouse
            if (CheckCollisionPointRec(mousePosition, textBounds))     // Mouse hover text
            {
                float scaleFactor = (float)GuiGetStyle(DEFAULT, TEXT_SIZE)/(float)guiFont.baseSize;
                int codepointIndex = 0;
                float glyphWidth = 0.0f;
                float widthToMouseX = 0;
                int mouseCursorIndex = 0;

                for (int i = textIndexOffset; i < textLength; i++)
                {
                    codepoint = GetCodepointNext(&mainBuff[i], &codepointSize);
                    codepointIndex = GetGlyphIndex(guiFont, codepoint);

                    if (guiFont.glyphs[codepointIndex].advanceX == 0) glyphWidth = ((float)guiFont.recs[codepointIndex].width*scaleFactor);
                    else glyphWidth = ((float)guiFont.glyphs[codepointIndex].advanceX*scaleFactor);

                    if (mousePosition.x <= (textBounds.x + (widthToMouseX + glyphWidth/2)))
                    {
                        mouseCursor.x = textBounds.x + widthToMouseX;
                        mouseCursorIndex = i;
                        break;
                    }

                    widthToMouseX += (glyphWidth + (float)GuiGetStyle(DEFAULT, TEXT_SPACING));
                }

                // Check if mouse cursor is at the last position
                int textEndWidth = GetTextWidth(mainBuff + textIndexOffset);
                if (GetMousePosition().x >= (textBounds.x + textEndWidth - glyphWidth/2))
                {
                    mouseCursor.x = textBounds.x + textEndWidth;
                    mouseCursorIndex = (int)strlen(mainBuff);
                }

                // Place cursor at required index on mouse click
                if ((mouseCursor.x >= 0) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    cursor.x = mouseCursor.x;
                    textBoxCursorIndex = mouseCursorIndex;
                    if (shadowBuff) // Password mode
                    {
                        // Calculate index position in shadowBuff
                        int nextCodepointSize = 0;
                        int shadowIndex=0;
                        for (int i = 0; i < mouseCursorIndex; i++){
                            GetCodepointNext(mainBuff + textBoxCursorIndex, &nextCodepointSize);
                            shadowIndex+=nextCodepointSize;
                        }
                        TextBoxShadowCursorIndex = shadowIndex;
                    }
                }
            }
            else mouseCursor.x = -1;

            // Recalculate cursor position.y depending on textBoxCursorIndex
            cursor.x = bounds.x + GuiGetStyle(TEXTBOX, TEXT_PADDING) + GetTextWidth(mainBuff + textIndexOffset) - GetTextWidth(mainBuff + textBoxCursorIndex) + GuiGetStyle(DEFAULT, TEXT_SPACING);
            //if (multiline) cursor.y = GetTextLines()

            // Finish text editing on ENTER or mouse click outside bounds
            if (!multiline && IsKeyPressed(KEY_ENTER)) {
                EditEvent.EventCode=KEY_PRESSED;
                EditEvent.EventNum=KEY_ENTER;
                ret=true;
            }
            else if(!CheckCollisionPointRec(mousePosition, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                EditEvent.EventCode=EDIT_END;
                EditEvent.EventNum=0;
                ret=true;
            }

            if (ret) {
                textBoxCursorIndex = 0;     // GLOBAL: Reset the shared cursor index
                TextBoxShadowCursorIndex = 0;
            }
        }
        else
        {
            if (CheckCollisionPointRec(mousePosition, bounds))
            {
                state = STATE_FOCUSED;

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    textBoxCursorIndex = (int)strlen(mainBuff);   // GLOBAL: Place cursor index to the end of current text
                    TextBoxShadowCursorIndex = shadowBuff ? (int)strlen(shadowBuff) : 0;
                    ret=true;
                }
            }
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (state == STATE_PRESSED)
    {
        GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_PRESSED)));
    }
    else if (state == STATE_DISABLED)
    {
        GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)));
    }
    else GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), BLANK);

    // Draw text considering index offset if required
    // NOTE: Text index offset depends on cursor position
    GuiDrawText(mainBuff + textIndexOffset, textBounds, GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT), GetColor(GuiGetStyle(TEXTBOX, TEXT + (state*3))));

    // Draw cursor
    if (editMode && !GuiGetStyle(TEXTBOX, TEXT_READONLY))
    {
        //if (autoCursorMode || ((blinkCursorFrameCounter/40)%2 == 0))
        GuiDrawRectangle(cursor, 0, BLANK, GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED)));

        // Draw mouse position cursor (if required)
        if (mouseCursor.x >= 0) GuiDrawRectangle(mouseCursor, 0, BLANK, GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED)));
    }
    else if (state == STATE_FOCUSED) GuiTooltip(bounds);
    //--------------------------------------------------------------------

    return ret;
}
*/