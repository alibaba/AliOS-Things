#include "aircraftBattle.h"
#include <stdio.h>
#include <stdlib.h>

MENU_COVER_TYP aircraftBattle_cover = {MENU_COVER_FUNC, NULL, NULL,
                                       aircraftBattle_cover_draw};
MENU_TASK_TYP  aircraftBattle_tasks = {aircraftBattle_init,
                                      aircraftBattle_uninit};
MENU_TYP       aircraftBattle       = {"aircraftBattle", &aircraftBattle_cover,
                           &aircraftBattle_tasks, aircraftBattle_key_handel,
                           NULL};

static aos_task_t aircraftBattle_task_handle;

#define MAX_L_CRAFT 1
#define MAX_M_CRAFT 5
#define MAX_S_CRAFT 10
#define MAX_BULLET  30
#define MY_CHANCE   3
#define AUTO_RELOAD -1024

uint8_t g_chance = MY_CHANCE;

dfo_t *my_craft;
dfo_t **bullet_group;

dfo_t **enemy_crafts;

static map_t  achilles_normal0_map       = {&icon_Mycraft0_19_19, -9, -9};
static map_t  achilles_normal1_map       = {&icon_Mycraft1_19_19, -9, -9};
static map_t  achilles_normal2_map       = {&icon_Mycraft2_19_19, -9, -9};
static map_t *achilles_normal_maplist[3] = {
    &achilles_normal0_map, &achilles_normal1_map, &achilles_normal2_map};

static map_t  achilles_destory0_map = {&icon_Mycraft_destory0_19_19, -9, -9};
static map_t  achilles_destory1_map = {&icon_Mycraft_destory1_19_19, -9, -9};
static map_t  achilles_destory2_map = {&icon_Mycraft_destory2_16_16, -7, -7};
static map_t *achilles_destory_maplist[3] = {
    &achilles_destory0_map, &achilles_destory1_map, &achilles_destory2_map};

static arms_t  achilles_arms0        = {-6, -3};
static arms_t  achilles_arms1        = {6, -3};
static arms_t *achilles_arms_list[2] = {&achilles_arms0, &achilles_arms1};

static map_t  titan_normal0_map       = {&icon_Lcraft_32_32, -15, -15};
static map_t *titan_normal_maplist[1] = {&titan_normal0_map};

static map_t  titan_destory0_map = {&icon_Lcraft_destory0_32_32, -15, -15};
static map_t  titan_destory1_map = {&icon_Lcraft_destory1_32_32, -15, -15};
static map_t  titan_destory2_map = {&icon_Lcraft_destory2_32_32, -15, -15};
static map_t *titan_destory_maplist[3] = {
    &titan_destory0_map, &titan_destory1_map, &titan_destory2_map};

static map_t  ares_normal0_map       = {&icon_Mcraft_12_12, -5, -5};
static map_t *ares_normal_maplist[1] = {&ares_normal0_map};

static map_t  ares_destory0_map       = {&icon_Mcraft_destory0_12_12, -5, -5};
static map_t  ares_destory1_map       = {&icon_Mcraft_destory1_12_12, -5, -5};
static map_t  ares_destory2_map       = {&icon_Mcraft_destory2_12_12, -5, -5};
static map_t *ares_destory_maplist[3] = {&ares_destory0_map, &ares_destory1_map,
                                         &ares_destory2_map};

static map_t  venture_normal0_map       = {&icon_Scraft_7_8, -3, -3};
static map_t *venture_normal_maplist[1] = {&venture_normal0_map};

static map_t  venture_destory0_map       = {&icon_Scraft_destory0_9_8, -4, -4};
static map_t  venture_destory1_map       = {&icon_Scraft_destory1_9_8, -4, -4};
static map_t  venture_destory2_map       = {&icon_Scraft_destory2_7_6, -3, -3};
static map_t *venture_destory_maplist[3] = {
    &venture_destory0_map, &venture_destory1_map, &venture_destory2_map};

static map_t  bullet_map        = {&icon_bullet_2_1, 0, -1};
static map_t *bullet_maplist[1] = {&bullet_map};

void aircraftBattle_cover_draw(int *draw_index)
{
    OLED_Clear();

    OLED_Icon_Draw(0, 0, &img_craft_bg_132_64, 1);

    for (int i = 0; i < random() % 20; i++) {
        OLED_DrawPoint(random() % 132, random() % 64, 1);
    }

    for (int i = 0; i < random() % 10; i++) {
        OLED_Icon_Draw(random() % 132, random() % 64, &icon_mini_start_3_3, 0);
    }

    OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
    OLED_Icon_Draw(122, 24, &icon_skip_right, 0);

    OLED_Refresh_GRAM();
    aos_msleep(800);
    return 0;
}

