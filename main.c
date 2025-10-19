#include "game_board.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define PATH_SEP "\\"
#else
#include <dirent.h>
#define PATH_SEP "/"
#endif




char **list_save_files(int *num_files, char* dir) {
    int capacity = 64;
    int count = 0;
    char **files = malloc(capacity * sizeof(char *));

#ifdef _WIN32
    WIN32_FIND_DATA findFileData;
    char search_path[260];
    snprintf(search_path, sizeof(search_path), "%s\\*", dir);

    HANDLE hFind = FindFirstFile(search_path, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        *num_files = 0;
        return files;
    }

    do {
        if (strcmp(findFileData.cFileName, ".") != 0 &&
            strcmp(findFileData.cFileName, "..") != 0) {
            if (count >= capacity) {
                capacity *= 2;
                files = realloc(files, capacity * sizeof(char *));
            }
            files[count++] = _strdup(findFileData.cFileName); // duplicate string
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);

#else
    DIR *_dir = opendir(dir);
    if (!_dir) {
        *num_files = 0;
        return files;
    }

    struct dirent *entry;
    while ((entry = readdir(_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) {
            if (count >= capacity) {
                capacity *= 2;
                files = realloc(files, capacity * sizeof(char *));
            }
            files[count++] = strdup(entry->d_name);
        }
    }

    closedir(_dir);
#endif

    *num_files = count;
    return files;
}

char* get_current_time() { //Gets system time and returns a char* of that time. Free after use
    char* buffer = malloc(24 * sizeof(char));
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, 24, "%Y-%m-%d_%H-%M-%S", t);
    return buffer;
}
void init_startup(char* dir){ //Creates a directory to hold save files
    struct stat st = {0};

    if (stat(dir, &st) == -1) {
        #ifdef _WIN32
            if (_mkdir(dir) != 0) {
                perror("Failed to create directory");
            }
        #else
            if (mkdir(dir, 0755) != 0) {
                perror("Failed to create directory");
            }
        #endif
    }
}
void create_save_file(Game_Board* board, char* dir){
    char *filename = get_current_time();
    if (!filename) return;
    char full_path[300];
    snprintf(full_path, sizeof(full_path), "%s" PATH_SEP "%s.txt", dir, filename);
    free(filename);

    FILE *file = fopen(full_path, "w");
    if (!file) {
        perror("Failed to open save file");
        return;
    }
    for (int i = 31; i >= 0; i--) {
        fputc((board->red_men >> i) & 1 ? '1' : '0', file);
    }
    fputc('\n', file);

    for (int i = 31; i >= 0; i--) {
        fputc((board->red_kings >> i) & 1 ? '1' : '0', file);
    }
    fputc('\n', file);

    for (int i = 31; i >= 0; i--) {
        fputc((board->black_men >> i) & 1 ? '1' : '0', file);
    }
    fputc('\n', file);

    for (int i = 31; i >= 0; i--) {
        fputc((board->black_kings >> i) & 1 ? '1' : '0', file);
    }
    fputc('\n', file);
    fclose(file);   
}
FILE* select_save_file(char** files, int file_amount){
    if (file_amount == 0) {
        return NULL;
    }
    for (int i = 0; i < 20; i++) {
        printf("\n");
    }
    printf("%*s",  (PADDING*3)/2, ""); 
    printf("Select a save file.\n");
    printf("\n");
    print_line(9);
    printf("\n");
    for (int i = 0; i < file_amount; i++) {
        print_padding();
        printf("File %d: %s\n", i + 1, files[i]);
    }
    printf("\n");
    print_line(9);
    printf("\n");

    int choice = 0;
    char input[32];
    while (1) {
        printf("\n");
        print_padding();
        printf("Type the number of a file to load, or type \"New Game\" to start fresh: ");

        if (!fgets(input, sizeof(input), stdin)){
            continue;
        }
        input[strcspn(input, "\n")] = '\0';


        if (strcasecmp(input, "new game") == 0) {
            return NULL;
        }

        if (sscanf(input, "%d", &choice) == 1 && choice >= 1 && choice <= file_amount) {
            break;
        }

        print_padding();
        printf("Invalid input. Please enter a number between 1 and %d or \"New Game\".\n", file_amount);
    }

    char full_path[300];
    snprintf(full_path, sizeof(full_path), "saves/%s", files[choice - 1]);
    FILE *f = fopen(full_path, "r");
    if (!f) {
        perror("Failed to open file");
        return NULL;
    }

    return f;
}
void load_save_file(FILE* fp, Game_Board* board) {
    char buffer[256];

    if (fgets(buffer, sizeof(buffer), fp)) {
        board->red_men = (uint32_t)strtoul(buffer, NULL, 2);
    }
    if (fgets(buffer, sizeof(buffer), fp)) {
        board->red_kings = (uint32_t)strtoul(buffer, NULL, 2);
    }
    if (fgets(buffer, sizeof(buffer), fp)) {
        board->black_men = (uint32_t)strtoul(buffer, NULL, 2);
    }
    if (fgets(buffer, sizeof(buffer), fp)) {
        board->black_kings = (uint32_t)strtoul(buffer, NULL, 2);
    }
    if (fgets(buffer, sizeof(buffer), fp)) {
        board->current_turn = atoi(buffer);
    }
}
void start_new_game(Game_Board* board){
    memset(board, 0, sizeof(Game_Board));
    board->current_turn = 1;
    set_new_board(&board->red_men, &board->black_men);
}
int main(){   
    char* dir = "saves";
    init_startup(dir);

    Game_Board board;
    int file_amount = 0;
    char **files = list_save_files(&file_amount, dir);
    if (file_amount == 0) {
        start_new_game(&board);
    } else {
        FILE *selected_file = select_save_file(files, file_amount);
        //Free file name char*
        for (int i = 0; i < file_amount; i++){
            free(files[i]);
        } 
        free(files);

        if (selected_file){
            load_save_file(selected_file, &board);
        }else{
            start_new_game(&board);
        }
    }
            turn(&board);
    return 0;
}
