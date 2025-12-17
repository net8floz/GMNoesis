# GMNoesis 
NoesisGUI is a really sophisticated UI that is rendered on top of your game. It has a ton of features and fair indie plans. [https://www.noesisengine.com/noesisgui/](https://www.noesisengine.com/noesisgui/)

GMNoesis, pronounce Gee'm'no'yee'sis', is a GameMaker extension. Currently only supports windows but you can change that by making a PR.

# Whats the state right now? 
It will load all your fonts and images and textures and display them on top of your gamemaker game. you can use mouse and keyboard and gamepad. the Studio's Interaction events seem to be broken. 

## How to work on the DLL 

1. Install CMake
2. Install Visual Studio Community 2022 and c++ stuff 
3. Download the Noesis native SDK 3.2.10 [https://www.noesisengine.com/forums/download/file.php?id=788](https://www.noesisengine.com/forums/download/file.php?id=788) and extract it to GMNoesisDll/ThirdParty. This should result in the file GMNoesisDll/ThirdParty/noesis/Documentation.html to exist. 
4. Open GMNoesisDll/ThirdParty/noesis/Build and open the windows x86 x64 solution in Visual Studio or Rider and build the solution. This will result in GMNoesisDll/ThirdParty/noesis/bin/windows_x86_64/NoesisApp.dll to exist.
5. Run the bat file GMNoesisDll/generate_sln.bat. This will result in GMNoesisDll/build/GMNoesis.sln to exist.
6. Open the GMNoesis solution in Rider and edit it. When you build all the relevant DLLs will be moved into the GM project extension folder and ready for testing.

