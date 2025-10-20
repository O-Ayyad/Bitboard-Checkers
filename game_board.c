#include "game_board.h"

const uint32_t mask = 1u;

//Sets the inital board in a standard checkers game
void set_new_board( uint32_t* red_m, uint32_t* black_m){
    for(int i = 0; i < 12; i++){
        *red_m |= (mask<<i);
        *black_m |= (mask<<(31-i));
    }
}

//Set bit
//Sets the bit of a board to a piece
void set_tile_to_piece(uint32_t* board, int position){ 
    if (position < 0 || position >= 32) return;
    *board |= (mask << position);
}

//Clear bit
/// Removes bit from board
void remove_piece_from_tile(uint32_t* board, int position){ //remove a piece from a tile
    if (position < 0 || position >= 32) return;
    *board &= ~(mask << position);
}

//Unused: toggles a bit from 0->1 or 1->0
void toggle_bit(uint32_t* board, int position){
    if (position < 0 || position >= 32) return;
    *board ^= (mask<<position);
}
//Returns 1 if a piece exists on that board, 0 otherwise.
int is_piece_at(uint32_t board, int position){
    if (position < 0 || position >= 32) return 0;
    return ((board)) & (mask<<position);
}

void move_piece(uint32_t* board, int start, int finish){
    remove_piece_from_tile(board, start);
    set_tile_to_piece(board, finish);
}

//1 for red 2 for black
void take_piece(Game_Board* board, int start, int finish, int player_turn) {
    // player_turn: 1 = red, 2 = black
    uint32_t* player_men;
    uint32_t* player_kings;
    uint32_t* opponent_men;
    uint32_t* opponent_kings;

    if (player_turn %2 == 1) {
        player_men = &board->red_men;
        player_kings = &board->red_kings;
        opponent_men = &board->black_men;
        opponent_kings = &board->black_kings;
    } else {
        player_men = &board->black_men;
        player_kings = &board->black_kings;
        opponent_men = &board->red_men;
        opponent_kings = &board->red_kings;
    }
    int is_king = ((*player_kings) & (mask << start)) != 0;

    //Remove attacker
    remove_piece_from_tile(player_men, start);
    remove_piece_from_tile(player_kings, start);

    //Find victim

    int start_row = start / 4;
    int start_row_parity = start_row % 2;
    
    int offset = finish - start;
    int victim_position = -1;
    if (start_row_parity == 0) { // Even row
        // top_left=3, top_right=4, bottom_left=-5, bottom_right=-4
        if (offset == 7) {
            victim_position = start + 3;
        } else if (offset == 9) {
            victim_position = start + 4;
        } else if (offset == -9) {
            victim_position = start - 5;
        } else if (offset == -7) {
            victim_position = start - 4;
        }
    } else { // Odd row
        if (offset == 7) {
            victim_position = start + 4;
        } else if (offset == 9) { 
            victim_position = start + 5;
        } else if (offset == -9) {
            victim_position = start - 4;
        } else if (offset == -7) {
            victim_position = start - 3;
        }
    }

    if (victim_position >= 0 && victim_position < 32) {
        remove_piece_from_tile(opponent_men, victim_position);
        remove_piece_from_tile(opponent_kings, victim_position);
    }

    if (is_king) {
        set_tile_to_piece(player_kings, finish);
    } else {
        set_tile_to_piece(player_men, finish);
    }
}

/*0 = empty, 1 = red man, 2 = red king, 3 = black man, 4 = black king*/
int get_piece_type(Game_Board* board, int position){
    uint32_t bit = mask << position;

    if (board->red_men & bit) return 1;
    else if (board->red_kings & bit) return  2;
    else if (board->black_men & bit) return  3;
    else if (board->black_kings & bit) return  4;
    else return 0;
}

