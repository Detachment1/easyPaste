# Cross-platform Windows/MacOS easyPaste
This project is used to reformat the clipboard text. It is useful when you want to copy a chunck of text in a pdf and paste it in the google translate. It is running in the background and remove the redundent new lines and white spaces in the clipboard automatically.
## How to use
* download the ```.tar.gz``` file in the [release](https://github.com/Detachment1/easyPaste/releases) page
* unzip the ```.tar.gz``` file
* run the ```easyPaste``` (mac) or ```easyPaste.exe``` (windows) under the bin directory
>note that for the mac system, do not double click the easyPaste file. you should run in the terminal using ```./easyPaste```
## Acknowledgements
The ui part of this project uses the [tray](https://github.com/dmikushin/tray) project. It is a Cross-platform, super tiny C99 implementation of a system tray icon with a popup menu. 
## Updates
v1.1.0
* support two types of shortcuts to control the behavior of copy (can be switched by clicking the first item of the popup menu)
  1. ```ctrl + c```(windows), ```command + c```(mac) remove new lines while ```ctrl + alt + c```(windows), ```ctrl + command + c```(mac) do normal copy.  
  2. ```ctrl + alt + c```(windows), ```ctrl + command + c```(mac) remove new lines and ```ctrl + c```(windows), ```command + c```(mac) do normal copy.