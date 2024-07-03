#include "DGuiEdit.h"
#include "raywui_log.h"
#include <string.h>
// Defined here because DGuiEdit::DrawTextBox() is a custom implementation of GuiTextBox()
//#define RAYGUI_IMPLEMENTATION
//#include <raygui.h>

#define DEFAULT_MAX_TEXT_LENGHT 128

const char TAG[9]="DGuiEdit";

DGuiEdit::DGuiEdit(int LeftPos, int TopPos, int WidgetWidth, int WidgetHeight, DGuiWidget *ParentWidget) : DGuiWidget(DEDIT,LeftPos,TopPos,WidgetWidth,WidgetHeight,ParentWidget) {
    InitDefault();
}

DGuiEdit::DGuiEdit(Rectangle WidgetBounds, DGuiWidget *ParentWidget) : DGuiWidget(DEDIT,WidgetBounds,ParentWidget) {
    InitDefault();
}

DGuiEdit::DGuiEdit(DTools::DTree WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback) : DGuiWidget(WidgetTree,ParentWidget,EventCallback)
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
    delete viewBuff;
    if (hideBuff) {
        delete hideBuff;
    }
}

void DGuiEdit::InitDefault(void)
{
    EditMode=false;
    ReadOnly=false;
    PasswordMode=false;
    MaxTextLenght=DEFAULT_MAX_TEXT_LENGHT;
    viewBuff=new char[MaxTextLenght+1];
    memset(viewBuff,'\0',MaxTextLenght+1);
    hideBuff=nullptr;
    textBoxShadowCursorIndex=0;
    DEFAULT_SIDE_SIZE=50;
    DEFAULT_WIDTH=50;
    DEFAULT_HEIGHT=20;
}

void DGuiEdit::FinalizeFromTree(DTools::DTree& WidgetTree)
{
    // ** Read class specific properties **
    // ReadOnly
    bool ro=WidgetTree.ReadBool(DJsonTree::ITEM_READ_ONLY,false);
    SetReadOnly(ro);
    // PasswordMode
    bool pm=WidgetTree.ReadBool(DJsonTree::ITEM_PASSWORD_MODE,false);
    SetPasswordMode(pm);
    // MaxTextLenght
    SetMaxTextLenght(WidgetTree.ReadInteger(DJsonTree::ITEM_MAX_TEXT_LENGHT,0));
}

/**
 * @brief Set max text lenght size.
 * String lenght greater of, will be truncated.
 * 
 * @param Lenght    ->  New max text lenght size. If set to 0, default value is assigned.
 */
void DGuiEdit::SetMaxTextLenght(size_t NewLenght) {
    if ((NewLenght == 0)) {
        NewLenght=DEFAULT_MAX_TEXT_LENGHT;
    }

    if (NewLenght == MaxTextLenght) {
        return;
    }

    MaxTextLenght=NewLenght;

    if (PasswordMode) {
        // In password mode real text buffer is hideBuff
        // Store
        Text.assign(hideBuff);
        // reallocate buffer
        if (hideBuff) {
            delete hideBuff;
        }
        hideBuff=new char[MaxTextLenght+1];
        memset(hideBuff,'\0',MaxTextLenght+1);
        // re-assign
        strcpy(hideBuff,Text.c_str());

        // viewBuff is used for masked view
        delete viewBuff;
        viewBuff=new char[MaxTextLenght+1];
        memset(viewBuff,'\0',MaxTextLenght+1);
    }
    else {
        // NO password mode (only viewBuff is used)
        // Store
        Text.assign(viewBuff);
        // reallocate buffer
        delete viewBuff;
        viewBuff=new char[MaxTextLenght+1];
        memset(viewBuff,'\0',MaxTextLenght+1);
        // re-assign
        strcpy(viewBuff,Text.c_str());
    }
}

size_t DGuiEdit::GetMaxTextLenght(void) {
    return MaxTextLenght;
}

void DGuiEdit::SetReadOnly(bool Enabled) {
    ReadOnly=Enabled;
}

bool DGuiEdit::GetReadOnly(void) {
    return ReadOnly;
}

void DGuiEdit::SetPasswordMode(bool Enabled) {
    if (PasswordMode == Enabled) {
        return;
    }

    if (Enabled) {
        // From disable to enable
        if (hideBuff) {
            // to be sure...
            delete hideBuff;
        }
        hideBuff=new char[MaxTextLenght+1];
        // Copy real text in hideBuff
        strcpy(hideBuff,viewBuff);
        // set cursor index
        textBoxShadowCursorIndex=GetGuiTextBoxCursorIndex();
        // Set viewBuff with '*'
        memset(viewBuff,'*',strlen(hideBuff));
        // Ri-calculate index position in viewBuff
        int prevCodepointSize = 0;
        int index=0;
        // Move backward text from cursor position
        for (int i = (textBoxShadowCursorIndex - prevCodepointSize); i < strlen(hideBuff); i++) {
            GetCodepointPrevious(hideBuff + textBoxShadowCursorIndex, &prevCodepointSize);
            index++;
        }
        SetGuiTextBoxCursorIndex(index);
    }
    else {
        // From enable to disable
        if (hideBuff) { //...to be sure
            // Copy real text in viewBuff
            strcpy(viewBuff,hideBuff);
            // delete highBuff
            delete hideBuff;
        }
        hideBuff=nullptr;
        SetGuiTextBoxCursorIndex(textBoxShadowCursorIndex);
        textBoxShadowCursorIndex=0;
    }

    PasswordMode=Enabled;
}

bool DGuiEdit::GetPasswordMode(void) {
    return PasswordMode;
}

