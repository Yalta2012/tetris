#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define SCREEN_X_SIZE 10
#define SCREEN_Y_SIZE 20
#define SCORE_BOARD_SIZE 5
#define SAVE_FILE "score_list.txt"

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
enum _colors
{
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    PURPLE = 5
};

int rand_int(int a, int b)
{
    return a + (rand()) % (b - a + 1);
}

typedef struct figure
{
    int x;
    int y;
    int color;
    int type;
    int blocks[4][4];
} figure;

typedef struct score_name
{
    char name[6];
    int score;
} score_name;

void rotate_figure(figure *a)
{

    int buf[4][4] = {0};
    int i, j;
    if (a->type == 0)
    {
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                buf[i][j] = a->blocks[j][i];
            }
        }
    }
    else if (a->type == 1)
    {
        return;
    }
    else if (a->type > 1)
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                buf[i][j] = a->blocks[2 - j][i];
            }
        }

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            a->blocks[i][j] = buf[i][j];
        }
    }
}

void create_figure(figure *a)
{
    int figures[7][4][4] = {
        {{0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0}},

        {{0, 0, 0, 0},
         {0, 1, 1, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0}},

        {{0, 1, 0, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},

        {{1, 1, 0, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},

        {{0, 1, 1, 0},
         {1, 1, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},

        {{0, 0, 1, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},

        {{1, 0, 0, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
    };
    int i, j;
    a->color = rand_int(1, 5);
    a->type = rand_int(0, 6);
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            a->blocks[i][j] = figures[a->type][i][j];
        }
    }
    a->x = 3;
    for (i = 3; i >= 0; i--)
    {
        for (j = 0; j < 4; j++)
        {
            if (a->blocks[i][j])
            {
                a->y = j - 5;
                return;
            }
        }
    }
}

void print_frame(int score, int field[][SCREEN_X_SIZE], const figure *block, const figure *next_block, int pause)
{
    int i, j;
    HANDLE hCon;
    COORD cPos;
    hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    cPos.X = 0;
    cPos.Y = 0;
    SetConsoleCursorPosition(hCon, cPos);
    for (i = 0; i < SCREEN_X_SIZE * 3 + 2; i++)
    {
        printf("=");
    }
    printf("\n");

    for (i = 0; i < SCREEN_Y_SIZE; i++)
    {
        printf("|");
        for (j = 0; j < SCREEN_X_SIZE; j++)
        {
            if ((i - block->y >= 0) && (i - block->y < 4) && (j - block->x >= 0) && (j - block->x < 4) && (block->blocks[i - block->y][j - block->x]))
            {
                printf("\x1B[1;3%dm[ ]\e[0m", block->color);
            }
            else if (field[i][j] == 0)
            {
                printf(" . ");
            }
            else
            {
                printf("\x1B[1;3%dm[ ]\e[0m", field[i][j]);
            }
        }
        printf("|");
        if (pause && i == 5)
        {
            printf("\tPAUSE");
        }
        else if (i == 5)
        {
            printf("\t     ");
        }

        if (i == 6)
            printf("\tSCORE: %d", score);
        else if (i == 8)
            printf("\tNEXT: ");
        else if (i == 9)
        {
            printf("\t");
            for (j = 0; j < 3 * 4 + 2; j++)
            {
                printf("=");
            }
        }
        else if (i > 9 && i < 14)
        {
            printf("\t|");
            for (j = 0; j < 4; j++)
            {
                if (next_block->blocks[i - 10][j])
                {

                    printf("\x1B[1;3%dm[ ]\e[0m", next_block->color);
                }
                else
                {
                    printf("   ");
                }
            }
            printf("|");
        }
        else if (i == 14)
        {
            printf("\t");

            for (j = 0; j < 3 * 4 + 2; j++)
            {
                printf("=");
            }
        }
        printf("\n");
    }

    for (i = 0; i < SCREEN_X_SIZE * 3 + 2; i++)
    {
        printf("=");
    }
    printf("\n");
}

int check_bottom(figure *a, int field[][SCREEN_X_SIZE])
{
    int i, j;
    int x, y;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (a->blocks[i][j])
            {
                x = a->x + j;
                y = a->y + i;
                if (y < 0)
                    break;
                if (y > SCREEN_Y_SIZE - 1 || field[y][x])
                {
                    return 0;
                }
            }
        }
    }

    return 1;
}

int check_turn(figure *a, int field[][SCREEN_X_SIZE])
{

    int i, j;
    int x, y;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (a->blocks[i][j])
            {
                x = a->x + j;
                y = a->y + i;

                if (x > SCREEN_X_SIZE - 1 || x < 0 || y > SCREEN_Y_SIZE - 1 || (field[y][x] && y >= 0))
                {
                    return 0;
                }
            }
        }
    }

    return 1;
}

