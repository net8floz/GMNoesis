
noesis_load_view("MainPage.xaml");

vm = new PlayerViewModel(); 



vm.set_name("net8floz");
vm.set_score(new PlayerScoreViewModel());
vm.score.set_current_score(69);

noesis_set_view_vm(vm.handle);

