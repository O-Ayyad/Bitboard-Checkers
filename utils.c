#include "game_board.h"


const int PADDING = 72;
const int TEXT_PADDING = 55;
void print_binary(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
    }
}
void clear_screen(){
        printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}
char get_piece(Game_Board* board, int bit) {
    uint32_t mask = 1U << bit;
    if (board->red_men & mask)   return 'r';
    if (board->red_kings & mask) return 'R';
    if (board->black_men & mask) return 'b';
    if (board->black_kings & mask) return 'B';
    return ' ';
}
void print_bitboards(Game_Board* board) { //Print the binary value of the board
    clear_screen();

    //Kings
    printf("                                                          ");
    printf("Red kings bitboard:     ");
    print_binary(board->red_kings);
    printf("    ");

    printf("Black kings bitboard:     ");
    print_binary(board->black_kings);
    printf("\n\n");

    //Men
    printf("                                                          ");
    printf("Red men bitboard:       ");
    print_binary(board->red_men);
    printf("    ");

    printf("Black men bitboard:       ");
    print_binary(board->black_men);
    printf("\n\n\n");
}
void print_padding(){
    printf("%*s", PADDING, "");
}
void print_text_padding(){
    printf("%*s", TEXT_PADDING, "");
}
void print_line(int width){
    print_padding();
    for (int i = 0; i < (width*10)+8; i++) {
        putchar('-');
    } printf("\n");
}
void print_player_turn(Game_Board* board){
    printf("%*s",  ((PADDING*3)/2)-4, "");
    int player = (board->current_turn % 2 == 0) ? 2 : 1; //If the current turn is even then its players 2 turn, otherwise its players 1's turn
    char* color[2] = {"Red", "Black"};
    printf("Player %d's (%s) turn!\n\n\n", player, color[player - 1]);
}

