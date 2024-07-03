# raywui is a C++ library for writing widgets based gui

Based on top of Raygui and Raylib, this project aim to create an eco-system of widget classes.

Can be used in 2 way:

- Use DGuiApp event based login (preferred).
- Instantiate a widget class and use Draw() method when you want to update it on screen but you need to handle app logic in classic raylib way.

Features:

* [x] DGuiApp to control **event base** and **callback** logic.
* [x] Each widget has its own class with properties stored inside.

Current widgets:

* [x] DGuiContainer
* [x] DGuiButton
* [x] DGuiEdit
* [x] DGuiLabel
* [x] DGuiStatusBar
* [x] DGuiImage
* [ ] DGuiCheckBox

## **WARNING: this lib it is under development: no alpha release yet**
