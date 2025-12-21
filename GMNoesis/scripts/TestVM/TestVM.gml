
function PlayerScoreViewModel() : GMNoesisVM("PlayerScoreViewModel", {
	high_score: GMNoesisVMType.number,
	current_score: GMNoesisVMType.number
}) constructor {}
	

function PlayerViewModel() : GMNoesisVM("PlayerViewModel", {
	score: {PlayerScoreViewModel},
	scores: [{PlayerScoreViewModel}],
	name: GMNoesisVMType.string,
	favorite_colors: [GMNoesisVMType.string],
	commands: {
		main_button_click: GMNoesisVMType.string
	},
	events: {
		play_reward_animation: GMNoesisVMType.number	
	}
}) constructor {
	handle_main_button_click = function(_string) {
		show_message($"{name} says {_string}");	
	}
}