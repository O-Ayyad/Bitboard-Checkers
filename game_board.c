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
/*
  1= Red Wins
  2= Black wins
  0= No win yet
*/
int check_win(Game_Board* board){
    //If black has no peices
    if(!board->black_kings && !board->black_men){
        return 1; //red wins
    }
    //if red has no peices
    if (board->red_men == 0 && board->red_kings == 0) {
        return 2; //Black wins
    }

    // Game not finished
    return 0;
}

/* Checks for all valid moves for a peice and returns an int of the tiles that a peice can go on and for double/triple hops
    Returns an array where index
    0 = top right, 
    1 = top left, 
    2 = bottom right, 
    3 = bottom left

    ALWAYS FREE THE RETURN VALUE AFTER CALLING
*/
int* check_for_moves(Game_Board* board, uint32_t pieces, int position){
    int row = position / 4;
    int col = position % 4;
    int moves_found = 0;
    uint32_t selected_piece = (pieces >> position);

    if(!(selected_piece & mask)){ //There is no peice at that position
        return NULL;
    }

    int* moves = malloc(4 * sizeof(int));
    for (int i = 0; i < 4; i++) {
        moves[i] = -1;
    }

    uint32_t occupied_tiles = board->red_kings | board->black_kings | board->red_men | board->black_men;

    int top_left_offset, top_right_offset, bottom_left_offset, bottom_right_offset;

    if (row % 2 == 0) { //even row
        top_left_offset = 3;
        top_right_offset = 4;
        bottom_left_offset = -5;
        bottom_right_offset = -4;
    } else { //odd row
        top_left_offset = 4;
        top_right_offset = 5;
        bottom_left_offset = -4;
        bottom_right_offset = -3;
    }

    // Top right
    if (row < 7) { //not top row
        int tr_pos = position + top_right_offset;
        if (tr_pos >= 0 && tr_pos < 32 && !(occupied_tiles & (1u << tr_pos))) {
            moves[0] = tr_pos;
            moves_found++;
        }
    }

    // Top left
    if (row < 7) { // not top row
        int tl_pos = position + top_left_offset;
        if (tl_pos >= 0 && tl_pos < 32 && !(occupied_tiles & (1u << tl_pos))) {
            moves[1] = tl_pos;
            moves_found++;
        }
    }

    // Bottom right
    if (row > 0) { // not bottom row
        int br_pos = position + bottom_right_offset;
        if (br_pos >= 0 && br_pos < 32 && !(occupied_tiles & (1u << br_pos))) {
            moves[2] = br_pos;
            moves_found++;
        }
    }

    // Bottom left
    if (row > 0) { //not bottom row
        int bl_pos = position + bottom_left_offset;
        if (bl_pos >= 0 && bl_pos < 32 && !(occupied_tiles & (1u << bl_pos))) {
            moves[3] = bl_pos;
            moves_found++;
        }
    }

    if (!moves_found) { // no valid moves
        free(moves);
        return NULL;
    }

    return moves;
}
//Checks for any valid moves for a full board and returns:
// 1 = there is a valid move
// 0 = there is none so game over
int any_vaild_moves(uint32_t peices){
    peices >= 3;
    for(int i; i<31;i++){
        
    }
}