/*  Checks for all valid moves for a piece and returns an int of the tile index that a piece can go
    including forced takes and such
    
    If a forced take is possible then all other moves are removed and only the forced takes returned

    Call this function again when piece was taken (returned_pointer[40] = 1)
    When checking a move for a hypotheical piece, pass in for param_piece_type
    0 = empty (returns empty list), 1 = red man, 2 = red king, 3 = black man, 4 = black king

    Returns an array where index

    //Forced takes
    //Regular moves
    0 = top right
    1 = top left
    2 = bottom right
    3 = bottom left

    //Forced takes
    4 = top right top right,  
    5 = top left top left
    6 = bottom right bottom right,   
    7 = bottom left bottom left

    //King moves
    8-15 = Top right diagonals
    16-23 = Top left diagonls
    24-31 = Bottom right diagonals
    32-39 = Bottom left diagonals

    40 = 1 or 0 where 1 means there was a forced take and 0 means it was a normal move.

    ALWAYS FREE THE RETURN VALUE AFTER CALLING
*/
int* check_for_moves(Game_Board* board, int position, int param_piece_type){

    int array_size = 41;
    int forced_move = 40;
    /* Moves
    //Regular moves
    0 = top right
    1 = top left
    2 = bottom right
    3 = bottom left

    //Forced takes
    4 = top right top right,  
    5 = top left top left
    6 = bottom right bottom right,   
    7 = bottom left bottom left

    //King moves
    8-15 = Top right diagonals
    16-23 = Top left diagonls
    24-31 = Bottom right diagonals
    32-39 = Bottom left diagonals

    40 = 1 or 0 where 1 means there was a forced take and 0 means it was a normal move.
    */
    int* moves = malloc((array_size) * sizeof(int));
    for (int i = 0; i < array_size; i++) {
        moves[i] = -1;
    }
    moves[forced_move]=0;

    uint32_t occupied_tiles_by_red = board->red_kings | board->red_men ;
    uint32_t occupied_tiles_by_black = board->black_men | board->black_kings;
    uint32_t occupied_tiles = occupied_tiles_by_red | occupied_tiles_by_black;

    int piece_type = 0; // 0 = empty, 1 = red man, 2 = red king, 3 = black man, 4 = black king
    if(param_piece_type == 0){
        piece_type = get_piece_type(board, position);
    }else{
        piece_type = param_piece_type;
    }

    int top_left_offset,
        top_right_offset,
        bottom_left_offset,
        bottom_right_offset;

    int row = position / 4;

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
    switch(piece_type){



        case 1:{ //Red man
            //Adjacent tiles
            int tr = position + top_right_offset;
            int tr_row = tr/4;
            int tl = position + top_left_offset;
            int tl_row = tl/4;

            //Top right next (for takes)
            int tr_next;
            //Swap parity for next row
            if(top_right_offset == 4){
                tr_next = tr + (top_right_offset + 1);
            } else{ //T_R_offset = 5
                tr_next = tr + (top_right_offset -1);
            }
            int tr_next_row = tr_next/4;

            //Top left next
            int tl_next;
            //Swap parity for next row
            if(top_left_offset == 3){
                tl_next = tl + (top_left_offset + 1);
            } else{ //T_L_offset = 4
                tl_next = tl + (top_left_offset -1);
            }
            int tl_next_row = tl_next/4;
            
            //Make sure the are on a row different by one
            //Check for adjacent free tiles
            if (abs(tr_row - row)== 1  && tr >= 0 && tr < 32 && !(occupied_tiles & (mask << tr)) && moves[forced_move] == 0){
                moves[0] = tr;
            }
            if (abs(tr_row - row) == 1 && tr >= 0 && tr < 32 && (occupied_tiles_by_black & (mask << tr))){ //Check if the top right top right tile is occupied by black               
                //If this is true, then check the tile after the black tile and if it is empty then add that tile

                if (abs(tr_next_row - tr_row) == 1 && tr_next >= 0 && tr_next < 32 && !(occupied_tiles & (mask << tr_next))){
                    moves[4] = tr_next;
                    moves[forced_move] = 1;
                }
            }

            //Check top left
            if (abs(tl_row - row) == 1 && tl >= 0 && tl < 32 && !(occupied_tiles & (mask << tl)) && moves[forced_move]==0){
               moves[1] = tl; 
            } 
            if (abs(tl_row - row) == 1 && tl >= 0 && tl < 32 && (occupied_tiles_by_black & (mask << tl))){ //Check if the top left top left tile is occupied by black               
                //If this is true, then check the tile after the black tile and if it is empty then add that tile
                if (tl_next_row - tl_row  == 1 && tl_next >= 0 && tl_next < 32 && !(occupied_tiles & (mask << tl_next))){
                    moves[5] = tl_next;
                    moves[forced_move] = 1;
                }
            }
            break;
        }


        case 2:{ //Red king

            int tr = position + top_right_offset;
            int tl = position + top_left_offset;
            int br = position + bottom_right_offset;
            int bl = position + bottom_left_offset;

            int bl_row = bl/4;
            int tr_row = tr/4;
            int tl_row = tl/4;            
            int br_row = br/4;

            int tr_occ = (occupied_tiles_by_black & (mask << tr));
            int tl_occ = (occupied_tiles_by_black & (mask << tl));
            int br_occ = (occupied_tiles_by_black & (mask << br));
            int bl_occ = (occupied_tiles_by_black & (mask << bl));

            //Check for jumps first
            if ((tr_occ && abs(tr_row - row) == 1 && tr >= 0 && tr < 32) ||
                (tl_occ && abs(tl_row - row) == 1 && tl >= 0 && tl < 32) ||
                (br_occ && abs(br_row - row) == 1 && br >= 0 && br < 32) ||
                (bl_occ && abs(bl_row - row) == 1 && bl >= 0 && bl < 32)) {

                
                //Top right occupied
                if(tr_occ && abs(tr_row - row) == 1 && tr >= 0 && tr < 32){
                    int tr_next;
                    if(top_right_offset == 4){
                        tr_next = tr + (top_right_offset + 1);
                    } else{
                        tr_next = tr + (top_right_offset -1);
                    }
                    int tr_next_row = tr_next/4;
                    if (abs(tr_next_row - tr_row) == 1 && tr_next >= 0 && tr_next < 32 && !(occupied_tiles & (mask << tr_next))){
                        moves[4] = tr_next;
                        moves[forced_move] = 1;
                    }
                }
                //Top left 
                if(tl_occ && abs(tl_row - row) == 1 && tl >= 0 && tl < 32){
                    int tl_next;
                    if(top_left_offset == 3){
                        tl_next = tl + top_left_offset + 1;
                    } else{ //4
                        tl_next = tl + top_left_offset -1;
                    }
                    int tl_next_row = tl_next/4;
                    if (abs(tl_next_row - tl_row) == 1 && tl_next >= 0 && tl_next < 32 && !(occupied_tiles & (mask << tl_next))){
                        moves[5] = tl_next;
                        moves[forced_move] = 1;
                    }
                }

                //Bottom right
                if(br_occ){
                    int br_next;
                    if(bottom_right_offset == -4){
                        br_next = br + (bottom_right_offset + 1);
                    } else{ //-3
                        br_next = br + (bottom_right_offset -1);
                    }
                    int br_next_row = br_next/4;
                    if (abs(br_next_row - br_row) == 1 && br_next >= 0 && br_next < 32 && !(occupied_tiles & (mask << br_next))){
                        moves[6] = br_next;
                        moves[forced_move] = 1;
                    }
                }
                
                //Bottom left
                if(bl_occ){
                    int bl_next;
                    if(bottom_left_offset == -5){
                        bl_next = bl + (bottom_left_offset + 1);
                    } else{ //-4
                        bl_next = bl + (bottom_left_offset -1);
                    }
                    int bl_next_row = bl_next/4;
                    if (abs(bl_next_row - bl_row) == 1 && bl_next >= 0 && bl_next < 32 && !(occupied_tiles & (mask << bl_next))){
                        moves[7] = bl_next;
                        moves[forced_move] = 1;
                    }
                }
            }

            //Calculate Diagonals
            if(moves[forced_move]== 0){

                //Top right diagonals
                int tr_idx = 8;
                int tr_offset = top_right_offset;
                for(int tr_pos = tr, tr_r = tr_row, curr_row = row;
                    tr_pos >= 0 && tr_pos < 32 && !(occupied_tiles & (mask << tr_pos)) && abs(tr_r - curr_row) == 1; 
                    tr_pos += (tr_offset = (tr_offset == 4 ? tr_offset + 1 :tr_offset - 1)), tr_idx++, curr_row = tr_r, tr_r = tr_pos / 4)
                {
                    moves[tr_idx] = tr_pos;
                }


                //Top left
                int tl_idx = 16;
                int tl_offset = top_left_offset;
                for(int tl_pos = tl, tl_r = tl_row, curr_row = row; 
                    tl_pos >= 0 && tl_pos < 32 && !(occupied_tiles & (mask << tl_pos)) && abs(tl_r - curr_row) == 1; 
                    tl_pos += (tl_offset = (tl_offset == 4 ? tl_offset - 1 :tl_offset + 1)), tl_idx++,  curr_row = tl_r, tl_r = tl_pos / 4)
                {
                    moves[tl_idx] = tl_pos;
                }

                //Bottom right
                int br_idx = 24;
                int br_offset = bottom_right_offset;
                for(int br_pos = br, br_r = br_row, curr_row = row; 
                    br_pos >= 0 && br_pos < 32 && !(occupied_tiles & (mask << br_pos)) && abs(br_r - curr_row) == 1; 
                    br_pos += (br_offset = (br_offset == -4 ? br_offset + 1 : br_offset - 1)), br_idx++, curr_row= br_r, br_r = br_pos / 4)  
                {
                    moves[br_idx] = br_pos;
                }

                 
                //Bottom left
                int bl_idx = 32;
                int bl_offset = bottom_left_offset;
                for(int bl_pos = bl, bl_r = bl_row, curr_row = row;  
                    bl_pos >= 0 && bl_pos < 32 && !(occupied_tiles & (mask << bl_pos)) && abs(bl_r - curr_row) == 1; 
                    bl_pos += (bl_offset = (bl_offset == -4? bl_offset - 1 : bl_offset + 1)), bl_idx++, curr_row = bl_r, bl_r = bl_pos / 4)
                {
                    moves[bl_idx] = bl_pos;
                }
            }

            break;
        }

        case 3:{ //Black man
            //Adjacent tiles
            int br = position + bottom_right_offset;
            int br_row = br/4;
            int bl = position + bottom_left_offset;
            int bl_row = bl/4;

            //b right next (for takes)
            int br_next;
            //Swap parity for next row
            if(bottom_right_offset == -4){
                br_next = br + (bottom_right_offset +1);
            } else{ //B_R_offset = -3
                br_next = br + (bottom_right_offset - 1);
            }
            int br_next_row = br_next/4;

            //bottom left next
            int bl_next;
            //Swap parity for next row
            if(bottom_left_offset == -5){
                bl_next = bl + (bottom_left_offset + 1);
            } else{ //B_L_offset = -4
                bl_next = bl + (bottom_left_offset -1);
            }
            int bl_next_row = bl_next/4;
            
            //Make sure the are on a row different by one
            //Check for adjacent free tiles
            if (abs(br_row - row) == 1  && br >= 0 && br < 32 && !(occupied_tiles & (mask << br)) && moves[forced_move] == 0){
                moves[2] = br;
            }
            if (abs(br_row - row) == 1 && br >= 0 && br < 32 && (occupied_tiles_by_red & (mask << br))){ //Check if the bottom right bottom right tile is occupied by red               
                //If this is true, then check the tile after the red tile and if it is empty then add that tile

                if (abs(br_next_row - br_row)  == 1 && br_next >= 0 && br_next < 32 && !(occupied_tiles & (mask << br_next))){
                    moves[6] = br_next;
                    moves[forced_move] = 1;
                }
            }

            //Check bttom left
            if (abs(bl_row - row) == 1 && bl >= 0 && bl < 32 && !(occupied_tiles & (mask << bl)) && moves[forced_move]==0){
               moves[3] = bl; 
            } 
            if (abs(bl_row - row) == 1 && bl >= 0 && bl < 32 && (occupied_tiles_by_red & (mask << bl))){ //Check if the top left top left tile is occupied by red              
                //If this is true, then check the tile after the red tile and if it is empty then add that tile
                if (abs(bl_next_row - bl_row) == 1 && bl_next >= 0 && bl_next < 32 && !(occupied_tiles & (mask << bl_next))){
                    moves[7] = bl_next;
                    moves[forced_move] = 1;
                }
            }
            break;
        }




        case 4:{//Black king

            int tr = position + top_right_offset;
            int tl = position + top_left_offset;
            int br = position + bottom_right_offset;
            int bl = position + bottom_left_offset;

            int bl_row = bl/4;
            int tr_row = tr/4;
            int tl_row = tl/4;            
            int br_row = br/4;

            int tr_occ = (occupied_tiles_by_red & (mask << tr));
            int tl_occ = (occupied_tiles_by_red & (mask << tl));
            int br_occ = (occupied_tiles_by_red & (mask << br));
            int bl_occ = (occupied_tiles_by_red & (mask << bl));

            //Check for jumps first
            if ((tr_occ && abs(tr_row - row) == 1 && tr >= 0 && tr < 32) ||
                (tl_occ && abs(tl_row - row) == 1 && tl >= 0 && tl < 32) ||
                (br_occ && abs(br_row - row) == 1 && br >= 0 && br < 32) ||
                (bl_occ && abs(bl_row - row) == 1 && bl >= 0 && bl < 32)) {
                
                //Top right occupied
                if(tr_occ && abs(tr_row - row) == 1 && tr >= 0 && tr < 32){
                    int tr_next;
                    if(top_right_offset == 4){
                        tr_next = tr + top_right_offset + 1;
                    } else{
                        tr_next = tr + top_right_offset -1;
                    }
                    int tr_next_row = tr_next/4;
                    if (abs(tr_next_row - tr_row) == 1 && tr_next >= 0 && tr_next < 32 && !(occupied_tiles & (mask << tr_next))){
                        moves[4] = tr_next;
                        moves[forced_move] = 1;
                    }
                }
                //Top left 
                if(tl_occ && abs(tl_row - row) == 1 && tl >= 0 && tl < 32){
                    int tl_next;
                    if(top_left_offset == 3){
                        tl_next = tl + top_left_offset + 1;
                    } else{ //4
                        tl_next = tl + top_left_offset -1;
                    }
                    int tl_next_row = tl_next/4;
                    if (abs(tl_next_row - tl_row) == 1 && tl_next >= 0 && tl_next < 32 && !(occupied_tiles & (mask << tl_next))){
                        moves[5] = tl_next;
                        moves[forced_move] = 1;
                    }
                }

                //Bottom right
                if(br_occ){
                    int br_next;
                    if(bottom_right_offset == -4){
                        br_next = br + (bottom_right_offset + 1);
                    } else{ //-3
                        br_next = br + (bottom_right_offset -1);
                    }
                    int br_next_row = br_next/4;
                    if (abs(br_next_row - br_row) == 1 && br_next >= 0 && br_next < 32 && !(occupied_tiles & (mask << br_next))){
                        moves[6] = br_next;
                        moves[forced_move] = 1;
                    }
                }
                
                //Bottom left
                if(bl_occ){
                    int bl_next;
                    if(bottom_left_offset == -5){
                        bl_next = bl + (bottom_left_offset + 1);
                    } else{ //-4
                        bl_next = bl + (bottom_left_offset -1);
                    }
                    int bl_next_row = bl_next/4;
                    if (abs(bl_next_row - bl_row) == 1 && bl_next >= 0 && bl_next < 32 && !(occupied_tiles & (mask << bl_next))){
                        moves[7] = bl_next;
                        moves[forced_move] = 1;
                    }
                }
            }

            //Calculate Diagonals
                        if(moves[forced_move]== 0){

                //Top right diagonals
                int tr_idx = 8;
                int tr_offset = top_right_offset;
                for(int tr_pos = tr, tr_r = tr_row, curr_row = row;
                    tr_pos >= 0 && tr_pos < 32 && !(occupied_tiles & (mask << tr_pos)) && abs(tr_r - curr_row) == 1; 
                    tr_pos += (tr_offset = (tr_offset == 4 ? tr_offset + 1 :tr_offset - 1)), tr_idx++, curr_row = tr_r, tr_r = tr_pos / 4)
                {
                    moves[tr_idx] = tr_pos;
                }


                //Top left
                int tl_idx = 16;
                int tl_offset = top_left_offset;
                for(int tl_pos = tl, tl_r = tl_row, curr_row = row; 
                    tl_pos >= 0 && tl_pos < 32 && !(occupied_tiles & (mask << tl_pos)) && abs(tl_r - curr_row) == 1; 
                    tl_pos += (tl_offset = (tl_offset == 4 ? tl_offset - 1 :tl_offset + 1)), tl_idx++,  curr_row = tl_r, tl_r = tl_pos / 4)
                {
                    moves[tl_idx] = tl_pos;
                }

                //Bottom right
                int br_idx = 24;
                int br_offset = bottom_right_offset;
                for(int br_pos = br, br_r = br_row, curr_row = row; 
                    br_pos >= 0 && br_pos < 32 && !(occupied_tiles & (mask << br_pos)) && abs(br_r - curr_row) == 1; 
                    br_pos += (br_offset = (br_offset == -4 ? br_offset + 1 : br_offset - 1)), br_idx++, curr_row= br_r, br_r = br_pos / 4)  
                {
                    moves[br_idx] = br_pos;
                }

                 
                //Bottom left
                int bl_idx = 32;
                int bl_offset = bottom_left_offset;
                for(int bl_pos = bl, bl_r = bl_row, curr_row = row;  
                    bl_pos >= 0 && bl_pos < 32 && !(occupied_tiles & (mask << bl_pos)) && abs(bl_r - curr_row) == 1; 
                    bl_pos += (bl_offset = (bl_offset == -4? bl_offset - 1 : bl_offset + 1)), bl_idx++, curr_row = bl_r, bl_r = bl_pos / 4)
                {
                    moves[bl_idx] = bl_pos;
                }
            }

            break;
        }
    }

    if(moves[forced_move] == 1){ //If there is a forced move remove all other moves
        for (int i = 0; i < 4; i++) {
            moves[i] = -1;
        } 
        for (int i = 8; i < 40; i++) {
            moves[i] = -1;
        }        
    }
    return moves;
}
/*
  1= Red Wins
  2= Black wins
  0= No win yet
*/
int check_win(Game_Board* board){
    uint32_t bk_p = board->black_kings | board->black_men;
    uint32_t rd_p = board-> red_men | board->red_kings;
    int turn =(board->current_turn % 2 == 1) ? 1 : 2; //1 = red 2 = black


    if (!bk_p) return 1; // Red wins
    if (!rd_p) return 2; // Black wins

    //Check for any of black moves
    if(turn == 2){
        int any_black_moves = 0;
        for (int i = 0; i < 32; i++) {
            if (is_piece_at(bk_p, i)) { // Check if there is a piece first
                int* moves = check_for_moves(board, i, 0);
                for (int j = 0; j < 40; j++) {  // different variable
                    if (moves[j] != -1) {
                        any_black_moves++;
                        break; // We found a move, no need to check anymore
                    }
                }
                free(moves);
            }
        }
        if(!any_black_moves){ //Checked and no black moves on black's turn, this means red won
            return 1;
        }
    }

    //Check for any of red moves
    if(turn ==1){        
        int any_red_moves = 0;
        for(int i = 0; i <32; i++){
            if(is_piece_at(rd_p,i)){ //Check if there is a piece first
                int* moves = check_for_moves(board, i, 0);
                for(int j = 0; j<40;j++){
                    if(moves[j] != -1){
                        any_red_moves ++;
                        break; //We found a move no need to check anymore
                    }
                }
                free(moves);
                }
            }
        if(!any_red_moves){ //Checked and no black moves on black's turn, this means red won
            return 2;
        }    
    }
    return 0;
}
void enter_to_cont() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {} // flush
    fflush(stdout);
}
char* index_to_tile(int index) {
    if (index < 0 || index > 31) {  
        return NULL;
    }
    char *notation = malloc(3 * sizeof(char)); 
    if (!notation) return NULL;
    
    int row = (index / 4) + 1;  
    int col_offset = (index % 4) * 2;
    col_offset += (row % 2 == 0) ? 1 : 0; 
    char col = 'A' + col_offset;
    
    sprintf(notation, "%c%d", col, row);
    return notation;

}
char* allowed_moves_to_string(int *moves) {
    char *buffer = malloc(512);
    buffer[0] = '\0';

    for (int i = 0; i < 40; i++) {
        if (moves[i] != -1) {
            int index = moves[i];
            int row = index / 4;

            int dark_square_num = index % 4;

            int col = dark_square_num * 2 + (row % 2);
            
            char col_char = 'A' + col;
            char row_char = '1' + row;

            char temp[8];
            snprintf(temp, sizeof(temp), "%c%c ", col_char, row_char);
            strcat(buffer, temp);
        }
    }

    // Remove the trailing comma
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == ' ') {
        buffer[len - 1] = '\0';
    }

    return buffer;
}
void check_for_promotion(Game_Board* board){
    //Red promotion
    for (int pos = 28; pos <= 31; pos++) {
        if (is_piece_at(board->red_men, pos)) {
            remove_piece_from_tile(&board->red_men, pos);
            set_tile_to_piece(&board->red_kings, pos);
        }
    }

    // black promotions
    for (int pos = 0; pos <= 3; pos++) {
        if (is_piece_at(board->black_men, pos)) {
            remove_piece_from_tile(&board->black_men, pos);
            set_tile_to_piece(&board->black_kings, pos);
        }
    } 
}
void turn(Game_Board* board){
    int to,from;
    int game_state = check_win(board);
    while(game_state == 0){
        print_screen(board);
        int input = get_user_input(board);
        printf("\n");
        switch(input){//Check for errors
            case -1:
                char* dir = "saves";
                print_text_padding();
                create_save_file(board, dir);
                printf("Game saved!");
                enter_to_cont();
                break;
            case -2:
                char response[8];
                int confirmed = 0;
                while (!confirmed) {
                    print_text_padding();
                    printf("Are you sure you want to exit? Y/N: ");
                    if (!fgets(response, sizeof(response), stdin)) continue;

                    if (response[0] == 'Y' || response[0] == 'y') {
                        print_text_padding();
                        return; //Exit out of turn
                    } else if (response[0] == 'N' || response[0] == 'n') {
                        confirmed = 1;
                    } else {
                        print_text_padding();
                        printf("Please enter Y or N.\n");
                    }
                }
                break;
            case -3:
            case -7:
                print_text_padding();
                printf("Invalid Input");
                enter_to_cont();
                break;
            case -4:
                print_text_padding();
                printf("This tile does not exist or is a white tile.");
                enter_to_cont();
                break;
            case -5:
                print_text_padding();
                printf("You have selected an empty tile or a piece that is not yours.");
                enter_to_cont();
                break;
            case -6:
                print_text_padding();
                printf("There is a piece to be taken...");               
                enter_to_cont();
                break;
            default:
                int* allowed_moves = check_for_moves(board, input, 0);
                int has_move = 0;
                for (int i = 0; i < 40; i++) {
                    if (allowed_moves[i] != -1) {
                        has_move = 1;
                        break;
                    }
                }
                if(!has_move){
                    print_text_padding();
                    printf("This piece has no legal moves.");
                    goto end;
                }
                printf("\n");
                print_text_padding();
                char* tile = index_to_tile(input);
                printf("You selected tile: %s\n", tile);
                free(tile);

                printf("\n");
                print_text_padding();
                char* coords = allowed_moves_to_string(allowed_moves);
                printf("Allowed tiles: %s\n", coords);
                free(coords);

                printf("\n");
                print_text_padding();
                printf("Select tile to move to: \n");

                int second_input = second_get_user_input(board);
                if (second_input == -1){
                    print_text_padding();
                    printf("Invalid input");
                    enter_to_cont();
                    goto end;
                }
                if (second_input == -2){
                    print_text_padding();
                    printf("Destination tile is occupied");
                    enter_to_cont();
                    goto end;
                }
                int is_second_legal = 0;
                for (int i = 0; i < 40; i++) { //Check if its  legal move
                    if (allowed_moves[i] == second_input) {
                        is_second_legal = 1;
                        break;
                    }
                }
                if(!is_second_legal){
                    printf("\n");
                    print_text_padding();
                    printf("That is not a legal move.");
                    enter_to_cont();
                    goto end;
                }
                from = input;
                to = second_input;
                int current_player = (board->current_turn % 2 == 1) ? 1 : 2;
                int piece_type = get_piece_type(board, from);
                uint32_t* mover_board = NULL;
                switch (piece_type) {
                    case 1: // red man
                        mover_board = &board->red_men;
                        break;

                    case 2: //red king
                        mover_board = &board->red_kings;
                        break;

                    case 3: //black man
                        mover_board = &board->black_men;
                        break;

                    case 4: //black king
                        mover_board = &board->black_kings;
                        break;

                    case 0:
                    default:
                        mover_board = NULL; //empty how did this happen
                        break;
                }
                if (mover_board == NULL) {
                    print_text_padding();
                    printf("No valid piece selected (index %d)\n", to);
                    enter_to_cont();
                    goto end;
                }
                if(allowed_moves[40] == 1 ){//This is a forced take
                    take_piece(board,from,to,current_player);
                    //Check for double jumps
                    int has_capture = 0;
                    int double_start = to;
                    int double_second_input;
                    do{
                        print_screen(board);
                        int* next_moves = check_for_moves(board, double_start, piece_type);
                        has_capture = next_moves[40];
                        if(has_capture){
                            char* double_coords = allowed_moves_to_string(next_moves);
                            print_text_padding();
                            printf("Double jump available at %s\n",double_coords);
                            free(double_coords);
                            print_text_padding();
                            printf("Select tile to jump to: \n");
                            double_second_input = second_get_user_input(board);
                            if (double_second_input == -1){
                                print_text_padding();
                                printf("Invalid input");
                                free(next_moves);
                                continue;
                            }
                            if (double_second_input == -2){
                                print_text_padding();
                                printf("Destination tile is occupied");
                                free(next_moves);
                                continue;
                            }
                            int double_is_second_legal = 0;
                            for (int i = 4; i < 8; i++) { //Check if its legal move
                                if (next_moves[i] == double_second_input) {
                                    double_is_second_legal = 1;
                                    break;
                                }
                            }
                            if(double_is_second_legal){
                                take_piece(board,double_start,double_second_input,current_player);
                                double_start = double_second_input;
                            }
                        }
                        free(next_moves);
                    }while(has_capture!=0);
                }else{ //Not a forced take just move piece
                    move_piece(mover_board,from,to);
                }
                board->current_turn++;
                end:                
                break;
        }
        check_for_promotion(board);
        game_state = check_win(board);
    }
    //Game is over

    print_screen(board);
    if(game_state == 1){
        printf("\n");
        for(int i = 0; i<11;i++){
            printf("%*s",  (PADDING*3)/2, "");
            printf("PLAYER 1 WINS!!\n");
            
        }
        enter_to_cont();
    }
    if(game_state == 2){
        printf("\n");
        for(int i = 0; i<11;i++){
            printf("%*s",  (PADDING*3)/2, "");
            printf("PLAYER 2 WINS!!\n");
        }
        enter_to_cont();
    }
}