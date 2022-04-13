#include "greedySnake.h"
#include <stdio.h>
#include <stdlib.h>

MENU_COVER_TYP greedySnake_cover = {MENU_COVER_FUNC, NULL, NULL,
                                    greedySnake_cover_draw};
MENU_TASK_TYP  greedySnake_tasks = {greedySnake_init, greedySnake_uninit};
MENU_TYP greedySnake = {"greedySnake", &greedySnake_cover, &greedySnake_tasks,
                        greedySnake_key_handel};

static aos_task_t greedySnake_task_handle;

#define MIN_LENGTH 4

#define SNAKE_UP    0b0010
#define SNAKE_LEFT  0b0001
#define SNAKE_RIGHT 0b0100
#define SNAKE_DOWN  0b1000

typedef struct {
    uint8_t  length;
    int16_t *XPos;
    int16_t *YPos;
    uint8_t  cur_dir;
    uint8_t  alive;
} Snake;

// left top -> (0,0)

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t eaten;
} Food;

typedef struct {
    int16_t border_top;
    int16_t border_right;
    int16_t border_botton;
    int16_t border_left;
    int16_t block_size;
} Map;
typedef struct {
    int16_t score;
    int16_t pos_x_max;
    int16_t pos_y_max;
} snake_game_t;

Food         food       = {-1, -1, 1};
Snake        snake      = {MIN_LENGTH, NULL, NULL, SNAKE_RIGHT, 1};
Map          map        = {2, 128, 62, 12, 4};
snake_game_t snake_game = {0, 0, 0};

void greedySnake_cover_draw(int *draw_index)
{
    // edit these only !
    int snake_size            = 3;
    int snake_head_init_pos_x = 10;
    int snake_head_init_pos_y = 20;
    int border_top            = 8;
    int border_right          = 117;
    int border_left           = 15;
    int border_bottom         = 56;
    // border 的长宽最好都整除 snake_size
    // edit these only !

    int ver_step   = (border_right - border_left - snake_size) / snake_size;
    int hor_step   = (border_bottom - border_top - snake_size) / snake_size;
    int whole_step = (ver_step + hor_step) * 2;

    int floating_y[20] = {-5, -4, -3, -2, -1, 0, 1,  2,  3,  4,
                          5,  4,  3,  2,  1,  0, -1, -2, -3, -4};
    OLED_Clear();
    for (int len = 0; len < 6; len++) {
        int snake_body_init_pos_x = snake_head_init_pos_x;
        int snake_body_init_pos_y = snake_head_init_pos_y + snake_size * len;
        int snake_body_draw_index = ((*draw_index) % (whole_step + 1)) - len;
        if (snake_body_draw_index < 0)
            snake_body_draw_index += (whole_step + 1);

        if (snake_body_draw_index <
                ((snake_head_init_pos_y - border_top) / snake_size + 1) &&
            snake_body_draw_index >= 0) {
            snake_body_init_pos_x = border_left;
            snake_body_init_pos_y =
                snake_head_init_pos_y - snake_body_draw_index * snake_size;
        }
        if (snake_body_draw_index <
                (((snake_head_init_pos_y - border_top) +
                  (border_right - border_left - snake_size)) /
                     snake_size +
                 1) &&
            snake_body_draw_index >=
                ((snake_head_init_pos_y - border_top) / snake_size + 1)) {
            snake_body_init_pos_x =
                border_left +
                (snake_body_draw_index -
                 ((snake_head_init_pos_y - border_top) / snake_size + 1)) *
                    snake_size;
            snake_body_init_pos_y = border_top;
        }
        if (snake_body_draw_index <
                (((snake_head_init_pos_y - border_top) +
                  (border_right - border_left - snake_size) +
                  (border_bottom - border_top - snake_size)) /
                     snake_size +
                 1) &&
            snake_body_draw_index >=
                (((snake_head_init_pos_y - border_top) +
                  (border_right - border_left - snake_size)) /
                     snake_size +
                 1)) {
            snake_body_init_pos_x = (border_right - snake_size);
            snake_body_init_pos_y =
                border_top + (snake_body_draw_index -
                              (((snake_head_init_pos_y - border_top) +
                                (border_right - border_left - snake_size)) /
                                   snake_size +
                               1)) *
                                 snake_size;
        }
        if (snake_body_draw_index <
                (((snake_head_init_pos_y - border_top) +
                  (border_right - border_left - snake_size) * 2 +
                  (border_bottom - border_top - snake_size)) /
                     snake_size +
                 1) &&
            snake_body_draw_index >=
                (((snake_head_init_pos_y - border_top) +
                  (border_right - border_left - snake_size) +
                  (border_bottom - border_top - snake_size)) /
                     snake_size +
                 1)) {
            snake_body_init_pos_x =
                (border_right - snake_size) -
                (snake_body_draw_index -
                 (((snake_head_init_pos_y - border_top) +
                   (border_right - border_left - snake_size) +
                   (border_bottom - border_top - snake_size)) /
                      snake_size +
                  1)) *
                    snake_size;
            snake_body_init_pos_y = (border_bottom - snake_size);
        }
        if (snake_body_draw_index < (whole_step + 1) &&
            snake_body_draw_index >=
                (((snake_head_init_pos_y - border_top) +
                  (border_right - border_left - snake_size) * 2 +
                  (border_bottom - border_top - snake_size)) /
                     snake_size +
                 1)) {
            snake_body_init_pos_x = border_left;
            snake_body_init_pos_y =
                (border_bottom - snake_size) -
                (snake_body_draw_index -
                 (((snake_head_init_pos_y - border_top) +
                   (border_right - border_left - snake_size) * 2 +
                   (border_bottom - border_top - snake_size)) /
                      snake_size +
                  1)) *
                    snake_size;
        }

        // printf("%d snake_body_pos %d %d\n", len, snake_body_init_pos_x,
        // snake_body_init_pos_y);
        OLED_FillRect(snake_body_init_pos_x, snake_body_init_pos_y, snake_size,
                      snake_size, 1);
        OLED_Icon_Draw(36, 27 + floating_y[(*draw_index) % 20],
                       &img_SNAKE_61_10, 0);

        OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
        OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
    }
    OLED_Refresh_GRAM();
    ++(*draw_index);
    if ((*draw_index) == (whole_step + 1) * 20)
        (*draw_index) = 0;
    aos_msleep(60);
}