dfo_t *create_achilles()
{
    dfo_t *achilles = (dfo_t *)malloc(sizeof(dfo_t));
    achilles->act_seq_list_len = 2;

    act_seq_t *achilles_normal_act    = (act_seq_t *)malloc(sizeof(act_seq_t));
    achilles_normal_act->act_seq_maps = achilles_normal_maplist;
    achilles_normal_act->act_seq_len  = 3;
    achilles_normal_act->act_seq_interval = 10;
    achilles_normal_act->act_is_destory   = 0;

    act_seq_t *achilles_destory_act    = (act_seq_t *)malloc(sizeof(act_seq_t));
    achilles_destory_act->act_seq_maps = achilles_destory_maplist;
    achilles_destory_act->act_seq_len  = 3;
    achilles_destory_act->act_seq_interval = 4;
    achilles_destory_act->act_is_destory   = 1;

    act_seq_t **achilles_act_seq_list =
        (act_seq_t **)malloc(sizeof(act_seq_t *) * achilles->act_seq_list_len);
    achilles_act_seq_list[0] = achilles_normal_act;
    achilles_act_seq_list[1] = achilles_destory_act;

    achilles->model = Achilles;

    achilles->speed = 4;
    achilles->range = -1;

    achilles->act_seq_list = achilles_act_seq_list;

    achilles->act_seq_type = 0;

    achilles->damage    = 8;
    achilles->full_life = 10;
    achilles->cur_life  = 10;

    achilles->arms_list_len = 2;
    achilles->arms_list     = achilles_arms_list;

    reload_dfo(achilles, 31, 116);

    return achilles;
}

dfo_t *create_titan()
{
    dfo_t *titan            = (dfo_t *)malloc(sizeof(dfo_t));
    titan->act_seq_list_len = 2;

    act_seq_t *titan_normal_act        = (act_seq_t *)malloc(sizeof(act_seq_t));
    titan_normal_act->act_seq_maps     = titan_normal_maplist;
    titan_normal_act->act_seq_len      = 1;
    titan_normal_act->act_seq_interval = 0;
    titan_normal_act->act_is_destory   = 0;

    act_seq_t *titan_destory_act    = (act_seq_t *)malloc(sizeof(act_seq_t));
    titan_destory_act->act_seq_maps = titan_destory_maplist;
    titan_destory_act->act_seq_len  = 3;
    titan_destory_act->act_seq_interval = 4;
    titan_destory_act->act_is_destory   = 1;

    act_seq_t **titan_act_seq_list =
        (act_seq_t **)malloc(sizeof(act_seq_t *) * titan->act_seq_list_len);
    titan_act_seq_list[0] = titan_normal_act;
    titan_act_seq_list[1] = titan_destory_act;

    titan->model = TiTan;

    titan->speed = 1;
    titan->range = -1;

    titan->act_seq_list = titan_act_seq_list;

    titan->act_seq_type = 0;

    titan->damage    = 100;
    titan->full_life = 60;
    titan->cur_life  = 60;

    titan->arms_list_len = 0;
    titan->arms_list     = NULL;

    reload_dfo(titan, AUTO_RELOAD, AUTO_RELOAD);

    return titan;
}

dfo_t *create_ares()
{
    dfo_t *ares            = (dfo_t *)malloc(sizeof(dfo_t));
    ares->act_seq_list_len = 2;

    act_seq_t *ares_normal_act        = (act_seq_t *)malloc(sizeof(act_seq_t));
    ares_normal_act->act_seq_maps     = ares_normal_maplist;
    ares_normal_act->act_seq_len      = 1;
    ares_normal_act->act_seq_interval = 0;
    ares_normal_act->act_is_destory   = 0;

    act_seq_t *ares_destory_act        = (act_seq_t *)malloc(sizeof(act_seq_t));
    ares_destory_act->act_seq_maps     = ares_destory_maplist;
    ares_destory_act->act_seq_len      = 3;
    ares_destory_act->act_seq_interval = 3;
    ares_destory_act->act_is_destory   = 1;

    act_seq_t **ares_act_seq_list =
        (act_seq_t **)malloc(sizeof(act_seq_t *) * ares->act_seq_list_len);
    ares_act_seq_list[0] = ares_normal_act;
    ares_act_seq_list[1] = ares_destory_act;

    ares->model = Ares;

    ares->speed = 2;
    ares->range = -1;

    ares->act_seq_list = ares_act_seq_list;

    ares->act_seq_type = 0;

    ares->damage    = 8;
    ares->full_life = 10;
    ares->cur_life  = 10;

    ares->arms_list_len = 0;
    ares->arms_list     = NULL;

    reload_dfo(ares, AUTO_RELOAD, AUTO_RELOAD);

    return ares;
}