void overlay(figure *a, int field[][SCREEN_X_SIZE])
{
    int i, j;
    int x, y;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (a->blocks[i][j])
            {
                x = a->x + j;
                y = a->y + i;
                if (y >= 0)
                {
                    // printf("NEW: x = %d y = %d\n", x, y);
                    // printf("NEW: j = %d i = %d\n", j, i);
                    field[y][x] = a->color;
                }
            }
        }
    }
}

int check_field(int field[][SCREEN_X_SIZE])
{
    int i, j, c, f, k;
    c = 0;
    for (i = SCREEN_Y_SIZE - 1; i >= 0; i--)
    {
        f = 1;
        for (j = 0; j < SCREEN_X_SIZE; j++)
        {
            if (!field[i][j])
            {
                f = 0;
                break;
            }
        }

        if (f)
        {
            c = c * 2 + 100;
            for (k = i; k >= 1; k--)
            {
                for (j = 0; j < SCREEN_X_SIZE - 1; j++)
                {
                    field[k][j] = field[k - 1][j];
                }
            }
            i++;
        }
    }

    return c;
}

int game()
{
    clock_t start_time, frame_start_time;

    int scr_x = SCREEN_X_SIZE;
    int scr_y = SCREEN_Y_SIZE;
    int score;
    figure block;
    figure next_block;
    figure buf_block;
    char input;
    int field[SCREEN_Y_SIZE][SCREEN_X_SIZE] = {0};
    int i;
    int pause;
    double change_frame_time;
    create_figure(&block);
    create_figure(&next_block);

    buf_block = block;
    change_frame_time = 0.25;
    pause = 0;
    input = 0;
    score = 0;
    while (1)
    {
        frame_start_time = clock();
        while (((double)(clock() - frame_start_time)) / CLOCKS_PER_SEC < change_frame_time)
        {
            start_time = clock();

            // system("cls"); //

            print_frame(score, field, &block, &next_block, pause);
            // printf("%d\n", input);
            while (((double)(clock() - start_time)) / CLOCKS_PER_SEC < 0.03)
            {
                if (kbhit())
                {
                    input = _getch();
                }
            }
            if (input == 27)
            {
                pause = !pause;
            };
            if (!pause)
            {

                if (input == 'h') // LEFT
                {
                    buf_block.x--;
                    if (check_turn(&buf_block, field))
                    {
                        block.x--;
                    }
                }

                if (input == 'u') // ROTATE
                {
                    rotate_figure(&buf_block);
                    if (check_turn(&buf_block, field))
                    {
                        rotate_figure(&block);
                    }
                }

                if (input == 'j') // DOWN
                {
                    buf_block.y++;
                    if (check_turn(&buf_block, field))
                    {
                        block.y++;
                    }
                }

                if (input == 'k')
                {
                    buf_block.x++;
                    if (check_turn(&buf_block, field))
                    {
                        block.x++;
                    }
                }
            }

            input = 0;
        }
        if (pause)
        {
            continue;
        }
        buf_block = block;

        buf_block.y++;
        if (check_bottom(&buf_block, field))
        {

            block.y++;
        }
        else
        {
            if (buf_block.y < 0)
            {
                return score;
            }
            overlay(&block, field);
            block = next_block;
            create_figure(&next_block);
            // return 0;
        }
        score += check_field(field);
        change_frame_time = ((25 - (score / 1000)) / 100.0);
    }
}

int save_score_board(score_name *list, char *way)
{
    FILE *f = fopen(way, "w");
    int i;
    if (!f)
        return 0;
    for (i = 0; i < SCORE_BOARD_SIZE; i++)
    {
        fprintf(f, "%s %d\n", list[i].name, list[i].score);
    }
    return 1;
}

void sort_board(score_name *list)
{
    int i, j;
    score_name temp;
    for (int i = 0; i < SCORE_BOARD_SIZE; i++)
    {
        for (int j = i + 1; j < SCORE_BOARD_SIZE; j++)
        {
            if (list[i].score < list[j].score)
            {
                temp = list[i];
                list[i] = list[j];
                list[j] = temp;
            }
        }
    }
}

