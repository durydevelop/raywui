#ifndef DGuiWidget_H
#define DGuiWidget_H

#include <raygui.h>
#include <string>
#include <map>
#include <DGuiCommon.h>
#include <dpplib/DPreferences.h>
#include <dpplib/DFilesystem.h>

struct DRglControl {
    DWidgetType WidgetType=DWidgetType::UNKNOWN;
    Rectangle Bounds{0,0,0,0};
    std::string Text;
    std::string Name;
};

/**
 * @brief DGuiWidget base class.
 * This is the base class for all widgets from which all widgets must be descended.
 */
class DGuiWidget
{
    protected:
        inline static const std::map<DWidgetType,std::string> WidgetTypes = {
            {DLABEL,        "Label"         },
            {DBUTTON,       "Button"        },
            {DEDIT,         "Edit"          },
            {DSTATUSBAR,    "StatusBar"     },
            {DCONTAINER,    "Container"     },
            {DIMAGE,        "Image"         },
            {DTOGGLESLIDE,  "ToggleSlide"   },
            {DTOGGLESWITCH, "ToggleSwitch"  },
            {DLED,          "Led"           },
        };

        inline static const std::map<DDocking,std::string> Dockings = {
            {DOCK_LEFT,     DJsonTree::VALUE_LEFT       },
            {DOCK_RIGHT,    DJsonTree::VALUE_RIGHT      },
            {DOCK_BOTTOM,   DJsonTree::VALUE_BOTTOM     },
            {DOCK_TOP,      DJsonTree::VALUE_TOP        },
            {DOCK_CENTER,   DJsonTree::VALUE_CENTER     },
            {DOCK_HCENTER,  DJsonTree::VALUE_HCENTER    },
            {DOCK_VCENTER,  DJsonTree::VALUE_VCENTER    },
        };

        inline static const std::map<DSide,std::string> Sides = {
            {SIDE_LEFT,     DJsonTree::VALUE_LEFT   },
            {SIDE_RIGHT,    DJsonTree::VALUE_RIGHT  },
            {SIDE_BOTTOM,   DJsonTree::VALUE_BOTTOM },
            {SIDE_TOP,      DJsonTree::VALUE_TOP    },
        };

        inline static const std::map<DAnchorSide,std::string> AnchorSides = {
            {ANCHOR_RIGHT_OF,   DJsonTree::ITEM_RIGHT_OF    },
            {ANCHOR_LEFT_OF,    DJsonTree::ITEM_LEFT_OF    },
            {ANCHOR_BOTTOM_OF,  DJsonTree::ITEM_BOTTOM_OF    },
            {ANCHOR_TOP_OF,     DJsonTree::ITEM_TOP_OF    },
        };

        inline static const std::map<DAlign,std::string> Aligns = {
            {ALIGN_RIGHT,   DJsonTree::ITEM_RIGHT   },
            {ALIGN_LEFT,    DJsonTree::ITEM_LEFT    },
            {ALIGN_BOTTOM,  DJsonTree::ITEM_BOTTOM  },
            {ALIGN_TOP,     DJsonTree::ITEM_TOP     },
            {ALIGN_CENTER,  DJsonTree::ITEM_CENTER  },
            {ALIGN_HCENTER, DJsonTree::ITEM_HCENTER },
            {ALIGN_VCENTER, DJsonTree::ITEM_VCENTER }
        };

    public:
        typedef struct DAnchor{
            DAnchorSide AnchorToSide;
            DAlign AlignToSide;
            std::string WidgetName;
            int AnchorOffset;
        } DAnchor;

        struct DLabel{
            DGuiWidget *Widget=nullptr;
            DSide Side=SIDE_LEFT;
            int OffsetX=5;
            int OffsetY=5;
        };

        struct DProperties {
            // ** Apparence **
            /// Defaults are set in SetWidgetType()
            unsigned int TextColor;
            int TextPadding;
            int TextSize;
            int TextSpacing;
            DTextAlign TextAlign;
            unsigned int BackGroundColor;
            unsigned int BorderColor;
            int BorderWidth;
            bool BorderVisible; /// Used to override border visible in widgets that does not show it.
            unsigned int LineColor;
            Font TextFont;
            DAnchor Anchor;

            /// Defauts set here
            // ** Behaviours **
            bool Enabled=true;
            bool Visible=true;
            std::map<DAlign,int> ParentAligns; /// Parent aligns,offsets
            
        }Properties;
        