dfo_t *create_venture()
{
    dfo_t *venture            = (dfo_t *)malloc(sizeof(dfo_t));
    venture->act_seq_list_len = 2;

    act_seq_t *venture_normal_act    = (act_seq_t *)malloc(sizeof(act_seq_t));
    venture_normal_act->act_seq_maps = venture_normal_maplist;
    venture_normal_act->act_seq_len  = 1;
    venture_normal_act->act_seq_interval = 0;
    venture_normal_act->act_is_destory   = 0;

    act_seq_t *venture_destory_act    = (act_seq_t *)malloc(sizeof(act_seq_t));
    venture_destory_act->act_seq_maps = venture_destory_maplist;
    venture_destory_act->act_seq_len  = 3;
    venture_destory_act->act_seq_interval = 2;
    venture_destory_act->act_is_destory   = 1;

    act_seq_t **venture_act_seq_list =
        (act_seq_t **)malloc(sizeof(act_seq_t *) * venture->act_seq_list_len);
    venture_act_seq_list[0] = venture_normal_act;
    venture_act_seq_list[1] = venture_destory_act;

    venture->model = Venture;

    venture->speed = 3;
    venture->range = -1;

    venture->act_seq_list = venture_act_seq_list;

    venture->act_seq_type = 0;

    venture->damage    = 5;
    venture->full_life = 2;
    venture->cur_life  = 2;

    venture->arms_list_len = 0;
    venture->arms_list     = NULL;

    reload_dfo(venture, AUTO_RELOAD, AUTO_RELOAD);

    return venture;
}

dfo_t *create_bullet()
{
    dfo_t *bullet            = (dfo_t *)malloc(sizeof(dfo_t));
    bullet->act_seq_list_len = 1;

    act_seq_t *bullet_normal_act    = (act_seq_t *)malloc(sizeof(act_seq_t));
    bullet_normal_act->act_seq_maps = bullet_maplist;
    bullet_normal_act->act_seq_len  = 1;
    bullet_normal_act->act_seq_interval = 0;
    bullet_normal_act->act_is_destory   = 0;

    act_seq_t **bullet_act_seq_list =
        (act_seq_t **)malloc(sizeof(act_seq_t *) * bullet->act_seq_list_len);
    bullet_act_seq_list[0] = bullet_normal_act;

    bullet->model = Bullet;

    bullet->speed = 10;
    bullet->range = 120;

    bullet->act_seq_list = bullet_act_seq_list;

    bullet->act_seq_type = 0;

    bullet->damage    = 1;
    bullet->full_life = 1;
    bullet->cur_life  = 0;

    bullet->arms_list_len = 0;
    bullet->arms_list     = NULL;

    bullet->start_x = -100;
    bullet->start_y = -100;
    bullet->cur_x   = -100;
    bullet->cur_y   = -100;

    return bullet;
}

void free_dfo(dfo_t *dfo)
{
    for (int i = 0; i < dfo->act_seq_list_len; i++) {
        free(dfo->act_seq_list[i]);
    }
    free(dfo->act_seq_list);
    free(dfo);
}

void global_create(void)
{
    my_craft = create_achilles();

    bullet_group = (dfo_t **)malloc(sizeof(dfo_t *) * MAX_BULLET);
    for (int i = 0; i < MAX_BULLET; i++) {
        bullet_group[i] = create_bullet();
    }

    enemy_crafts = (dfo_t **)malloc(sizeof(dfo_t *) *
                                    (MAX_L_CRAFT + MAX_M_CRAFT + MAX_S_CRAFT));
    for (int i = 0; i < MAX_L_CRAFT + MAX_M_CRAFT + MAX_S_CRAFT; i++) {
        if (i < MAX_L_CRAFT)
            enemy_crafts[i] = create_titan();
        else if (i < MAX_L_CRAFT + MAX_M_CRAFT && i >= MAX_L_CRAFT)
            enemy_crafts[i] = create_ares();
        else if (i < MAX_L_CRAFT + MAX_M_CRAFT + MAX_S_CRAFT &&
                 i >= MAX_L_CRAFT + MAX_M_CRAFT)
            enemy_crafts[i] = create_venture();
    }
}

