vector<int> __card_deck__;

vector<int> __player_1_deck__;
vector<int> __player_2_deck__;

vector<int> __card_buffer__;

TRandom3* __PRNG__;

int __nb_plays__ = 10000;
int __max_loops__ = 10000;
TH1D* __loop_count_hist__;
int __loop_counter__;

void do_play();
void init_card_deck();
void give_cards();
void do_match();
bool is_perfect_permute();
bool did_anyone_lose();

//debug
void print_player(vector<int> &player_deck_);
void display_loading(int current_index_, int end_index_, string title_ = "Processing", bool force_display_ = false);
string ERROR    = "\033[1;31m[ERROR] \033[00m";
string INFO  = "\033[1;32m[INFO] \033[00m";
string WARNING   = "\033[1;33m[WARNING] \033[00m";
string ALERT = "\033[1;35m[ALERT] \033[00m";
int verbosity_level = 0;
double __last_displayed_value__ = -1;


void bataille(){

  __PRNG__ = new TRandom3(time(NULL));
  __loop_count_hist__ = new TH1D("__loop_count_hist__", "__loop_count_hist__", 200, 0, __max_loops__+1);
  __loop_count_hist__->GetXaxis()->SetTitle("Nombre de retournements de cartes");
  __loop_count_hist__->GetYaxis()->SetTitle("Nombre parties");

  int unfinished_count = 0;
  for(int i_play = 0 ; i_play < __nb_plays__ ; i_play++){
    display_loading(i_play, __nb_plays__);
    do_play();
    // cout << "__loop_counter__ = " << __loop_counter__ << endl;
    __loop_count_hist__->Fill(__loop_counter__);
    if(__loop_counter__ == __max_loops__){
      unfinished_count++;
      // cout << "UNFINISHED :" << endl;
      // cout << "p1 = "; print_player(__player_1_deck__);
      // cout << "p2 = "; print_player(__player_2_deck__);
      // cout << "************" << endl;
    }
  }

  __loop_count_hist__->Draw();
  gStyle->SetOptStat(0);
  gPad->SetLogy();

  cout << "Unfinished fraction : " << ((double) unfinished_count)/__nb_plays__*100. << endl;

}


void do_play(){

  init_card_deck();
  give_cards();
  // cout << "__player_1_deck__.size() = " << __player_1_deck__.size() << endl;
  // cout << "__player_2_deck__.size() = " << __player_2_deck__.size() << endl;
  // cout << "p1 = "; print_player(__player_1_deck__);
  // cout << "p2 = "; print_player(__player_2_deck__);
  do_match();

}
void init_card_deck(){

  __card_deck__.clear();

  for(int i_familly = 0 ; i_familly < 4 ; i_familly++){
    for(int i_card = 0 ; i_card < 13 ; i_card++){
      __card_deck__.emplace_back(i_card);
    }
  }

}
void give_cards(){

  __player_1_deck__.clear();
  __player_2_deck__.clear();
  __card_buffer__.clear();

  int chosen_card = -1;
  while(__card_deck__.size() != 0){
    chosen_card = int(__PRNG__->Rndm()*__card_deck__.size()); // int() = floor
    if(__card_deck__.size() % 2 == 0){
      __player_1_deck__.emplace_back(__card_deck__[chosen_card]);
    } else {
      __player_2_deck__.emplace_back(__card_deck__[chosen_card]);
    }
    __card_deck__.erase(__card_deck__.begin()+chosen_card);
  }

}
void do_match(){

  if(is_perfect_permute()){
    __loop_counter__ = -1;
    cout << "Perfect :" << endl;
    cout << "p1 = "; print_player(__player_1_deck__);
    cout << "p2 = "; print_player(__player_2_deck__);
    cout << "************" << endl;
    return;
  }

  __loop_counter__ = 0;
  while(not did_anyone_lose()){

    __loop_counter__++;
    if(__loop_counter__ >= __max_loops__) break;

    if(__player_1_deck__[0] > __player_2_deck__[0]){ // p1 wins
      __player_1_deck__.emplace_back(__player_1_deck__[0]);
      __player_1_deck__.emplace_back(__player_2_deck__[0]);
      while(__card_buffer__.size() != 0){
        __player_1_deck__.emplace_back(__card_buffer__[0]);
        __card_buffer__.erase(__card_buffer__.begin());
      }
      __player_1_deck__.erase(__player_1_deck__.begin());
      __player_2_deck__.erase(__player_2_deck__.begin());
    } else if(__player_2_deck__[0] > __player_1_deck__[0]) { // p2 wins
      __player_2_deck__.emplace_back(__player_1_deck__[0]);
      __player_2_deck__.emplace_back(__player_2_deck__[0]);
      while(__card_buffer__.size() != 0){
        __player_2_deck__.emplace_back(__card_buffer__[0]);
        __card_buffer__.erase(__card_buffer__.begin());
      }
      __player_1_deck__.erase(__player_1_deck__.begin());
      __player_2_deck__.erase(__player_2_deck__.begin());
    } else { //bataille !
      // put same card into buffer
      __card_buffer__.emplace_back(__player_1_deck__[0]);
      __card_buffer__.emplace_back(__player_2_deck__[0]);
      __player_1_deck__.erase(__player_1_deck__.begin());
      __player_2_deck__.erase(__player_2_deck__.begin());
      if(did_anyone_lose()){ // anyone lost ?
        break;
      }
      // put extra card into buffer
      __card_buffer__.emplace_back(__player_1_deck__[0]);
      __card_buffer__.emplace_back(__player_2_deck__[0]);
      __player_1_deck__.erase(__player_1_deck__.begin());
      __player_2_deck__.erase(__player_2_deck__.begin());
      // then replay
    }
  }

}

bool is_perfect_permute(){
  int last_win = -1;
  for(int i_card = 0 ; i_card < __player_1_deck__.size(); i_card++){
    if(__player_1_deck__[i_card] > __player_2_deck__[i_card]){
      if(last_win == 1) return false;
      last_win = 1;
    } else if(__player_2_deck__[i_card] > __player_1_deck__[i_card]) {
      if(last_win == 2) return false;
      last_win = 2;
    } else{
      i_card++; // skip next
    }
  }
  return true;
}
bool did_anyone_lose(){
  return (__player_1_deck__.size() == 0 or __player_2_deck__.size() == 0);
}

//debug
void print_player(vector<int> &player_deck_){
  cout << "{ ";
  for(auto const &card: player_deck_){
    cout << card << ", ";
  }
  cout << " }" << endl;
}

void display_loading(int current_index_, int end_index_, string title_, bool force_display_) {

  int percent = int(round(double(current_index_) / end_index_ * 100.));
  if(force_display_ or current_index_ >= end_index_-1) {
    if(__last_displayed_value__ != -1) clog << "\r" << title_ << " : " << 100 << "%" << endl;
    __last_displayed_value__ = -1;
    return;
  }
  if(__last_displayed_value__ == -1 or __last_displayed_value__ < percent) {
    __last_displayed_value__ = percent;
    clog << "\r" << title_ << " : " << percent << "%" << flush << "\r";
  }

}
