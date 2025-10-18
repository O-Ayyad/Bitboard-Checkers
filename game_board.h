#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    //Each distince piece lives on a 32 bit board
    //Since white piece cannot hold a piece, there is no need to double the size of the struct in mem to hold empty tiles
    //Bottom left is post 0 and top right is pos 31
    /*  
        [ ] [28] [ ] [29] [ ] [30] [ ] [31]

        [24] [ ] [25] [ ] [26] [ ] [27] [ ]

        [ ] [20] [ ] [21] [ ] [22] [ ] [23]

        [16] [ ] [17] [ ] [18] [ ] [19] [ ]

        [ ] [12] [ ] [13] [ ] [14] [ ] [15]

        [8]  [ ] [9]  [ ] [10] [ ] [11] [ ]

        [ ] [4]  [ ] [5]  [ ] [6]  [ ]  [7]

        [0]  [ ] [1]  [ ] [2]  [ ] [3]  [ ]
    */ 
    uint32_t red_men;
    uint32_t red_kings;
    uint32_t black_men;
    uint32_t black_kings;
    uint16_t current_turn;
} Game_Board;

extern const uint32_t mask;


//game_board.c
void set_new_board(uint32_t* red_m, uint32_t* black_m);
void set_tile_to_piece(uint32_t* board, int position);
void remove_piece_from_tile(uint32_t* board, int position);
int is_piece_at(uint32_t board, int position);

void turn(Game_Board* board);


//utils.c
void clear_screen();

void print_screen(Game_Board* board);
void print_bitboards(Game_Board* board);
void print_binary(uint32_t value);
void print_line(int width);
void print_player_turn(Game_Board* board);
void print_board(Game_Board* board);
char get_piece(Game_Board* board, int bit);

int vailidate_user_input(Game_Board* board, char* input);
int get_user_input(Game_Board* board);



#endif