void DGuiEdit::ClearText(void) {
    //strcpy(viewBuff,"");
    memset(viewBuff,'\0',MaxTextLenght+1);
    if (hideBuff) {
        memset(hideBuff,'\0',MaxTextLenght+1);
        textBoxShadowCursorIndex=0;
    }
    Text.clear();
}

const std::string& DGuiEdit::GetText(void) {
    if (hideBuff) {
        // Password mode
        Text.assign(hideBuff);    
    }
    else {
        // Normal mode
        Text.assign(viewBuff);
    }
    
    return Text;
}

/**
 * @brief Get the direct pointer to text buffer.
 * 
 * @return char* 
 */
char* DGuiEdit::GetTextPtr(void) {
    if (hideBuff) {
        // Password mode
        return hideBuff;
    }
    else {
        // Normal mode
        return viewBuff;
    }
}

void DGuiEdit::SetText(std::string NewText) {
    if (hideBuff) {
        // Password mode
        if (strcmp(hideBuff,NewText.c_str()) == 0) {
            // Same
            return;
        }
        strcpy(hideBuff,NewText.c_str());
        Text.assign(hideBuff);
    }
    else {
        // NormalMode
        if (strcmp(viewBuff,NewText.c_str()) == 0) {
            // Same
            return;
        }
        strcpy(viewBuff,NewText.c_str());
        Text.assign(viewBuff);
    }
}

bool DGuiEdit::IsEmpty(void) {
    if (hideBuff) {
        // Password mode
        return (strlen(hideBuff) == 0);
    }
    else {
        // Normal mode
        return (strlen(viewBuff) == 0);
    }
}

/**
 * @brief Draw the label.
 * Label is a static widget, so should be drawn only if something changes.
 */

void DGuiEdit::Draw()
{
    bool CurrReadOnly=GuiGetStyle(Type,TEXT_READONLY);
    GuiSetStyle(Type,TEXT_READONLY,ReadOnly);

    if (GuiTextBoxMasked(Bounds,viewBuff,hideBuff,MaxTextLenght,EditMode)) {
        if (EditMode) {
            DWidgetEvent Event={DEventCode::EDIT_END, nullptr};
            SendEvent(Event);
        }
        EditMode=!EditMode;
    }

    GuiSetStyle(Type,TEXT_READONLY,CurrReadOnly);
}
/*
// Text Box control
// NOTE: Returns true on ENTER pressed (useful for data validation)
int DGuiEdit::DrawTextBox(Rectangle bounds, char *mainBuff, char *shadowBuff, int textSize, bool editMode)
{
    #if !defined(RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN)
        #define RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN  40        // Frames to wait for autocursor movement
    #endif
    #if !defined(RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY)
        #define RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY      1        // Frames delay for autocursor movement
    #endif

    int result = 0;
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
                textBoxShadowCursorIndex = shadowBuff ? shadowLength : 0;
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
                    for (int i = (shadowLength + codepointSize); i > textBoxShadowCursorIndex; i--) shadowBuff[i] = shadowBuff[i - codepointSize];

                    // Add new codepoint in current cursor position
                    for (int i = 0; i < codepointSize; i++) shadowBuff[textBoxShadowCursorIndex + i] = charEncoded[i];

                    textBoxShadowCursorIndex += codepointSize;
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
            if ((textLength > 0) && IsKeyPressed(KEY_HOME)) textBoxCursorIndex = 0;

            // Move cursor to end
            if ((textLength > textBoxCursorIndex) && IsKeyPressed(KEY_END)) textBoxCursorIndex = textLength;

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
                        GetCodepointNext(shadowBuff + textBoxShadowCursorIndex, &nextCodepointSize);

                        // Move backward text from cursor position
                        for (int i = textBoxShadowCursorIndex; i < shadowLength; i++) shadowBuff[i] = shadowBuff[i + nextCodepointSize];

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
                        GetCodepointPrevious(shadowBuff + textBoxShadowCursorIndex, &prevCodepointSize);

                        // Move backward text from cursor position
                        for (int i = (textBoxShadowCursorIndex - prevCodepointSize); i < shadowLength; i++) shadowBuff[i] = shadowBuff[i + prevCodepointSize];

                        // Prevent cursor index from decrementing past 0
                        if (textBoxShadowCursorIndex > 0)
                        {
                            textBoxShadowCursorIndex -= codepointSize;
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
                        GetCodepointPrevious(shadowBuff + textBoxShadowCursorIndex, &prevCodepointSize);

                        if (textBoxShadowCursorIndex >= prevCodepointSize) textBoxShadowCursorIndex -= prevCodepointSize;

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
                        GetCodepointNext(shadowBuff + textBoxShadowCursorIndex, &nextCodepointSize);

                        if ((textBoxShadowCursorIndex + nextCodepointSize) <= shadowLength) textBoxShadowCursorIndex += nextCodepointSize;

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
                        textBoxShadowCursorIndex = shadowIndex;
                    }
                }
            }
            else mouseCursor.x = -1;

            // Recalculate cursor position.y depending on textBoxCursorIndex
            cursor.x = bounds.x + GuiGetStyle(TEXTBOX, TEXT_PADDING) + GetTextWidth(mainBuff + textIndexOffset) - GetTextWidth(mainBuff + textBoxCursorIndex) + GuiGetStyle(DEFAULT, TEXT_SPACING);
            //if (multiline) cursor.y = GetTextLines()

            // Finish text editing on ENTER or mouse click outside bounds
            if ((!multiline && IsKeyPressed(KEY_ENTER)) ||
                (!CheckCollisionPointRec(mousePosition, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
            {
                textBoxCursorIndex = 0;     // GLOBAL: Reset the shared cursor index
                result = 1;
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
                    result = 1;
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

    return result;      // Mouse button pressed: result = 1
}
*/
