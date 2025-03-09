#ifndef DGuiCommon_H
#define DGuiCommon_H

#include <functional>
#include "raygui.h"
#include <string>
#include <map>

struct DJsonTree {
            inline static const std::string SEC_CHILDREN="Children";
            inline static const std::string SEC_BOUNDS="Bounds";
            inline static const std::string SEC_ITEMS="Items";
            inline static const std::string SEC_STATIC="Static";
            inline static const std::string SEC_DYNAMIC="Dynamic";

            inline static const std::string ITEM_NAME="Name";
            inline static const std::string ITEM_ANCHOR="Anchor";
            inline static const std::string ITEM_TYPE="Type";
            inline static const std::string ITEM_TEXT="Text";
            inline static const std::string ITEM_TEXT_SIZE="TextSize";
            inline static const std::string ITEM_TEXT_SIDE="TextSide";
            inline static const std::string ITEM_TEXT_COLOR="TextColor";
            inline static const std::string ITEM_TEXT_SPACING="TextSpacing";
            inline static const std::string ITEM_TEXT_PADDING="TextPadding";
            inline static const std::string ITEM_BORDER_COLOR="BorderColor";
            inline static const std::string ITEM_BORDER_WIDTH="BorderWidth";
            inline static const std::string ITEM_BORDER_VISIBLE="BorderVisible";
            inline static const std::string ITEM_LINE_COLOR="LineColor";
            inline static const std::string ITEM_BACKGROUND_COLOR="BackgroundColor";
            inline static const std::string ITEM_TEXT_ALIGN_H="TextAlignH";
            inline static const std::string ITEM_TEXT_ALIGN_V="TextAlignV";
            inline static const std::string ITEM_LEFT="Left";
            inline static const std::string ITEM_TOP="Top";
            inline static const std::string ITEM_WIDTH="Width";
            inline static const std::string ITEM_HEIGHT="Height";
            inline static const std::string ITEM_DOCKING="Docking";
            inline static const std::string ITEM_SIDE="Side";
            inline static const std::string ITEM_SIDE_OF="SideOf";
            inline static const std::string ITEM_SIDE_ALIGN="SideAlign";
            inline static const std::string ITEM_SIZE="Size";
            inline static const std::string ITEM_OFFSET="Offset";
            inline static const std::string ITEM_BOUNDS="Bounds";
            inline static const std::string ITEM_READ_ONLY="ReadOnly";
            inline static const std::string ITEM_PASSWORD_MODE="PasswordMode";
            inline static const std::string ITEM_MAX_TEXT_LENGHT="MaxTextLenght";
            inline static const std::string ITEM_ENABLED="Enabled";
            inline static const std::string ITEM_VISIBLE="Visible";
            inline static const std::string ITEM_SHOW_BORDER="ShowBorder";
            inline static const std::string ITEM_FILENAME="Filename";
            inline static const std::string ITEM_ROTATION="Rotation";
            inline static const std::string ITEM_SCALE="Scale";
            inline static const std::string ITEM_LABEL="Label";
            inline static const std::string ITEM_PREFIX="Prefix";
            inline static const std::string ITEM_SUFFIX="Suffix";
            inline static const std::string ITEM_ON_COLOR="OnColor";
            inline static const std::string ITEM_OFF_COLOR="OffColor";
            inline static const std::string ITEM_SWITCHED_ON="SwitchedOn";
            inline static const std::string ITEM_CENTER_X="CenterX";
            inline static const std::string ITEM_CENTER_Y="CenterY";
            inline static const std::string ITEM_RADIUS="Radius";
            inline static const std::string ITEM_RIGHT_OF="RightOf";
            inline static const std::string ITEM_LEFT_OF="LeftOf";
            inline static const std::string ITEM_BOTTOM_OF="BottomOf";
            inline static const std::string ITEM_TOP_OF="TopOf";
            inline static const std::string ITEM_CAPTION="Caption";

