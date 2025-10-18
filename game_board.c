#include "game_board.h"

const uint32_t mask = 1u;

//Sets the inital board in a standard checkers game
void set_new_board( uint32_t* red_m, uint32_t* black_m){
    *red_m |= 0b111111111111;
    *black_m = 0b111111111111<<20;
}

//Sets the tile of a board to a piece
void set_tile_to_piece(uint32_t* board, int position){ 
    *board |= (mask << position);
}

void remove_piece_from_tile(uint32_t* board, int position){ //remove a piece from a tile
    *board &= ~(mask << position);
}
//Returns 1 if a piece exists on that board, 0 otherwise.
int is_piece_at(uint32_t board, int position){
    return ((board)>>position) & 1;
}
void turn(Game_Board* board){
    unsigned char to,from;
    while(!check_win){
        print_player_turn(board);
        int input = get_user_input(board);
        switch(input){//Check for errors
            case -1:
            //TODO: SAVE GAME
                break;
            case -2:
                // TODO: EXIT GAME 
                break;
            case -3:
                // TODO: HANDLE INVALID INPUT
                break;
            case -4:
                // TODO: HANDLE TILE NOT EXISTS
                break;
            case -5:
                // TODO: HANDLE NO PIECE ON TILE
                break;
            default:

        }
    }
}
/*
  1= Red Wins
  2= Black wins
  3= Draw
  0= No win yet
*/
int check_win(Game_Board* board){
    uint32_t bk_p = board->black_kings | board->black_men;
    uint32_t rd_p = board-> red_men | board->red_kings;
    int any_black_moves = 0;
    int any_red_moves = 0;

    if (!bk_p) return 1; // Red wins
    if (!rd_p) return 2; // Black wins

    //Check for any of black moves
    for (int i = 0; i < 32; i++) {
        if (is_piece_at(rd_p, i)) { // Check if there is a piece first
            int* moves = check_for_moves(board, i, 0);
            for (int j = 0; j < 40; j++) {  // different variable
                if (moves[j] != 1) {
                    any_black_moves++;
                    break; // We found a move, no need to check anymore
                }
            }
            free(moves);
            if (any_black_moves) {
                break;
            }
        }
    }
    //Check for any of red moves
    for(int i = 0; i <32; i++){
        if(is_piece_at(rd_p,i)){ //Check if there is a piece first
            int* moves = check_for_moves(board, i, 0);
            for(int j = 0; j<40;j++){
                if(moves[j] != 1){
                    any_red_moves ++;
                    break; //We found a move no need to check anymore
                }
            }
            free(moves);
            if(any_red_moves){
                break;
            }
        }
    }
    if(!any_black_moves && !any_red_moves){//Both have no moves
        return 3; //Draw
    }
    if(!any_red_moves){ //Red has no moves when black does, black wins
        return 2;
    }else if(!any_black_moves){ //Black has no moves when red does, red wins
        return 1;
    }else{
        // Both have moves, game not finished
        return 0;
    }
}

