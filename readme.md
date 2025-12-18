# GMNoesis 
NoesisGUI is a really sophisticated UI that is rendered on top of your game. It has a ton of features and fair indie plans. [https://www.noesisengine.com/noesisgui/](https://www.noesisengine.com/noesisgui/)

GMNoesis, pronounce Gee'm'no'yee'sis', is a GameMaker extension. Currently only supports windows but you can change that by making a PR.

# Whats the state right now? 
Noesis itself is fully functional on top of the game maker window and interactable with input. View models can be created in GML and bound to the main view. You can send commands through view models through VMs from Noesis -> GML.

```
// define a view model

function PlayerViewModel() : GMNoesisVM("PlayerViewModel", {
  // define what types you want this should match the VM you create in App Studio
	name: GMNoesisVMType.string,
	score: GMNoesisVMType.number,
	commands: {
  // define any commands you want, this should match the VM you create in App STudio
		main_button_click: GMNoesisVMType.string
	}
}) constructor {

  // command callbacks will execute on functions that begin with "handle_" followed by the command you predefined
	handle_main_button_click = function(_string) {
		show_message($"{name} says {_string}");	
	}
}

// load a view and bind a view model

// note that GMNoesis doesnt need any setup or initialize function just load the view and set the VM
noesis_load_view("MainPage.xaml");

vm = new PlayerViewModel(); 

vm.set_name("Player One!");
vm.set_score(69);

noesis_set_view_vm(vm.handle);

```

# Whats the quickest way i can play around with this
1. clone the project
2. download noesis studio [https://www.noesisengine.com/studio/](https://www.noesisengine.com/studio/)
3. open the noesis project in GMNoesis/datafiles/Screens
4. edit the ui
5. run the gamemaker project and enjoy the ui you made

## How to work on the DLL 

1. Install CMake
2. Install Visual Studio Community 2022 and c++ stuff 
3. Download the Noesis native SDK 3.2.10 [https://www.noesisengine.com/forums/download/file.php?id=788](https://www.noesisengine.com/forums/download/file.php?id=788) and extract it to GMNoesisDll/ThirdParty. This should result in the file GMNoesisDll/ThirdParty/noesis/Documentation.html to exist. 
4. Open GMNoesisDll/ThirdParty/noesis/Build and open the windows x86 x64 solution in Visual Studio or Rider and build the solution. This will result in GMNoesisDll/ThirdParty/noesis/bin/windows_x86_64/NoesisApp.dll to exist.
5. Run the bat file GMNoesisDll/generate_sln.bat. This will result in GMNoesisDll/build/GMNoesis.sln to exist.
6. Open the GMNoesis solution in Rider and edit it. When you build all the relevant DLLs will be moved into the GM project extension folder and ready for testing.