int greedySnake_init(void)
{
    snake_game.pos_x_max =
        (map.border_right - map.border_left) / map.block_size;
    snake_game.pos_y_max =
        (map.border_botton - map.border_top) / map.block_size;
    snake.XPos = (int16_t *)malloc(snake_game.pos_x_max * snake_game.pos_y_max *
                                   sizeof(int16_t));
    snake.YPos = (int16_t *)malloc(snake_game.pos_x_max * snake_game.pos_y_max *
                                   sizeof(int16_t));
    snake.length  = MIN_LENGTH;
    snake.cur_dir = SNAKE_RIGHT;
    snake.alive   = 1;

    snake_game.score = 0;

    food.eaten = 1;

    for (uint8_t i = 0; i < snake.length; i++) {
        snake.XPos[i] = snake_game.pos_x_max / 2 + i;
        snake.YPos[i] = snake_game.pos_y_max / 2;
    }

    OLED_Clear();
    draw_snake();
    gen_food();
    draw_food();
    OLED_Refresh_GRAM();

    aos_task_new_ext(&greedySnake_task_handle, "greedySnake_task", greedySnake_task, NULL, 1024, AOS_DEFAULT_APP_PRI);
    printf("aos_task_new greedySnake_task\n");
    return 0;
}

void draw_snake()
{
    uint16_t i = 0;
    OLED_Icon_Draw(map.border_left + snake.XPos[i] * map.block_size,
                   map.border_top + snake.YPos[i] * map.block_size,
                   &icon_snake0_4_4, 0);
    for (; i < snake.length - 2; i++) {
        OLED_Icon_Draw(map.border_left + snake.XPos[i] * map.block_size,
                       map.border_top + snake.YPos[i] * map.block_size,
                       ((i % 2) ? &icon_snake1_4_4 : &icon_snake0_4_4), 0);
    }
    OLED_Icon_Draw(map.border_left + snake.XPos[i] * map.block_size,
                   map.border_top + snake.YPos[i] * map.block_size,
                   &icon_snake1_4_4, 0);
}

void gen_food()
{
    int i = 0;
    if (food.eaten == 1) {
        while (1) {
            food.x = rand() % (snake_game.pos_x_max - 6) + 3;
            food.y = rand() % (snake_game.pos_y_max - 6) + 3;

            for (i = 0; i < snake.length; i++) {
                if ((food.x == snake.XPos[i]) && (food.y == snake.YPos[i]))
                    break;
            }
            if (i == snake.length) {
                food.eaten = 0;
                break;
            }
        }
    }
}

void draw_food()
{
    if (food.eaten == 0) {
        OLED_Icon_Draw(map.border_left + food.x * map.block_size,
                       map.border_top + food.y * map.block_size, &icon_food_4_4,
                       0);
    }
}

void greedySnake_task(void)
{
    while (1) {
        if (snake.alive) {
            OLED_Clear();

            OLED_Icon_Draw(3, 41, &icon_scores_5_21, 0);
            OLED_DrawRect(11, 1, 118, 62, 1);
            draw_score(snake_game.score);
            Snake_Run(snake.cur_dir);
            OLED_Refresh_GRAM();

            aos_msleep(200);
        } else {
            OLED_Clear();
            OLED_Show_String(30, 12, "GAME OVER", 16, 1);
            OLED_Show_String(10, 40, "press K1&K2 to quit", 12, 1);
            OLED_Refresh_GRAM();
            aos_msleep(500);
        }
    }
}

