#include "game_board.h"

int main(){
    Game_Board board = {0};
    board.current_turn = 1;
    set_new_board(&board.red_men, &board.black_men);
    turn(&board);
    getchar(); 
}