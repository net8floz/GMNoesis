
function PlayerScoreViewModel() : GMNoesisVM("PlayerScoreViewModel", {
	high_score: GMNoesisVMType.number,
	current_score: GMNoesisVMType.number
}) constructor {}
	

function PlayerViewModel() : GMNoesisVM("PlayerViewModel", {
	//favorite_colors: [GMNoesisVMType.string],
	score: {PlayerScoreViewModel},
	name: GMNoesisVMType.string,
	//score: GMNoesisVMType.number,
	//favorite_colors: [GMNoesisVMType.string],
	//commands: {
	//	main_button_click: GMNoesisVMType.string
	//}
}) constructor {
	//handle_main_button_click = function(_string) {
	//	show_message($"{name} says {_string}");	
	//}
}