void print_column_labels(int cell_width, int row_height) {
    char* letters = "ABCDEFGH";
    for (int r = 0; r < row_height; r++) {
        print_padding();
        for (int c = 0; c < 9; c++) {
            for (int s = 0; s < cell_width; s++) {
                char ch = ' ';
                if (s == cell_width / 2 && r == (row_height / 2) && c != 0)
                    ch = letters[c - 1];
                putchar(ch);
            }
            putchar('|');
        }
        putchar('\n');
    }
}
void print_board(Game_Board* board){
    print_bitboards(board);
    //Print top bar
    print_player_turn(board);
    int cell_width = 9;//width of each tile including separator
    int row_height = 3; //height of each tile

    print_column_labels(cell_width, row_height);
    //Print the main board the pieces
    print_line(cell_width);
    for (int i = 0; i < 8; i++) {

    char* nums = "87654321";

    for (int r = 0; r < row_height; r++) {

        // print padding
        for (int pad = 0; pad < PADDING; pad++) {
            putchar(' ');
        }

        for (int c = 0; c < 10; c++) {

            for (int s = 0; s < cell_width; s++) {
                char ch = ' ';

                int board_col = c;
                int board_row = i;

                int checkers_row = 7 - board_row;
                int visual_col = board_col - 1;

                int index = -1;
                if (board_col >= 1 && board_col <= 8) {
                    int is_dark = ((board_row + board_col) % 2 == 0);
                    if (is_dark) {
                        // Correct calculation based on row parity
                        int dark_square_in_row = visual_col / 2;
                        index = checkers_row * 4 + dark_square_in_row;
                    }
                }

                int is_middle_of_cell = (s == cell_width / 2) && (r == row_height / 2);
                int is_edge_column = (c == 0) || (c == 9);

                if (is_middle_of_cell) {
                    if (is_edge_column) {
                        ch = nums[i];
                    } else if (index >= 0 && index < 32) {
                        ch = get_piece(board, index);
                    }
                }
                putchar(ch);
            }
            if (c != 9) {
                putchar('|');
            }
        }
        putchar('\n');
    }
    print_line(cell_width);
}
    print_column_labels(cell_width, row_height);
    putchar('\n');
    putchar('\n');
}
/*  
    return >= 0   = Successful, read return value
    return = -1  = save gamestate
    return = -2  = exit game
    return = -3  = string not formatted correctly
    return = -4  = tile does not exists (h9, g4)
    return = -5  = no piece on tile
    return = -6  = There is a piece to be taken
    return = -7  = Other error
*/
int validate_user_input(Game_Board* board, char* input, int current_turn){
    int player_turn = (current_turn %2 == 0) ? 2 : 1; //1 for red 2 for black
    if(strcasecmp(input, "save") == 0){
        return -1;
    }
    if(strcasecmp(input, "exit") == 0){
        return -2;
    }
    //Check for string not formatted to two chars
    if (*(input+2) != '\0' ||
        *input == '\0' ||
        *(input+1) == '\0'||
        !input){
        return -3;
    }
    int int_first;
    char first = toupper(input[0]);
    char second = toupper(input[1]);
    //Check if the first char is a number or character
    if (strchr("12345678", first) && strchr("ABCDEFGH", second)) {
        int_first = 1;
    }
    //letter then number
    else if (strchr("ABCDEFGH", first) && strchr("12345678", second)) {
        int_first = 0;
    }
    else {
        return -4; // invalid input
    }
    int row,col;
    if(int_first){
        row = first - '1'; 
        col = second - 'A';
    }else{
        row = second - '1';
        col = first - 'A';
    }
    if (row < 0 || row > 7 || col < 0 || col > 7) {
        return -4;
    }
    int visual_row = 7 - row;  // Convert to display coordinates
    int visual_col = col;
    if ((visual_row + visual_col + 1) % 2 != 0) {  // +1 accounts for column offset
        return -4; // Not a dark square
    }
    int dark_square_in_row = col / 2;
    int index = row * 4 + dark_square_in_row;
    
    uint32_t color_board = (player_turn == 1)
        ? (board->red_kings | board->red_men)
        : (board->black_kings | board->black_men);
    
    if(!is_piece_at(color_board, index)){
        return -5;
    }

    int forced_pieces[32];
    int forced_count = 0;
    for (int pos = 0; pos < 32; pos++) {
        int piece_type = get_piece_type(board, pos);
        //Skip empty squares or opponent pieces
        if (piece_type == 0){
            continue;
        }
        if (player_turn == 1 && piece_type > 2){ //Black piece when its red turn
            continue;
        }
        if (player_turn == 2 && piece_type < 3) //vice versa
        {
            continue;
        }

        int* moves = check_for_moves(board, pos, piece_type);
        if (moves[40] == 1) {
            forced_pieces[forced_count++] = pos;
        }
        free(moves);
    }
    if (forced_count > 0) {
        int is_forced_piece = 0;
        for (int i = 0; i < forced_count; i++) {
            if (forced_pieces[i] == index) {
                is_forced_piece = 1;
                break;
            }
        }
        if (!is_forced_piece) {
            return -6;  // There are forced moves, but this isnt one of them
        }
    }
    return index;
}
int secondary_valid_user_input(Game_Board* board, char* input){
    if (*(input+2) != '\0' || *input == '\0' || *(input+1) == '\0' || !input){
        return -3;
    }
    
    int int_first;
    char first = toupper(input[0]);
    char second = toupper(input[1]);
    
    if (strchr("12345678", first) && strchr("ABCDEFGH", second)) {
        int_first = 1;
    }
    else if (strchr("ABCDEFGH", first) && strchr("12345678", second)) {
        int_first = 0;
    }
    else {
        return -1;
    }
    
    int row, col;
    if(int_first){
        row = first - '1';
        col = second - 'A';
    } else {
        row = second - '1';
        col = first - 'A';
    }
    
    if (row < 0 || row > 7 || col < 0 || col > 7) {
        return -1;
    }
    
    // Check if dark square
    int visual_row = 7 - row;
    int visual_col = col;
    if ((visual_row + visual_col + 1) % 2 != 0) {
        return -1;
    }
    
    int dark_square_in_row = col / 2;
    int index = row * 4 + dark_square_in_row;
    
    
    uint32_t full_board = board->black_kings | board->black_men | 
                          board->red_kings | board->red_men;
    
    if(!is_piece_at(full_board, index)){
        return index;
    } else {
        return -2;
    }
}
/*  return >= 0   = Successful, read return value
    return = -1  = save gamestate
    return = -2  = exit game
    return = -3  = string not formatted correctly
    return = -4  = tile does not exists (h9, g4)
    return = -5  = no piece on tile
    return = -6  = There is a piece to be taken
    return = -7  = Other error
*/
int get_user_input(Game_Board* board){    
    print_text_padding();
    printf("Select tile (Examples: A1, b2, 4g, 7H), or type in SAVE to save the game state, or type in EXIT to leave without saving\n");
    print_text_padding();
    printf("Selection: ");
    char line[64];
    fgets(line, sizeof(line), stdin);
    line[strcspn(line, "\n")] = 0;
    return validate_user_input(board, line,board->current_turn);
}
int second_get_user_input(Game_Board* board){    
    print_text_padding();
    printf("Selection: ");
    char line[64];
    fgets(line, sizeof(line), stdin);
    line[strcspn(line, "\n")] = 0;
    return secondary_valid_user_input(board, line);
}
void print_screen(Game_Board * board){
    clear_screen();
    print_board(board);
}
