#include "game_board.h"
#include <Windows.h>

int main(){
    SetConsoleOutputCP(CP_UTF8);
    Game_Board board = {0};
    board.current_turn = 1;
    set_board(&board.red_men, &board.black_men);
    print_screen(&board);
    getchar();
    
}