            inline static const std::string VALUE_BOTTOM="Bottom";
            inline static const std::string VALUE_TOP="Top";
            inline static const std::string VALUE_LEFT="Left";
            inline static const std::string VALUE_RIGHT="Right";
            inline static const std::string VALUE_CENTER="Center";
            inline static const std::string VALUE_HCENTER="CenterOriz";
            inline static const std::string VALUE_VCENTER="CenterVert";
            inline static const std::string VALUE_JSON="Json";
        };

//typedef GuiControl DWidgetType;
// Re-define gui controls
typedef enum {
    UNKNOWN = -1,
    DCONTAINER = 0,

    // Basic controls
    DLABEL,          // Used also for: LABELBUTTON
    DBUTTON,
    DTOGGLE,         // Used also for: TOGGLEGROUP
    DSLIDER,         // Used also for: SLIDERBAR, TOGGLESLIDER
    DPROGRESSBAR,
    DCHECKBOX,
    DCOMBOBOX,
    DDROPDOWNBOX,
    DEDIT,          // DTEXTBOX used also for: TEXTBOXMULTI
    DVALUEBOX,
    DSPINNER,        // Uses: BUTTON, VALUEBOX
    DLISTVIEW,
    DCOLORPICKER,
    DSCROLLBAR,
    DSTATUSBAR,
    DIMAGE,
    DTOGGLESLIDE,
    DTOGGLESWITCH,
    DLED,
} DWidgetType;

enum DDocking { DOCK_HCENTER=-7, DOCK_VCENTER=-6, DOCK_CENTER=-5, DOCK_TOP=-4, DOCK_BOTTOM=-3, DOCK_RIGHT=-2, DOCK_LEFT=-1 };
enum DSide { SIDE_LEFT = DOCK_LEFT, SIDE_RIGHT = DOCK_RIGHT, SIDE_BOTTOM = DOCK_BOTTOM, SIDE_TOP = DOCK_TOP, SIDE_CUSTOM = 0}; /// Generic side
enum DAnchorSide { ANCHOR_NONE, ANCHOR_RIGHT_OF, ANCHOR_LEFT_OF, ANCHOR_BOTTOM_OF, ANCHOR_TOP_OF };
enum DWidthMode { WIDTH_AUTO=-2, WIDTH_PARENT=-1, WIDTH_DEFAULT=0 };
enum DAlign { ALIGN_LEFT, ALIGN_RIGHT, ALIGN_BOTTOM, ALIGN_TOP, ALIGN_CENTER }; /// Generic Align
enum DTextAlignH { TEXT_ALIGN_HLEFT=TEXT_ALIGN_LEFT, TEXT_ALIGN_HCENTER=TEXT_ALIGN_CENTER, TEXT_ALIGN_HRIGHT=TEXT_ALIGN_RIGHT }; /// Text horizontal align
enum DTextAlignV { TEXT_ALIGN_VTOP=TEXT_ALIGN_TOP, TEXT_ALIGN_VCENTER=TEXT_ALIGN_CENTER, TEXT_ALIGN_VBOTTOM=TEXT_ALIGN_BOTTOM }; /// Text vertical align
typedef struct DTextAlign{
    DTextAlignH Horiz=TEXT_ALIGN_HLEFT;
    DTextAlignV Vert=TEXT_ALIGN_VCENTER;
} DTextAlign;

enum DEventCode { UNKOWN_EVENT=-1, BUTTON_PRESS, EDIT_END, TOGGLE_CHANGED, KEY_PRESSED };

// Rapresent a widget event
typedef struct _DWidgetEvent{
    DEventCode EventCode;
    int EventNum;
    void *EventData;
} DWidgetEvent;

// Rapresent a global event ( a widget event plus widget info)
typedef struct _DGuiEvent{
    DWidgetType WidgetType;
    std::string WidgetId; /// @todo deprecare
    uintptr_t WidgetUid;
    DWidgetEvent WidgetEvent;    
} DGuiEvent;

// Global event callback
typedef std::function<void (DGuiEvent Event)> OnGuiEventCallback;
// Widget event callback
typedef std::function<bool (DWidgetEvent Event)> OnWidgetEventCallback;
#endif