void aircraftBattle_key_handel(key_code_t key_code)
{
    switch (key_code) {
        case 0b1000:
            move_MyCraft(my_craft, LEFT);
            break;
        case 0b0001:
            move_MyCraft(my_craft, UP);
            break;
        case 0b0100:
            move_MyCraft(my_craft, DOWN);
            break;
        case 0b0010:
            move_MyCraft(my_craft, RIGHT);
            break;
        default:
            break;
    }
}

map_t *get_cur_map(dfo_t *craft)
{
    act_seq_t *cur_act_seq = craft->act_seq_list[craft->act_seq_type];
    map_t *cur_map = cur_act_seq->act_seq_maps[cur_act_seq->act_seq_index];
    return cur_map;
}

void reload_dfo(dfo_t *craft, int pos_x, int pos_y)
{
    for (int i = 0; i < craft->act_seq_list_len; i++) {
        craft->act_seq_list[i]->act_seq_index        = 0;
        craft->act_seq_list[i]->act_seq_interval_cnt = 0;
    }

    craft->start_x = pos_x;
    craft->start_y = pos_y;

    if (pos_x == AUTO_RELOAD) {
        uint16_t height = get_cur_map(craft)->icon->width;
        craft->start_x  = random() % (64 - height) + height / 2;
    }
    if (pos_y == AUTO_RELOAD) {
        uint16_t width = get_cur_map(craft)->icon->height;
        craft->start_y = -(random() % 1000) - width / 2;
    }

    if (craft->model != Bullet)
        LOGI(EDU_TAG, "reset craft %d at %d,%d\n", craft->model, craft->start_x,
               craft->start_y);

    craft->cur_x    = craft->start_x;
    craft->cur_y    = craft->start_y;
    craft->cur_life = craft->full_life;

    craft->act_seq_type = 0;
}

void destory(dfo_t *craft)
{
    craft->act_seq_type = 1;
    craft->cur_life     = 0;
}

void move_enemy(dfo_t *craft)
{
    if (craft->cur_life <= 0)
        return;
    map_t *cur_map = get_cur_map(craft);
    craft->cur_y += craft->speed;
    int top = craft->cur_y + cur_map->offset_y;
    if (top > 132)
        reload_dfo(craft, AUTO_RELOAD, AUTO_RELOAD);
}

void move_MyCraft(dfo_t *my_craft, my_craft_dir_e_t dir)
{
    if (my_craft->cur_life <= 0)
        return;
    map_t *cur_map = get_cur_map(my_craft);
    int    top     = my_craft->cur_y + cur_map->offset_y;
    int    bottom  = my_craft->cur_y + cur_map->offset_y + cur_map->icon->width;

    int left  = my_craft->cur_x + cur_map->offset_x;
    int right = my_craft->cur_x + cur_map->offset_x + cur_map->icon->height;

    switch (dir) {
        case UP:
            if (!(top - my_craft->speed < 0))
                my_craft->cur_y -= my_craft->speed;
            break;
        case DOWN:
            if (!(bottom + my_craft->speed > 132))
                my_craft->cur_y += my_craft->speed;
            break;
        case LEFT:
            if (!(left - my_craft->speed < 0))
                my_craft->cur_x -= my_craft->speed;
            break;
        case RIGHT:
            if (!(right + my_craft->speed > 64))
                my_craft->cur_x += my_craft->speed;
            break;
        default:
            break;
    }
}

void move_bullet(dfo_t *bullet)
{
    if (bullet->cur_life <= 0)
        return;
    map_t *cur_map = get_cur_map(bullet);
    bullet->cur_y -= bullet->speed;
    int bottom = bullet->cur_y + cur_map->offset_y + cur_map->icon->width;
    if (bottom < 0 || (bullet->start_y - bullet->cur_y) > bullet->range) {
        bullet->cur_life = 0;
        bullet->cur_x    = -100;
    }
}

dfo_t *get_deactived_bullet()
{
    for (int i = 0; i < MAX_BULLET; i++) {
        if (bullet_group[i]->cur_life <= 0)
            return bullet_group[i];
    }
    return NULL;
}