/*  Checks for all valid moves for a piece and returns an int of the tile index that a piece can go
    including forced takes and such
    
    If a forced take is possible then all other moves are removed and only the forced takes returned

    Call this function again when piece was taken (returned_pointer[12] = 1)

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

int* check_for_moves(Game_Board* board, int position, int multiple_hops){

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

    uint32_t bit = mask << position;

    if (board->red_men & bit) piece_type = 1;
    else if (board->red_kings & bit) piece_type = 2;
    else if (board->black_men & bit) piece_type = 3;
    else if (board->black_kings & bit) piece_type = 4;


    int top_left_offset,
        top_right_offset,
        bottom_left_offset,
        bottom_right_offset;

    int row = position / 4;
    int col = position % 4;

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
                tr_next = tr + top_right_offset + 1;
            } else{ //T_R_offset = 5
                tr_next = tr + top_right_offset -1;
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
                    moves[0] = tl_next;
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

                moves[forced_move] = 1;
                
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
                    if(bottom_right_offset == -5){
                        br_next = br + (bottom_right_offset + 1);
                    } else{ //-4
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
                    if(bottom_left_offset == -4){
                        bl_next = bl + (bottom_left_offset + 1);
                    } else{ //-3
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
                for(int tr_pos = tr, tr_r = tr_row, curr_row = row;
                    tr_pos >= 0 && tr_pos < 32 && !(occupied_tiles & (mask << tr_pos)) && abs(tr_r - curr_row) == 1; 
                    tr_pos += (top_right_offset == 4 ? top_right_offset + 1 : top_right_offset - 1), tr_idx++, curr_row = tr_r, tr_r = tr_pos / 4)
                {
                    moves[tr_idx] = tr_pos;
                }


                //Top left
                int tl_idx = 16;
                for(int tl_pos = tl, tl_r = tl_row, curr_row = row; 
                    tl_pos >= 0 && tl_pos < 32 && !(occupied_tiles & (mask << tl_pos)) && abs(tl_r - curr_row) == 1; 
                    tl_pos += (top_left_offset == 3 ? top_left_offset + 1 : top_left_offset - 1), tl_idx++,  curr_row = tl_r, tl_r = tl_pos / 4)
                {
                    moves[tl_idx] = tl_pos;
                }

                //Bottom right
                int br_idx = 24;
                for(int br_pos = br, br_r = br_row, curr_row = row; 
                    br_pos >= 0 && br_pos < 32 && !(occupied_tiles & (mask << br_pos)) && abs(br_r - curr_row) == 1; 
                    br_pos += (bottom_right_offset == -4 ? bottom_right_offset + 1 : bottom_right_offset - 1), br_idx++, curr_row= br_r, br_r = br_pos / 4)  
                {
                    moves[br_idx] = br_pos;
                }

                 
                //Bottom left
                int bl_idx = 32;
                for(int bl_pos = bl, bl_r = bl_row, curr_row = row;  
                    bl_pos >= 0 && bl_pos < 32 && !(occupied_tiles & (mask << bl_pos)) && abs(bl_r - curr_row) == 1; 
                    bl_pos += (bottom_left_offset == -5 ? bottom_left_offset + 1 : bottom_left_offset - 1), bl_idx++, curr_row = bl_r, bl_r = bl_pos / 4)
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
                br_next = br + (bottom_right_offset - 1);
            } else{ //B_R_offset = -5
                br_next = br + (bottom_right_offset + 1);
            }
            int br_next_row = br_next/4;

            //bottom left next
            int bl_next;
            //Swap parity for next row
            if(bottom_left_offset == -3){
                bl_next = bl + (bottom_left_offset - 1);
            } else{ //B_L_offset = -4
                bl_next = bl + (bottom_left_offset +1);
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

            int simple_move_idx[4] = {tr,tl,br,bl};
            //Check for jumps first
            if ((tr_occ && abs(tr_row - row) == 1 && tr >= 0 && tr < 32) ||
                (tl_occ && abs(tl_row - row) == 1 && tl >= 0 && tl < 32) ||
                (br_occ && abs(br_row - row) == 1 && br >= 0 && br < 32) ||
                (bl_occ && abs(bl_row - row) == 1 && bl >= 0 && bl < 32)) {

                moves[forced_move] = 1;
                
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
                    if(bottom_right_offset == -5){
                        br_next = br + (bottom_right_offset + 1);
                    } else{ //-4
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
                    if(bottom_left_offset == -4){
                        bl_next = bl + (bottom_left_offset + 1);
                    } else{ //-3
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
                for(int tr_pos = tr, tr_r = tr_row, curr_row = row;
                    tr_pos >= 0 && tr_pos < 32 && !(occupied_tiles & (mask << tr_pos)) && abs(tr_r - curr_row) == 1; 
                    tr_pos += (top_right_offset == 4 ? top_right_offset + 1 : top_right_offset - 1), tr_idx++, curr_row = tr_r, tr_r = tr_pos / 4)
                {
                    moves[tr_idx] = tr_pos;
                }


                //Top left
                int tl_idx = 16;
                for(int tl_pos = tl, tl_r = tl_row, curr_row = row; 
                    tl_pos >= 0 && tl_pos < 32 && !(occupied_tiles & (mask << tl_pos)) && abs(tl_r - curr_row) == 1; 
                    tl_pos += (top_left_offset == 3 ? top_left_offset + 1 : top_left_offset - 1), tl_idx++,  curr_row = tl_row, tl_r = tl_pos / 4)
                {
                    moves[tl_idx] = tl_pos;
                }

                //Bottom right
                int br_idx = 24;
                for(int br_pos = br, br_r = br_row, curr_row = row; 
                    br_pos >= 0 && br_pos < 32 && !(occupied_tiles & (mask << br_pos)) && abs(br_r - curr_row) == 1; 
                    br_pos += (bottom_right_offset == -4 ? bottom_right_offset + 1 : bottom_right_offset - 1), br_idx++, curr_row= br_r, br_r = br_pos / 4)  
                {
                    moves[br_idx] = br_pos;
                }

                 
                //Bottom left
                int bl_idx = 32;
                for(int bl_pos = bl, bl_r = bl_row, curr_row = row;  
                    bl_pos >= 0 && bl_pos < 32 && !(occupied_tiles & (mask << bl_pos)) && abs(bl_r - curr_row) == 1; 
                    bl_pos += (bottom_left_offset == -5 ? bottom_left_offset + 1 : bottom_left_offset - 1), bl_idx++, curr_row = bl_r, bl_r = bl_pos / 4)
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
//Checks for any valid moves for a full board and returns:
// 1 = there is a valid move
// 0 = there is none so game over
int any_vaild_moves(uint32_t pieces){
    pieces >= 3;
    for(int i; i<31;i++){
        
    }
}