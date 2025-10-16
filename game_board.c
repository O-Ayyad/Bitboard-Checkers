#include "game_board.h"

const uint32_t mask = 1u;

void set_board( uint32_t* red_m, uint32_t* black_m){ //Sets the inital board in a standard checkers game
    *red_m |= 0b111111111111;
    *black_m = 0b111111111111<<20;
}

void set_tile_to_piece(uint32_t* board, int position){ //Sets the tile of a board to a peice
    *board |= (mask << position);
}

void remove_peice_from_tile(uint32_t* board, int position){ //remove a peice from a tile
    *board &= ~(mask << position);
}

int is_peice_at(uint32_t board, int position){ //Returns 1 if a peice exists on that baord, 0 otherwise.
    return ((board)>>position) & 1;
}
void turn(Game_Board* board){
    unsigned char to,from;
    while(1){
        
    }
}