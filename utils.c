#include "game_board.h"
#ifdef _WIN32
#include <windows.h>
#endif

#define PADDING 72 // spaces at start of each line
void print_binary(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
    }
}
void clear_screen(){
    printf("gsdfnkd");
    #ifdef _WIN32
        // Windows
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD pos = {0, 0};  // top-left
        SetConsoleCursorPosition(hConsole, pos);
    #else
        // Unix/Linux/macOS
        printf("\033[H");
    #endif
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
void print_line(int width){
    printf("%*s", PADDING, "");
    for (int i = 0; i < (width*9)+8; i++) {
        putchar('-');
    } printf("\n");
}
void print_player_turn(Game_Board* board){
    printf("%*s",  (PADDING*3)/2, "");
    int player = (board->current_turn % 2 == 0) ? 2 : 1; //If the current turn is even then its players 2 turn, otherwise its players 1's turn
    printf("Player %d's turn!\n\n\n", player);
}
void print_board(Game_Board* board){
    print_bitboards(board);
    //Print top bar
    print_player_turn(board);
    int cell_width = 9;//width of each tile including separator
    int row_height = 3; //height of each tile

    char char_buffer[512];
    char* charp = char_buffer;
    char* letters = "ABCDEFGH";

    for(int r = 0; r < row_height; r++){
        for(int pad = 0; pad < PADDING; pad++){
            *charp++ = ' ';
        }
        for(int c = 0; c < 8; c++){
            for(int s = 0; s < cell_width; s++){
                char ch = ' ';
                if(s == cell_width/2 && r == (row_height/2) && c != 0){
                    ch = letters[c-1];
                }
                *charp++ = ch;
            }
            *charp++ = '|';
        }
        *charp++ = '\n';
    }
    *charp = '\0';
    //Print the main board the peices
    printf("%s", char_buffer);
    print_line(cell_width);
    for (int i = 0; i<8;i++){

        char peice_buffer[512];
        char* p = peice_buffer;

        char* nums = "87654321";

        for(int r = 0; r < row_height; r++){
            for(int pad = 0; pad < PADDING; pad++){
                *p++ = ' ';
            }

            for(int c = 0; c < 9; c++){

                for(int s = 0; s < cell_width; s++){            
                    char ch = ' ';

                    int board_col = c;
                    int board_row = i;
                    int index = (7 - board_row) * 4 + (board_col / 2);
                    
                    int is_middle_of_cell = (s == cell_width / 2) && (r == row_height / 2);
                    int is_edge_column = (c == 0)||(c==8);
                    int is_dark_square = ((board_row + board_col) % 2 == 0);

                    if (is_middle_of_cell) {
                        if (is_edge_column) {
                            ch = nums[i];
                        } else if(is_dark_square){
                            ch = get_piece(board, index);
                        }
                    }
                    *p++ = ch;
                }
                if(c!=8){
                   *p++ = '|';
                }
            }
            *p++ = '\n';
        }
        *p = '\0';
        printf("%s", peice_buffer);
        print_line(cell_width);
    }
    printf("%s", char_buffer);
}
/*  return >= 0   = Successful, read return value
    return = -1  = save gamestate
    return = -2  = exit game
    return = -3  = string not formatted correctly
    return = -4  = tile does not exists (h9, g4)
    return = -5  = no peice on tile
*/
int vailidate_user_input(Game_Board* board, char* input){
    
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
    int row;
    int col;
    if(int_first){
        row = first - '0'-1; 
        col = second - 'A';
    }else{
        row = second - '0'-1;
        col = first - 'A';
    }
    //Just in case
    int index = (7 - row) * 4 + (col / 2);
    if(get_piece(board, index)){
        return index;
    }else{
        return -5;
    }
}
int get_user_input(Game_Board* board){
    printf("Select tile (Examples: A1, b2, 4g, 7H), or type in SAVE to save the game state, or type in EXIT to leave without saving\n");
    printf("Selection: ");
    char line[5];
    fgets(line, sizeof(line), stdin);
    line[strcspn(line, "\n")] = 0;
    return vailidate_user_input(board, line);
}
void print_screen(Game_Board * board){
    clear_screen();
    print_board(board);
}
