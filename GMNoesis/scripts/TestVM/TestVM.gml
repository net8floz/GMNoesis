function PlayerViewModel() : GMNoesisVM("PlayerViewModel", {
	name: GMNoesisVMType.string,
	score: GMNoesisVMType.number,
	commands: {
		main_button_click: GMNoesisVMType.string
	}
}) constructor {
	handle_main_button_click = function(_string) {
		show_message($"{name} says {_string}");	
	}
}