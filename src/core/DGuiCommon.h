#ifndef DGuiCommon_H
#define DGuiCommon_H

#include <functional>
#include "raygui.h"
#include <string>
#include <map>

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
} DWidgetType;

enum DEventCode { BUTTON_PRESS, EDIT_END, TOGGLE_CHANGED };

// Rapresent a widget event
typedef struct _DWidgetEvent{
    DEventCode EventCode;
    void *EventData;
} DWidgetEvent;

// Rapresent a global event ( a widget event plus widget info)
typedef struct _DGuiEvent{
    DWidgetType WidgetType;
    std::string WidgetId;
    DWidgetEvent WidgetEvent;    
} DGuiEvent;

// Global event callback
typedef std::function<void (DGuiEvent Event)> OnGuiEventCallback;
// Widget event callback
typedef std::function<bool (DWidgetEvent Event)> OnWidgetEventCallback;
#endif