        DGuiWidget(DWidgetType WidgetType, int LeftPos, int TopPos, int WidgetWidth, int WidgetHeight, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiWidget(DWidgetType WidgetType, Rectangle WidgetBounds, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiWidget(DWidgetType WidgetType, DDocking DockingPos, int OtherSize, DGuiWidget *ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        DGuiWidget(DTools::DTree& WidgetTree, DGuiWidget* ParentWidget, OnWidgetEventCallback EventCallback = nullptr);
        virtual ~DGuiWidget();
        
        static DGuiWidget* New(DTools::DTree& WidgetTree, DGuiWidget* ParentWidget = nullptr, OnWidgetEventCallback EventCallback = nullptr);
        static DGuiWidget* New(const std::string& LayoutFilename, DGuiWidget* ParentWidget = nullptr, OnWidgetEventCallback EventCallback = nullptr);

        /// Virtual method that MUST be implemented by sub-class
        virtual void Draw() = 0;
        virtual void UpdateSize(void);
        
        /// Virtual methods that CAN be reimplemented
        virtual void SetText(std::string NewText, bool ForceAutoSize = false);
        virtual const std::string& GetText(void);
        virtual void SetOnGuiEvent(OnGuiEventCallback Callback);
        virtual DGuiWidget* FindWidgetByName(std::string WidgetName); /// Reimplement if  the sub-class contains widgets (like DGuiContainer)
        virtual void SetPos(int LeftPos, int TopPos);
        virtual void SetSize(int Width, int Height);

        /// Static methods
        static DTools::DTree ExtractDTree(const DTools::fs::path& Filename);
        static std::string RglToJson(std::string Filename);
        static DRglControl DecodeRglLine(std::string Line);
        static DWidgetType NameToType(const std::string& WidgetTypeName);
        static std::string TypeToName(DWidgetType WidgetType);
        static DDocking NameToDocking(const std::string& DockingSideName, DDocking Default);
        static std::string DockingToName(DDocking DockingPos);
        static DSide NameToSide(const std::string& SideName, DSide Default);
        static std::string SideToName(DSide Side);
        static DAnchorSide NameToAnchor(const std::string& AnchorSideName);
        static std::string AnchorToName(DAnchorSide AnchorSide);
        static DAlign NameToAlign(const std::string& AlignName, DAlign Default);
        static std::string AlignToName(DAlign Align);
        static unsigned int ColorStringToInt(std::string ColorString);

        /// Methods
        void Clear(void);
        void Draws(void);

        void SetWidgetType(DWidgetType WidgetType);
        void SetTextSize(int NewSize, bool ForceAutoSize);
        void SetTextSpacing(int NewSpacing, bool ForceAutoSize);
        void SetTextPadding(int NewPadding, bool ForceAutoSize);
        void SetWidth(int Width);
        void SetHeight(int Height);
        void SetParent(DGuiWidget *Parent);
        void SetTextAlign(std::string AlignHoriz, std::string AlignVert);
        void SetOnWidgetEvent(OnWidgetEventCallback Callback);
        void SendEvent(DWidgetEvent WidgetEvent);
        void SetDocking(DDocking DockingPos, int OtherSize);
        void SetDocking(std::string DockingSideName, int OtherSize);
        Rectangle GetTextBounds(void);
        void SetLabel(std::string LabelText, int FontSize, DSide LabelSide, uint SideOffset);
        void SetLabel(std::string LabelText, int FontSize, DSide LabelSide, int OffsetX, int OffsetY);
        
        void UpdateLabel(void);
        bool UpdateAnchor(void);
        void UpdateParentAligns(void);
        void SetBounds(int LeftPos, int TopPos, int Width, int Height);
        void SetBounds(Rectangle WidgetBounds);
        bool SetAnchor(DAnchor Anchor);
        bool SetAnchor(DAnchorSide AnchorSide, DAlign SideAlign, std::string WidgetName, int Offset);
        void SetParentAligns(std::map<std::string,int> AlignList);
        void AddParentAlign(std::string AlignName, int AlignOffset,bool ForceUpdate);
        void SetBorderWidth(uint8_t NewWidth);
        void SetBorderVisible(bool Visible);
        void SetEnabled(bool Enabled);
        void SetVisible(bool Visible);

        int GetTextSize(void);
        int GetTextWidth(std::string TextStr, Font TextFont, float FontSize);
        size_t GetWidth(void);
        size_t GetHeight(void);
        Rectangle GetAbsBounds(void);
        DGuiWidget* GetParent(void);
        DWidgetType GetWidgetType(void);
        std::string GetWidgetTypeName(void);
        std::string GetId(void);
        std::string GetLastError(void);
        bool IsReady(void);

        // Raygui implemetation wrappers
        GuiState GetGuiState(void);
        int GetGuiTextBoxCursorIndex(void);
        void SetGuiTextBoxCursorIndex(int cursorIndex);
        bool IsGuiLocked(void);
        float GetGuiAlpha(void);

        void RayGuiDrawRectangle(Rectangle Bounds, int BorderWidth, Color BorderColor, Color Tint);
        void RayGuiDrawText(const char *TextStr, Rectangle TextBounds, int Alignment, Color Tint);
        void RayGuiDrawText(std::string TextStr, Rectangle TextBounds, DTextAlign alignment, Color tint);

        std::string Name;
        Rectangle Bounds;
        std::string Text;
        //Rectangle TextBounds;
        uintptr_t Uid;

    protected:
        int DEFAULT_SIDE_SIZE=20;   //! Used for set docking position, will be overrided by widget subclass
        int DEFAULT_WIDTH=50;       //! Used for set default witdh, will be overrided by widget subclass
        int DEFAULT_HEIGHT=50;      //! Used for set default height, will be overrided by widget subclass

        DWidgetType Type;
        DGuiWidget *Parent=nullptr;
        DLabel Label;
        
        OnGuiEventCallback OnGuiEvent;
        OnWidgetEventCallback OnWidgetEvent;
        std::string LastError;
        bool Ready;

        void GenerateId(void);
        bool InitFromTree(DTools::DTree& WidgetTree);
        void BackupCurrentGuiStyle(void);
        void UpdateCurrentGuiStyle(void);
        void RestoreCurrentGuiStyle(void);

    private:
        std::string Id;
        DProperties TempStyle;
};
#endif