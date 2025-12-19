vm = new PlayerViewModel(); 

vm.set_name("net8floz");
vm.set_favorite_colors(["red", "blue", "green"]);

vm.set_score(new PlayerScoreViewModel());
vm.score.set_current_score(69);


var _score1 = new PlayerScoreViewModel();
_score1.set_current_score(100);
_score1.set_high_score(200);

var _score2 = new PlayerScoreViewModel();
_score2.set_current_score(100);
_score2.set_high_score(200);

vm.set_scores([_score1, _score2]);


noesis_load_view("MainPage.xaml");
noesis_set_view_vm(vm.handle);

