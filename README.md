# CoveredReflector （映窗）

[![build](https://img.shields.io/badge/Build-pass-green.svg)](https://github.com/elecreak/CoveredReflector/actions?query=workflow%3AMSBuild+) [![ver](https://img.shields.io/badge/Version-v0.1.0(beta.1)-blue.svg)](https://github.com/elecreak/CoveredReflector/releases) 

CoveredReflector, a application that can display part of a covered window in the top floating window, and even can manipulate that covered window in this floating window. 

映窗，一个可以将被遮挡窗口部分画面显示在顶部浮动窗口，甚至可以在浮动窗口操控被遮挡窗口的小工具程序。

[![examplefunc_gif](https://github.com/elecreak/CoveredReflector/blob/main/img/showfunc_readme.gif)](https://github.com/elecreak/CoveredReflector/blob/main/img/showfunc_readme.gif)


## Highlights

* display covered window on top float window;
* ctrl_down when close float window to goto the target window;
* modify hotkey;

## Install

Download [release](https://github.com/elecreak/CoveredReflector/releases) and run the setup app on your Windows system.

## Basic Usage

* Main Func: After starting the software, use the default hotkey "Ctrl+Alt+D" at any time and then use the mouse to select an area of a window to see the selected part of the window in the resulting floating window, even if the window is obscured by other windows.
* Floating window usage and closure: After the floating window is generated, hold down the left mouse button inside the floating window to drag it, and double-click the left mouse button to close the floating window. And if you hold down the Ctrl key and double-click the left button to close the floating window, the software will place the target window of the captured floating window at the top.
* Change hotkey: Right click on the application icon in the tray and select "Set Hotkey" from the pop-up menu. Hold down the new hotkey in the settings window and click the save button to close the settings window and permanently change the hotkey.
* I'm so sorry that the function of manipulating the target window through floating windows has not been implemented yet.

## Advanced Usage

* Because the current version of the floating window requires GDI rendering for capturing the target window of the rendered image, for target windows rendered using default methods such as DirectX and OpenGL, the floating window in the current version will display all black when working. For this situation, you can add restrictions such as "-- disable gpu -- disable direct composition" when launching the corresponding program to restrict the application window from using GDI rendering, or add restrictions such as the above command after the target value of the corresponding program shortcut property. However, please note that this will affect the performance of the target window.

## Bugs/Requests

Please send bug reports and feature requests through [github issue tracker](https://github.com/elecreak/CoveredReflector/issues). CoveredReflector is currently under development now and it's open to any constructive suggestions.

## License

Distributed under the terms of the  [Apache 2.0 license](https://github.com/elecreak/CoveredReflector/blob/main/LICENSE).
