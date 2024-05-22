#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define SCREEN_X_SIZE 10
#define SCREEN_Y_SIZE 20

enum _moving
{
    STOP,
    ROTATE,
    RIGHT,
    DOWN,
    LEFT
};

enum _colors
{
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    PURPLE = 5
}

rand_int(int a, int b)
{
    return a + (rand()) % (b - a + 1);
}

typedef struct point
{
    int x;
    int y;
} point;

typedef struct figure
{
    point coords;
    int color;
    int type;
    int blocks[4][4];
} figure;

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
    a->coords.x = 3;
    for (i = 3; i >= 0; i--)
    {
        for (j = 0; j < 4; j++)
        {
            if (a->blocks[i][j])
            {
                a->coords.y = j - 5;
                return;
            }
        }
    }
}

void print_frame(int score, int field[][SCREEN_X_SIZE], const figure *block, const figure *next_block)
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
            if ((i - block->coords.y >= 0) && (i - block->coords.y < 4) && (j - block->coords.x >= 0) && (j - block->coords.x < 4) && (block->blocks[i - block->coords.y][j - block->coords.x]))
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
                x = a->coords.x + j;
                y = a->coords.y + i;
                if (y < 0)
                    break;
                if (y > SCREEN_Y_SIZE - 1 || field[y][x])
                {
                    // printf("%d %d\n", x, y);
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
                x = a->coords.x + j;
                y = a->coords.y + i;
                // if (y < 0)
                //     break;
                if (x > SCREEN_X_SIZE - 1 || x < 0 || y > SCREEN_Y_SIZE - 1 || (field[y][x] && y >= 0))
                {
                    // printf("%d %d\n", x, y);
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
                x = a->coords.x + j;
                y = a->coords.y + i;
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
    int score = 0;
    figure block;
    figure next_block;
    figure buf_block;
    char input = 0;
    int direction;
    int field[SCREEN_Y_SIZE][SCREEN_X_SIZE] = {0};
    int i;
    create_figure(&block);
    create_figure(&next_block);

    buf_block = block;

    while (1)
    {
        frame_start_time = clock();
        while (((double)(clock() - frame_start_time)) / CLOCKS_PER_SEC < 0.2)
        {
            start_time = clock();

            // system("cls"); //

            print_frame(score, field, &block, &next_block);
            // printf("%d\n", input);
            while (((double)(clock() - start_time)) / CLOCKS_PER_SEC < 0.05)
            {
                if (kbhit())
                {
                    input = _getch();
                }
                else
                {
                    direction = STOP;
                }
            }

            if (input == 'h') // LEFT
            {
                buf_block.coords.x--;
                if (check_turn(&buf_block, field))
                {
                    block.coords.x--;
                }
            }

            if (input == 'u') // ROTATE
            {
                direction = ROTATE;
                rotate_figure(&buf_block);
                if (check_turn(&buf_block, field))
                {
                    rotate_figure(&block);
                }
            }

            if (input == 'j') // DOWN
            {
                direction = DOWN;
                buf_block.coords.y++;
                if (check_turn(&buf_block, field))
                {
                    block.coords.y++;
                }
            }

            if (input == 'k')
            {
                direction = RIGHT;
                buf_block.coords.x++;
                if (check_turn(&buf_block, field))
                {
                    block.coords.x++;
                }
            };

            input = 0;
        }
        buf_block = block;

        buf_block.coords.y++;
        if (check_bottom(&buf_block, field))
        {

            block.coords.y++;
        }
        else
        {
            if (buf_block.coords.y < 0)
            {
                return score;
            }
            overlay(&block, field);
            block = next_block;
            create_figure(&next_block);
            // return 0;
        }
        score += check_field(field);
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
    printf("SCORE: %d", game());
    return 0;
}