void shut_craft(dfo_t *craft)
{
    if (craft->cur_life <= 0)
        return;

    if (craft->arms_list == NULL || craft->arms_list_len == 0)
        return;

    for (int i = 0; i < craft->arms_list_len; i++) {
        dfo_t *bullet = get_deactived_bullet();
        if (bullet == NULL)
            return;
        reload_dfo(bullet, craft->cur_x + craft->arms_list[i]->offset_x,
                   craft->cur_y + craft->arms_list[i]->offset_y);
    }
}

void draw_dfo(dfo_t *dfo)
{
    map_t *cur_map = get_cur_map(dfo);
    int    top     = dfo->cur_y + cur_map->offset_y;
    int    bottom  = dfo->cur_y + cur_map->offset_y + cur_map->icon->width;

    int left  = dfo->cur_x + cur_map->offset_x;
    int right = dfo->cur_x + cur_map->offset_x + cur_map->icon->height;
    if (top > 132 || bottom < 0 || left > 64 || right < 0)
        return;

    OLED_Icon_Draw(dfo->cur_y + cur_map->offset_y,
                   64 -
                       (dfo->cur_x + cur_map->offset_x + cur_map->icon->height),
                   cur_map->icon, 2);
}

void craft_update_act(dfo_t *craft)
{
    act_seq_t *cur_act_seq = craft->act_seq_list[craft->act_seq_type];
    if (cur_act_seq->act_seq_interval == 0)
        return;
    ++(cur_act_seq->act_seq_interval_cnt);
    if (cur_act_seq->act_seq_interval_cnt >= cur_act_seq->act_seq_interval) {
        cur_act_seq->act_seq_interval_cnt = 0;
        ++(cur_act_seq->act_seq_index);
        if (cur_act_seq->act_seq_index >= cur_act_seq->act_seq_len) {
            cur_act_seq->act_seq_index = 0;
            if (cur_act_seq->act_is_destory == 1) {
                switch (craft->model) {
                    case Achilles:
                        reload_dfo(craft, 31, 116);
                        break;
                    case Venture:
                    case Ares:
                    case TiTan:
                        reload_dfo(craft, AUTO_RELOAD, AUTO_RELOAD);
                        break;
                    case Bullet:
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void global_update(void)
{
    for (int i = 0; i < MAX_L_CRAFT + MAX_M_CRAFT + MAX_S_CRAFT; i++) {
        craft_update_act(enemy_crafts[i]);
        move_enemy(enemy_crafts[i]);
    }
    for (int i = 0; i < MAX_BULLET; i++) {
        move_bullet(bullet_group[i]);
    }
    craft_update_act(my_craft);
    shut_craft(my_craft);
    global_hit_check();
}

void global_draw(void)
{
    for (int i = 0; i < MAX_L_CRAFT + MAX_M_CRAFT + MAX_S_CRAFT; i++) {
        draw_dfo(enemy_crafts[i]);
    }
    for (int i = 0; i < MAX_BULLET; i++) {
        draw_dfo(bullet_group[i]);
    }
    draw_dfo(my_craft);
}

int hit_check(dfo_t *bullet, dfo_t *craft)
{
    if (craft->cur_y <= 0 || craft->cur_x <= 0)
        return 0;
    if (craft->cur_life <= 0)
        return 0;
    if (bullet->cur_life <= 0)
        return 0;
    act_seq_t *cur_act_seq = bullet->act_seq_list[bullet->act_seq_type];
    map_t *cur_map = cur_act_seq->act_seq_maps[cur_act_seq->act_seq_index];

    for (int bullet_bit_x = 0; bullet_bit_x < (cur_map->icon->height);
         bullet_bit_x++) {
        for (int bullet_bit_y = 0; bullet_bit_y < (cur_map->icon->width);
             bullet_bit_y++) {
            uint8_t bit =
                (cur_map->icon->p_icon_mask == NULL) ?
                    cur_map->icon
                            ->p_icon_data[bullet_bit_x / 8 + bullet_bit_y] &
                        (0x01 << bullet_bit_x % 8) :
                    cur_map->icon
                            ->p_icon_mask[bullet_bit_x / 8 + bullet_bit_y] &
                        (0x01 << bullet_bit_x % 8);
            if (bit == 0)
                continue;

            int bit_cur_x = bullet->cur_x + cur_map->offset_x +
                            cur_map->icon->height - bullet_bit_x;
            int bit_cur_y = bullet->cur_y + cur_map->offset_y + bullet_bit_y;

            act_seq_t *cur_craft_act_seq =
                craft->act_seq_list[craft->act_seq_type];
            map_t *cur_craft_map =
                cur_craft_act_seq
                    ->act_seq_maps[cur_craft_act_seq->act_seq_index];

            for (int craft_bit_x = 0;
                 craft_bit_x < (cur_craft_map->icon->height); craft_bit_x++) {
                for (int craft_bit_y = 0;
                     craft_bit_y < (cur_craft_map->icon->width);
                     craft_bit_y++) {
                    uint8_t craft_bit =
                        (cur_craft_map->icon->p_icon_mask == NULL) ?
                            cur_craft_map->icon->p_icon_data[craft_bit_x / 8 +
                                                             craft_bit_y] &
                                (0x01 << craft_bit_x % 8) :
                            cur_craft_map->icon->p_icon_mask[craft_bit_x / 8 +
                                                             craft_bit_y] &
                                (0x01 << craft_bit_x % 8);
                    if (craft_bit == 0)
                        continue;
                    // 找到有效点对应的绝对坐标
                    int craft_bit_cur_x =
                        craft->cur_x + cur_craft_map->offset_x +
                        cur_craft_map->icon->height - craft_bit_x;
                    int craft_bit_cur_y =
                        craft->cur_y + cur_craft_map->offset_y + craft_bit_y;
                    // 开始遍历所有可撞击对象
                    if (craft_bit_cur_x == bit_cur_x &&
                        craft_bit_cur_y == bit_cur_y) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

void global_hit_check(void)
{
    // 子弹撞击检测
    for (int j = 0; j < MAX_BULLET; j++) {
        dfo_t *bullet = bullet_group[j];
        if (bullet->cur_life <= 0)
            continue;

        for (int i = 0; i < MAX_L_CRAFT + MAX_M_CRAFT + MAX_S_CRAFT; i++) {
            dfo_t *craft = enemy_crafts[i];
            if (craft->cur_life <= 0)
                continue;

            if (hit_check(bullet, craft)) {
                craft->cur_life -= bullet->damage;
                bullet->cur_life = 0;
                bullet->cur_x    = -100;
                if (craft->cur_life <= 0) {
                    destory(craft);
                }
                continue;
            }
        }
    }

    // 我方飞机撞击检测
    for (int i = 0; i < MAX_L_CRAFT + MAX_M_CRAFT + MAX_S_CRAFT; i++) {
        dfo_t *craft = enemy_crafts[i];
        if (craft->cur_life <= 0)
            continue;

        if (hit_check(my_craft, craft)) {
            craft->cur_life -= my_craft->damage;
            my_craft->cur_life -= craft->damage;
            if (craft->cur_life <= 0) {
                destory(craft);
            }
            if (my_craft->cur_life <= 0) {
                destory(my_craft);
                g_chance--;
            }
            continue;
        }
    }
}

int aircraftBattle_init(void)
{
    OLED_Clear();
    OLED_Refresh_GRAM();
    global_create();
    g_chance = MY_CHANCE;
    aos_task_new_ext(&aircraftBattle_task_handle, "aircraftBattle_task", aircraftBattle_task, NULL, 1024, AOS_DEFAULT_APP_PRI);
    LOGI(EDU_TAG, "aos_task_new aircraftBattle_task\n");
    return 0;
}

int aircraftBattle_uninit(void)
{
    free_dfo(my_craft);
    for (int i = 0; i < MAX_L_CRAFT + MAX_M_CRAFT + MAX_S_CRAFT; i++)
        free_dfo(enemy_crafts[i]);
    for (int i = 0; i < MAX_BULLET; i++)
        free_dfo(bullet_group[i]);

    aos_task_delete(&aircraftBattle_task_handle);
    LOGI(EDU_TAG, "aos_task_delete aircraftBattle_task\n");
    return 0;
}

void aircraftBattle_task()
{
    while (1) {
        if (g_chance > 0) {
            OLED_Clear();
            global_update();
            global_draw();
            OLED_Refresh_GRAM();
            aos_msleep(40);
        } else {
            OLED_Clear();
            OLED_Show_String(30, 12, "GAME OVER", 16, 1);
            OLED_Show_String(10, 40, "press K1&K2 to quit", 12, 1);
            OLED_Refresh_GRAM();
            aos_msleep(1000);
        }
    }
}
