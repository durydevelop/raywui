                                                                   
                                                                 
  Base concept
  ┌──────────DCanvas───────────┐ DCanvas is main canvas of DApp  
  │                            │ has 2 list of widgets           
  │┌───static───┐┌──dynamic───┐│ static widgets:                 
  ││  widgets   ││  widgets   ││ widgets that are always drawn.  
  ││            ││            ││ dynamic widgets:                
  ││            ││            ││ widgets that can be draws only  
  ││            ││            ││ one per time.                   
  ││   always   ││   drawn    ││                                 
  ││ drawn all  ││  one of    ││                                 
  ││            ││            ││                                 
  ││            ││            ││                                 
  │└────────────┘└────────────┘│                                 
  └────────────────────────────┘                                 

Widgets class heiracry
                                                     
 ┌───────────────────────────────────────────┐       
 │         DGuiWidget (abstarct class)       │       
 └───────────────────┬──┬────────────────────┘       
                     │  │                            
                     │  │                            
 ┌──DGuiContainer──┐ │  │  ┌────────────┐            
 │ ┌────────────┐  │◄┘  ├─►│ DGuiButton │            
 │ │ DGuiButton │  │    │  └────────────┘            
 │ └────────────┘  │    │  ┌────────────┐            
 │ ┌────────────┐  │    └─►│ DGuilabel  │            
 │ │ DGuilabel  │  │       └────────────┘            
 │ └────────────┘  │            ...                  
 └─────────────────┘                                 
  DGuiContainer can contains more widgets

Example:
                ┌──────────────────────────────────────────────────┐             
                │                                                  │             
                │                                                  │             
                │ ┌DGuiContainer─┐ ┌─────────DGuiContainer────────┐│             
Static──────────┼─┼►             │ │                              ││             
                │ │┌───────────┐ │ │     Buttons                  ││             
                │ ││DButton    │ │ │                              ││             
                │ │└───────────┘ │ │     Labels                   ││             
                │ │┌───────────┐ │ │                             ◄┼┼───── Dynamic
                │ ││DButton    │ │ │     ....                     ││             
                │ │└───────────┘ │ │                              ││             
                │ │              │ │                              ││             
                │ │              │ │                              ││             
                │ │              │ │                              ││             
                │ └──────────────┘ └──────────────────────────────┘│             
                ┌─────────────────────────────────────────────────┐│             
Static──────────┼►                 DStatusBar                     ││             
                └─────────────────────────────────────────────────┘┘             