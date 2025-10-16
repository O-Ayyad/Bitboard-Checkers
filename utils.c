#include "game_board.h"
#ifdef _WIN32
#include <windows.h>
#endif

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
void print_line(int width, int padding){
    printf("%*s", padding, "");
    for (int i = 0; i < (width*9)+8; i++) {
        putchar('-');
    } printf("\n");
}
void print_player_turn(Game_Board* board, int padding){
    printf("%*s",  (padding*3)/2, "");
    int player = (board->current_turn % 2 == 0) ? 2 : 1; //If the current turn is even then its players 2 turn, otherwise its players 1's turn
    printf("Player %d's turn!\n\n\n", player);
}
void print_board(Game_Board* board){
    print_bitboards(board);
    //Print top bar
    int padding = 72;// spaces at start of each line
    print_player_turn(board, padding);
    int cell_width = 9;//width of each tile including separator
    int row_height = 3; //height of each tile

    char char_buffer[512];
    char* charp = char_buffer;
    char* letters = "ABCDEFGH";

    for(int r = 0; r < row_height; r++){
        for(int pad = 0; pad < padding; pad++){
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
    print_line(cell_width,padding);
    for (int i = 0; i<8;i++){

        char peice_buffer[512];
        char* p = peice_buffer;

        char* nums = "87654321";

        for(int r = 0; r < row_height; r++){
            for(int pad = 0; pad < padding; pad++){
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
        print_line(cell_width,padding);
    }

    printf("%s", char_buffer);
}
void print_screen(Game_Board * board){
    clear_screen();
    print_board(board);
}