int import_score_board(score_name *list, char *way)
{
    FILE *f = fopen(way, "r");
    char chbuf[100];
    int i;
    if (!f)
    {
        f = fopen(way, "w");
        if (!f)
            return 0;
        for (i = 0; i < SCORE_BOARD_SIZE; i++)
        {
            fprintf(f, "EMPTY 0\n");
        }
        fclose(f);
    }
    fclose(f);
    f = fopen(way, "r");
    for (i = 0; i < SCORE_BOARD_SIZE; i++)
    {
        if (fscanf(f, "%s %d", chbuf, &list[i].score) != 2)
        {
            strcpy(list[i].name, "EMPTY");
            list[i].score = 0;
        }
        else
        {
            list[i].name[5] = 0;
            strncpy(list[i].name, chbuf, 5);
        }
    }
    fclose(f);
    sort_board(list);
    return 1;
}

int new_records(score_name *score_list, int score, char *way)
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO structCursorInfo;
    GetConsoleCursorInfo(console, &structCursorInfo);
    structCursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(console, &structCursorInfo);

    COORD cPOS;
    int i, j;
    char b;
    char buf[6] = {0};
    for (i = 0; i < SCORE_BOARD_SIZE; i++)
    {
        if (score > score_list[i].score)
        {
            printf("\t!!!NEW RECORD!!!\n"
                   "\tEnter your name:\n"
                   ">>_____");
            cPOS.X = 2;
            cPOS.Y = 6;
            SetConsoleCursorPosition(console, cPOS);
            for (j = 0; j < 5; j++)
            {
                b = _getche();
                if (b >= '0' && b < 127)
                {
                    buf[j] = b;
                }
                else
                {
                    if (!b)
                    {
                        _getch();
                    }
                    cPOS.X = 2 + j;
                    SetConsoleCursorPosition(console, cPOS);
                    printf("_");
                    SetConsoleCursorPosition(console, cPOS);
                    j--;
                }
            }
            for (j = SCORE_BOARD_SIZE - 1; j > i; j--)
            {
                score_list[j].score = score_list[j - 1].score;
                strcpy(score_list[j].name, score_list[j - 1].name);
            }
            strcpy(score_list[i].name, buf);
            score_list[i].name[6] = 0;

            score_list[i].score = score;
            save_score_board(score_list, SAVE_FILE);
            break;
        }
    }
}

int main()
{

    srand(time(NULL));
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode;
    GetConsoleMode(console, &consoleMode);
    consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(console, consoleMode);

    CONSOLE_CURSOR_INFO structCursorInfo;
    GetConsoleCursorInfo(console, &structCursorInfo);
    structCursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(console, &structCursorInfo);

    COORD cPOS;

    clock_t start_time;
    int score;
    int key;
    int i, j;
    char b;
    score_name score_list[SCORE_BOARD_SIZE];
    FILE *f;

    import_score_board(score_list, SAVE_FILE);
    while (1)
    {
        system("cls");

        printf("\x1b[1;30;47m\tTETRIS\x1b[0m\n\n");
        printf("\tMenu\n"
               "\t1.Start game\n"
               "\t2.Control\n"
               "\t3.Leaderboard\n"
               "\t0.Exit\n"
               "\t>> ");
        scanf("%d", &key);
        fflush(stdin);

        switch (key)
        {
        case 1:
            system("cls");
            score = game();
            system("cls");
            printf("\x1b[1;30;47m\tGAME OVER\x1b[0m\n\n");
            printf("\n\tYOUR SCORE: %d\n", score);
            start_time = clock();
            while (((double)(clock() - start_time)) / CLOCKS_PER_SEC < 2)
            {
                _getch();
            }
            new_records(score_list, score, SAVE_FILE);

            break;
        case 2:
            system("cls");
            printf("\x1b[1;30;47m\tCONTROL\x1b[0m\n\n"
                   "[Esc] - PAUSE\n"
                   "      ROTATE - [u]\n"
                   "    LEFT - [h] [j] [k] - RIGHT \n"
                   "\t\t|\n"
                   "\t       DOWN  \n"
                   "\tback - any key\n"
                   "\t>> ");
            _getch();
            break;
        case 3:
            system("cls");
            printf("\x1b[1;30;47m\tRECORDS\x1b[0m\n");
            for (i = 0; i < SCORE_BOARD_SIZE; i++)
            {
                printf("\t%s %d\n", score_list[i].name, score_list[i].score);
            }
            printf("\tback - any key\n\t>> ");
            _getch();
            break;
        case 0:
            return 0;
        default:
            printf("Unknown command\n");
            break;
        }
    }
    return 0;
}