int greedySnake_uninit(void)
{
    free(snake.YPos);
    free(snake.XPos);
    aos_task_delete(&greedySnake_task_handle);
    printf("aos_task_delete greedySnake_task\n");
    return 0;
}

void greedySnake_key_handel(key_code_t key_code) { Snake_Run(key_code); }

void check_snake_alive()
{
    if (snake.XPos[snake.length - 1] < 0 ||
        snake.XPos[snake.length - 1] >= snake_game.pos_x_max ||
        snake.YPos[snake.length - 1] < 0 ||
        snake.YPos[snake.length - 1] >= snake_game.pos_y_max) {
        snake.alive = 0;
    }
    for (int i = 0; i < snake.length - 1; i++) {
        if (snake.XPos[snake.length - 1] == snake.XPos[i] &&
            snake.YPos[snake.length - 1] == snake.YPos[i]) {
            snake.alive = 0;
            break;
        }
    }
}

void Snake_Run(uint8_t dir)
{
    uint16_t i;

    switch (dir) {
        case SNAKE_RIGHT:
            if (snake.cur_dir != SNAKE_LEFT) {
                for (i = 0; i < snake.length - 1; i++) {
                    snake.XPos[i] = snake.XPos[i + 1];
                    snake.YPos[i] = snake.YPos[i + 1];
                }
                snake.XPos[snake.length - 1] = snake.XPos[snake.length - 2] + 1;
                snake.YPos[snake.length - 1] = snake.YPos[snake.length - 2] - 0;
                snake.cur_dir                = dir;
            }
            break;
        case SNAKE_LEFT:
            if (snake.cur_dir != SNAKE_RIGHT) {
                for (i = 0; i < snake.length - 1; i++) {
                    snake.XPos[i] = snake.XPos[i + 1];
                    snake.YPos[i] = snake.YPos[i + 1];
                }
                snake.XPos[snake.length - 1] = snake.XPos[snake.length - 2] - 1;
                snake.YPos[snake.length - 1] = snake.YPos[snake.length - 2] + 0;
                snake.cur_dir                = dir;
            }
            break;
        case SNAKE_DOWN:
            if (snake.cur_dir != SNAKE_UP) {
                for (i = 0; i < snake.length - 1; i++) {
                    snake.XPos[i] = snake.XPos[i + 1];
                    snake.YPos[i] = snake.YPos[i + 1];
                }
                snake.XPos[snake.length - 1] = snake.XPos[snake.length - 2] + 0;
                snake.YPos[snake.length - 1] = snake.YPos[snake.length - 2] + 1;
                snake.cur_dir                = dir;
            }
            break;
        case SNAKE_UP:
            if (snake.cur_dir != SNAKE_DOWN) {
                for (i = 0; i < snake.length - 1; i++) {
                    snake.XPos[i] = snake.XPos[i + 1];
                    snake.YPos[i] = snake.YPos[i + 1];
                }
                snake.XPos[snake.length - 1] = snake.XPos[snake.length - 2] + 0;
                snake.YPos[snake.length - 1] = snake.YPos[snake.length - 2] - 1;
                snake.cur_dir                = dir;
            }
            break;

        default:
            break;
    }

    check_snake_alive();
    check_food_eaten();
    draw_snake();
    draw_food();
}

void check_food_eaten()
{
    if (snake.XPos[snake.length - 1] == food.x &&
        snake.YPos[snake.length - 1] == food.y) {
        snake.length++;
        snake.XPos[snake.length - 1] = food.x;
        snake.YPos[snake.length - 1] = food.y;
        snake_game.score++;
        food.eaten = 1;
        gen_food();
    }
}

icon_t *get_icon_num_5_3(uint8_t num)
{
    switch (num) {
        case 0:
            return &icon_0_5_3;
            break;
        case 1:
            return &icon_1_5_3;
            break;
        case 2:
            return &icon_2_5_3;
            break;
        case 3:
            return &icon_3_5_3;
            break;
        case 4:
            return &icon_4_5_3;
            break;
        case 5:
            return &icon_5_5_3;
            break;
        case 6:
            return &icon_6_5_3;
            break;
        case 7:
            return &icon_7_5_3;
            break;
        case 8:
            return &icon_8_5_3;
            break;
        case 9:
            return &icon_9_5_3;
            break;
        default:
            break;
    }
}

void draw_score(uint16_t score)
{
    OLED_Icon_Draw(3, 35, get_icon_num_5_3((uint8_t)(score / 100)), 0);
    OLED_Icon_Draw(3, 31, get_icon_num_5_3((uint8_t)((score % 100) / 10)), 0);
    OLED_Icon_Draw(3, 27, get_icon_num_5_3((uint8_t)(score % 10)), 0);
}
