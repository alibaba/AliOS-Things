/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "aircraftBattle.h"
#include "hal_oled.h"

MENU_COVER_TYP aircraftBattle_cover = {MENU_COVER_FUNC, NULL, NULL, aircraftBattle_cover_draw};
MENU_TASK_TYP aircraftBattle_tasks = {
    aircraftBattle_init,
    aircraftBattle_uninit};
MENU_TYP aircraftBattle = {
    "aircraftBattle",
    &aircraftBattle_cover,
    &aircraftBattle_tasks,
    aircraftBattle_key_handel,
    NULL};

#define ABS(X) ((X) >= 0 ? (X) : (-X))
#define MAX_L_CRAFT 1
#define MAX_L_CRAFT 1
#define MAX_M_CRAFT 2
#define MAX_S_CRAFT 5
#define MAX_BULLET 30
#define MY_CHANCE 3

key_code_t g_key_code;
uint8_t g_chance = MY_CHANCE;

plane_t *l_craft_group[MAX_L_CRAFT];
plane_t *m_craft_group[MAX_M_CRAFT];
plane_t *s_craft_group[MAX_S_CRAFT];
bullet_t *bullet_group[MAX_BULLET];

//
map_t my_craft_normal0_map = {&icon_Mycraft0_19_19, -9, -9};
map_t my_craft_normal1_map = {&icon_Mycraft1_19_19, -9, -9};
map_t my_craft_normal2_map = {&icon_Mycraft2_19_19, -9, -9};
map_t *my_craft_normal_maplist[] = {&my_craft_normal0_map, &my_craft_normal1_map, &my_craft_normal2_map};
act_seq_t my_craft_normal_act = {my_craft_normal_maplist, 3, 10, 0, 0};

map_t my_craft_destory0_map = {&icon_Mycraft_destory0_19_19, -9, -9};
map_t my_craft_destory1_map = {&icon_Mycraft_destory1_19_19, -9, -9};
map_t my_craft_destory2_map = {&icon_Mycraft_destory2_16_16, -7, -7};
map_t *my_craft_destory_maplist[] = {&my_craft_destory0_map, &my_craft_destory1_map, &my_craft_destory2_map};
act_seq_t my_craft_destory_act = {my_craft_destory_maplist, 3, 4, 1, reset_MyCraft};

act_seq_t *my_craft_act_seq[] = {&my_craft_normal_act, &my_craft_destory_act};
plane_t my_craft = {31, 116, my_craft_act_seq, 2, 0, 4, 1, 1, 1};

//
map_t l_craft_normal0_map = {&icon_Lcraft_32_32, -15, -15};
map_t *l_craft_normal_maplist[] = {&l_craft_normal0_map};
act_seq_t l_craft_normal_act = {l_craft_normal_maplist, 1, 0, 0, 0};

map_t l_craft_destory0_map = {&icon_Lcraft_destory0_32_32, -15, -15};
map_t l_craft_destory1_map = {&icon_Lcraft_destory1_32_32, -15, -15};
map_t l_craft_destory2_map = {&icon_Lcraft_destory2_32_32, -15, -15};
map_t *l_craft_destory_maplist[] = {&l_craft_destory0_map, &l_craft_destory1_map, &l_craft_destory2_map};
act_seq_t l_craft_destory_act = {l_craft_destory_maplist, 3, 4, 1, reset_Lcraft};

act_seq_t *l_craft_act_seq[] = {&l_craft_normal_act, &l_craft_destory_act};
plane_t g_l_craft = {-100, -100, l_craft_act_seq, 2, 0, 1, 100, 60, 0};

//
map_t m_craft_normal0_map = {&icon_Mcraft_12_12, -5, -5};
map_t *m_craft_normal_maplist[] = {&m_craft_normal0_map};
act_seq_t m_craft_normal_act = {m_craft_normal_maplist, 1, 0, 0, 0};

act_seq_t *m_craft_act_seq[] = {&m_craft_normal_act};
plane_t g_m_craft = {-100, -100, m_craft_act_seq, 1, 0, 2, 10, 10};

//
map_t s_craft_normal0_map = {&icon_Scraft_7_8, -3, -3};
map_t *s_craft_normal_maplist[] = {&s_craft_normal0_map};
act_seq_t s_craft_normal_act = {s_craft_normal_maplist, 1, 0, 0, 0};

act_seq_t *s_craft_act_seq[] = {&s_craft_normal_act};
plane_t g_s_craft = {-100, -100, s_craft_act_seq, 1, 0, 3, 2, 2};

//
map_t bullet_map = {&icon_bullet_2_1, 0, -1};
map_t *bullet_maplist[] = {&bullet_map};
act_seq_t bullet_act = {bullet_maplist, 1, 0, 0, 0};

act_seq_t *bullet_act_seq[] = {&bullet_act};
bullet_t g_bullet = {-100, -100, bullet_act_seq, 1, 0, 10, 1, 1, 0, 120};

void aircraftBattle_cover_draw(int *draw_index)
{
    // int star_num_max = random() % 20;
    OLED_Clear();

    OLED_Icon_Draw(0, 0, &img_craft_bg_132_64, 1);

    for (int i = 0; i < random() % 20; i++)
    {
        OLED_DrawPoint(random() % 132, random() % 64, 1);
    }

    for (int i = 0; i < random() % 10; i++)
    {
        OLED_Icon_Draw(random() % 132, random() % 64, &icon_mini_start_3_3, 0);
    }

    OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
    OLED_Icon_Draw(122, 24, &icon_skip_right, 0);

    OLED_Refresh_GRAM();
    aos_msleep(800);
    return 0;
}

void aircraftBattle_key_handel(key_code_t key_code)
{
    g_key_code = key_code;
    my_key_handel(g_key_code);
}

void my_key_handel(key_code_t key_code)
{
    switch (key_code)
    {
    case 0b1000:
        move_MyCraft(&my_craft, LEFT);
        break;
    case 0b0001:
        move_MyCraft(&my_craft, UP);
        break;
    case 0b0100:
        move_MyCraft(&my_craft, DOWN);
        break;
    case 0b0010:
        move_MyCraft(&my_craft, RIGHT);
        break;
    // case 0b1010:
    //     shut_MyCraft(&my_craft);
    //     break;
    default:
        break;
    }
}

act_seq_t *get_cur_act_seq(plane_t *craft)
{
    return craft->act_seq_list[craft->act_type];
}

map_t *get_cur_map(plane_t *craft)
{
    act_seq_t *cur_act_seq = craft->act_seq_list[craft->act_type];
    map_t *cur_map = cur_act_seq->maplist[cur_act_seq->act_index];
    return cur_map;
}

void reset_Lcraft(plane_t *craft)
{
    craft->pos_x = random() % 32 + 16;
    craft->pos_y = -random() % 10 - 16; // 负坐标 延迟多久出现 以及密度
    for (int i = 0; i < craft->act_list_len; i++)
    {
        craft->act_seq_list[i]->act_index = 0;
        craft->act_seq_list[i]->timer = 0;
    }
    craft->cur_life = craft->full_life;
    craft->act_type = 0;
}

void destory(plane_t *craft)
{
    craft->act_type = 1;
    craft->cur_life = 0;
}

void reset_Mcraft(plane_t *craft)
{
    craft->pos_x = random() % 52 + 6;
    craft->pos_y = -random() % 1000 - 16; // 负坐标 延迟多久出现
    craft->cur_life = craft->full_life;
    craft->act_type = 0;
}

void reset_Scraft(plane_t *craft)
{
    craft->pos_x = random() % 56 + 4;
    craft->pos_y = -random() % 1000 - 16; // 负坐标 延迟多久出现
    craft->cur_life = craft->full_life;
    craft->act_type = 0;
}

void reset_MyCraft(plane_t *craft)
{
    craft->cur_life = craft->full_life;
    craft->pos_x = 31;
    craft->pos_y = 116;
    craft->act_type = 0;
    for (int i = 0; i < craft->act_list_len; i++)
    {
        craft->act_seq_list[i]->act_index = 0;
        craft->act_seq_list[i]->timer = 0;
    }
}

void move_Lcraft(plane_t *craft)
{
    if (craft->cur_life <= 0)
        return;
    map_t *cur_map = get_cur_map(craft);
    craft->pos_y += craft->speed;
    int top = craft->pos_y + cur_map->offset_y;
    if (top > 132)
        reset_Lcraft(craft);
}

void move_Mcraft(plane_t *craft)
{
    if (craft->cur_life <= 0)
        return;
    map_t *cur_map = get_cur_map(craft);
    craft->pos_y += craft->speed;
    int top = craft->pos_y + cur_map->offset_y;
    if (top > 132)
        reset_Mcraft(craft);
}

void move_Scraft(plane_t *craft)
{
    if (craft->cur_life <= 0)
        return;
    map_t *cur_map = get_cur_map(craft);
    craft->pos_y += craft->speed;
    int top = craft->pos_y + cur_map->offset_y;
    if (top > 132)
        reset_Scraft(craft);
}

void move_MyCraft(plane_t *my_craft, my_craft_dir_e_t dir)
{
    if (my_craft->cur_life <= 0)
        return;
    map_t *cur_map = get_cur_map(my_craft);
    int top = my_craft->pos_y + cur_map->offset_y;
    int bottom = my_craft->pos_y + cur_map->offset_y + cur_map->icon->width;

    int left = my_craft->pos_x + cur_map->offset_x;
    int right = my_craft->pos_x + cur_map->offset_x + cur_map->icon->height;

    switch (dir)
    {
    case UP:
        if (!(top - my_craft->speed < 0))
            my_craft->pos_y -= my_craft->speed;
        break;
    case DOWN:
        if (!(bottom + my_craft->speed > 132))
            my_craft->pos_y += my_craft->speed;
        break;
    case LEFT:
        if (!(left - my_craft->speed < 0))
            my_craft->pos_x -= my_craft->speed;
        break;
    case RIGHT:
        if (!(right + my_craft->speed > 64))
            my_craft->pos_x += my_craft->speed;
        break;
    default:
        break;
    }
}

void move_bullet(bullet_t *bullet)
{
    if (bullet->cur_life <= 0)
        return;
    if (bullet->active == 0)
        return;
    map_t *cur_map = get_cur_map(bullet);
    bullet->pos_y -= bullet->speed;
    int bottom = bullet->pos_y + cur_map->offset_y + cur_map->icon->width;
    if (bottom < 0)
    {
        reset_bullet(bullet);
    }
    else if ((bullet->start_y - bullet->pos_y) > bullet->range)
    {
        reset_bullet(bullet);
    }
}

plane_t *get_deactived_bullet()
{
    for (int i = 0; i < MAX_BULLET; i++)
    {
        if (bullet_group[i]->active == 0)
            return bullet_group[i];
    }
}

bullet_t *reload_bullet(int fire_pos_x, int fire_pos_y)
{
    bullet_t *load_bullet = get_deactived_bullet();
    load_bullet->start_x = fire_pos_x;
    load_bullet->start_y = fire_pos_y;
    load_bullet->pos_x = fire_pos_x;
    load_bullet->pos_y = fire_pos_y;
    load_bullet->active = 1;
    load_bullet->cur_life = load_bullet->full_life;
    return load_bullet;
}

void reset_bullet(bullet_t *bullet)
{
    bullet->pos_x = -100;
    bullet->act_type = 0;
    bullet->active = 0;
}

void shut_MyCraft(plane_t *craft)
{
    // 其实枪口应该作为结构体的一部分 有一个对应的offset
    // 通过这个offset去生成子弹
    // 但是吧 。。。 再这么整太乱了 先随便定个offset乱射吧 之后有空再整理进结构体里
    // int gun_fire_pos_x = craft->pos_x - 6;
    // int gun_fire_pos_y = craft->pos_y - 2;
    if (craft->cur_life <= 0)
        return;
    reload_bullet(craft->pos_x - 6, craft->pos_y - 3);
    reload_bullet(craft->pos_x + 6, craft->pos_y - 3);
}

void draw_craft(plane_t *craft)
{
    map_t *cur_map = get_cur_map(craft);
    // 画图的时候 还得用图片的右上角 转换坐标系很麻烦啊
    OLED_Icon_Draw(
        craft->pos_y + cur_map->offset_y,
        64 - (craft->pos_x + cur_map->offset_x + cur_map->icon->height),
        cur_map->icon,
        2);
}

void draw_bullet(bullet_t *bullet)
{
    if (bullet->active == 0)
        return;
    act_seq_t *cur_act_seq = bullet->act_seq_list[bullet->act_type];
    map_t *cur_map = cur_act_seq->maplist[cur_act_seq->act_index];

    OLED_Icon_Draw(
        bullet->pos_y + cur_map->offset_y,
        64 - (bullet->pos_x + cur_map->offset_x + cur_map->icon->height),
        cur_map->icon,
        2);
}

void craft_update_act(plane_t *craft)
{
    act_seq_t *cur_act_seq = craft->act_seq_list[craft->act_type];
    ++(cur_act_seq->timer);
    if (cur_act_seq->timer == cur_act_seq->const_delay)
    {

        cur_act_seq->timer = 0;
        ++(cur_act_seq->act_index);
        if (cur_act_seq->act_index == cur_act_seq->const_length)
        {
            if (cur_act_seq->isDestory == 1)
            {
                cur_act_seq->reset_func(craft);
            }
            else
                cur_act_seq->act_index = 0;
        }
    }
}

void global_update(void)
{
    for (int i = 0; i < MAX_L_CRAFT; i++)
    {
        craft_update_act(l_craft_group[i]);
        move_Lcraft(l_craft_group[i]);
    }
    for (int i = 0; i < MAX_M_CRAFT; i++)
    {
        craft_update_act(m_craft_group[i]);
        move_Mcraft(m_craft_group[i]);
    }
    for (int i = 0; i < MAX_S_CRAFT; i++)
    {
        craft_update_act(s_craft_group[i]);
        move_Scraft(s_craft_group[i]);
    }
    for (int i = 0; i < MAX_BULLET; i++)
    {
        // craft_update_act(s_craft_group[i]); 子弹暂时没有特效 后面可以考虑加
        move_bullet(bullet_group[i]);
    }
    craft_update_act(&my_craft);
    shut_MyCraft(&my_craft);
}

void global_draw(void)
{
    for (int i = 0; i < MAX_L_CRAFT; i++)
    {
        draw_craft(l_craft_group[i]);
    }
    for (int i = 0; i < MAX_M_CRAFT; i++)
    {
        draw_craft(m_craft_group[i]);
    }
    for (int i = 0; i < MAX_S_CRAFT; i++)
    {
        draw_craft(s_craft_group[i]);
    }
    for (int i = 0; i < MAX_BULLET; i++)
    {
        draw_bullet(bullet_group[i]);
    }
    draw_craft(&my_craft);
}

int hit_check(bullet_t *bullet, plane_t *craft)
{
    if (craft->pos_y <= 0)
        return 0;
    if (craft->cur_life <= 0)
        return 0;
    if (bullet->active <= 0)
        return 0;
    act_seq_t *cur_act_seq = bullet->act_seq_list[bullet->act_type];
    map_t *cur_map = cur_act_seq->maplist[cur_act_seq->act_index];

    for (int bullet_bit_x = 0; bullet_bit_x < (cur_map->icon->height); bullet_bit_x++)
    {
        for (int bullet_bit_y = 0; bullet_bit_y < (cur_map->icon->width); bullet_bit_y++)
        {
            uint8_t bit = (cur_map->icon->p_icon_mask == NULL) ? cur_map->icon->p_icon_data[bullet_bit_x / 8 + bullet_bit_y] & (0x01 << bullet_bit_x % 8) : cur_map->icon->p_icon_mask[bullet_bit_x / 8 + bullet_bit_y] & (0x01 << bullet_bit_x % 8);
            if (bit == 0)
                continue;

            int bit_pos_x = bullet->pos_x + cur_map->offset_x + cur_map->icon->height - bullet_bit_x;
            int bit_pos_y = bullet->pos_y + cur_map->offset_y + bullet_bit_y;

            act_seq_t *cur_craft_act_seq = craft->act_seq_list[craft->act_type];
            map_t *cur_craft_map = cur_craft_act_seq->maplist[cur_craft_act_seq->act_index];

            for (int craft_bit_x = 0; craft_bit_x < (cur_craft_map->icon->height); craft_bit_x++)
            {
                for (int craft_bit_y = 0; craft_bit_y < (cur_craft_map->icon->width); craft_bit_y++)
                {
                    uint8_t craft_bit = (cur_craft_map->icon->p_icon_mask == NULL) ? cur_craft_map->icon->p_icon_data[craft_bit_x / 8 + craft_bit_y] & (0x01 << craft_bit_x % 8) : cur_craft_map->icon->p_icon_mask[craft_bit_x / 8 + craft_bit_y] & (0x01 << craft_bit_x % 8);
                    if (craft_bit == 0)
                        continue;
                    // 找到有效点对应的绝对坐标
                    int craft_bit_pos_x = craft->pos_x + cur_craft_map->offset_x + cur_craft_map->icon->height - craft_bit_x;
                    int craft_bit_pos_y = craft->pos_y + cur_craft_map->offset_y + craft_bit_y;
                    // 开始遍历所有可撞击对象
                    if (craft_bit_pos_x == bit_pos_x && craft_bit_pos_y == bit_pos_y)
                    {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

int hit_check_me(plane_t *bullet, plane_t *craft)
{
    if (craft->pos_y <= 0)
        return 0;
    if (craft->cur_life <= 0)
        return 0;
    if (bullet->cur_life <= 0)
        return 0;
    act_seq_t *cur_act_seq = bullet->act_seq_list[bullet->act_type];
    map_t *cur_map = cur_act_seq->maplist[cur_act_seq->act_index];

    for (int bullet_bit_x = 0; bullet_bit_x < (cur_map->icon->height); bullet_bit_x++)
    {
        for (int bullet_bit_y = 0; bullet_bit_y < (cur_map->icon->width); bullet_bit_y++)
        {
            uint8_t bit = (cur_map->icon->p_icon_mask == NULL) ? cur_map->icon->p_icon_data[bullet_bit_x / 8 + bullet_bit_y] & (0x01 << bullet_bit_x % 8) : cur_map->icon->p_icon_mask[bullet_bit_x / 8 + bullet_bit_y] & (0x01 << bullet_bit_x % 8);
            if (bit == 0)
                continue;

            int bit_pos_x = bullet->pos_x + cur_map->offset_x + cur_map->icon->height - bullet_bit_x;
            int bit_pos_y = bullet->pos_y + cur_map->offset_y + bullet_bit_y;

            act_seq_t *cur_craft_act_seq = craft->act_seq_list[craft->act_type];
            map_t *cur_craft_map = cur_craft_act_seq->maplist[cur_craft_act_seq->act_index];

            for (int craft_bit_x = 0; craft_bit_x < (cur_craft_map->icon->height); craft_bit_x++)
            {
                for (int craft_bit_y = 0; craft_bit_y < (cur_craft_map->icon->width); craft_bit_y++)
                {
                    uint8_t craft_bit = (cur_craft_map->icon->p_icon_mask == NULL) ? cur_craft_map->icon->p_icon_data[craft_bit_x / 8 + craft_bit_y] & (0x01 << craft_bit_x % 8) : cur_craft_map->icon->p_icon_mask[craft_bit_x / 8 + craft_bit_y] & (0x01 << craft_bit_x % 8);
                    if (craft_bit == 0)
                        continue;
                    // 找到有效点对应的绝对坐标
                    int craft_bit_pos_x = craft->pos_x + cur_craft_map->offset_x + cur_craft_map->icon->height - craft_bit_x;
                    int craft_bit_pos_y = craft->pos_y + cur_craft_map->offset_y + craft_bit_y;
                    // 开始遍历所有可撞击对象
                    if (craft_bit_pos_x == bit_pos_x && craft_bit_pos_y == bit_pos_y)
                    {
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
    for (int j = 0; j < MAX_BULLET; j++)
    {
        bullet_t *bullet = bullet_group[j];
        if (bullet->active == 0)
            continue;

        for (int i = 0; i < MAX_L_CRAFT; i++)
        {
            plane_t *craft = l_craft_group[i];
            if (craft->cur_life <= 0)
                continue;

            if (hit_check(bullet, craft))
            {
                printf("l_craft hit!\n");
                craft->cur_life -= bullet->harm;
                reset_bullet(bullet);
                if (craft->cur_life <= 0)
                {
                    printf("l_craft destory!\n");
                    destory(craft);
                }
                // 不再遍历当前对象
                continue;
            }
        }

        for (int i = 0; i < MAX_M_CRAFT; i++)
        {
            plane_t *craft = m_craft_group[i];
            if (craft->cur_life <= 0)
                continue;

            if (hit_check(bullet, craft))
            {
                printf("m_craft hit!\n");
                craft->cur_life -= bullet->harm;
                reset_bullet(bullet);
                if (craft->cur_life <= 0)
                {
                    printf("m_craft destory!\n");
                    reset_Mcraft(craft);
                }
                // 不再遍历当前对象
                continue;
            }
        }

        for (int i = 0; i < MAX_S_CRAFT; i++)
        {
            plane_t *craft = s_craft_group[i];
            if (craft->cur_life <= 0)
                continue;

            if (hit_check(bullet, craft))
            {
                printf("s_craft hit!\n");
                craft->cur_life -= bullet->harm;
                reset_bullet(bullet);
                if (craft->cur_life <= 0)
                {
                    printf("s_craft destory!\n");
                    reset_Scraft(craft);
                }
                // 不再遍历当前对象
                continue;
            }
        }
    }
    // 我方飞机撞击检测
    for (int i = 0; i < MAX_L_CRAFT; i++)
    {
        plane_t *craft = l_craft_group[i];
        if (craft->cur_life <= 0)
            continue;

        if (hit_check_me(&my_craft, craft))
        {
            printf("my_craft destory!\n");
            destory(&my_craft);
            g_chance--;
            destory(&craft);
            return;
        }
    }

    for (int i = 0; i < MAX_M_CRAFT; i++)
    {
        plane_t *craft = m_craft_group[i];
        if (craft->cur_life <= 0)
            continue;

        if (hit_check_me(&my_craft, craft))
        {
            printf("my_craft destory!\n");
            destory(&my_craft);
            g_chance--;
            // destory(&craft);
            return;
        }
    }

    for (int i = 0; i < MAX_S_CRAFT; i++)
    {
        plane_t *craft = s_craft_group[i];
        if (craft->cur_life <= 0)
            continue;

        if (hit_check_me(&my_craft, craft))
        {
            printf("my_craft destory!\n");
            g_chance--;
            destory(&my_craft);
            // destory(&craft);
            return;
        }
    }
}

void global_create(void)
{
    for (int i = 0; i < MAX_L_CRAFT; i++)
    {
        plane_t *craft = (plane_t *)malloc(sizeof(plane_t));
        memcpy(craft, &g_l_craft, sizeof(plane_t));

        act_seq_t *act0 = (act_seq_t *)malloc(sizeof(act_seq_t));
        memcpy(act0, &l_craft_normal_act, sizeof(act_seq_t));
        act_seq_t *act1 = (act_seq_t *)malloc(sizeof(act_seq_t));
        memcpy(act1, &l_craft_destory_act, sizeof(act_seq_t));
        craft->act_seq_list[0] = act0;
        craft->act_seq_list[1] = act1;

        reset_Lcraft(craft);
        l_craft_group[i] = craft;
    }
    for (int i = 0; i < MAX_M_CRAFT; i++)
    {
        plane_t *craft = (plane_t *)malloc(sizeof(plane_t));
        memcpy(craft, &g_m_craft, sizeof(plane_t));
        reset_Mcraft(craft);
        m_craft_group[i] = craft;
    }
    for (int i = 0; i < MAX_S_CRAFT; i++)
    {
        plane_t *craft = (plane_t *)malloc(sizeof(plane_t));
        memcpy(craft, &g_s_craft, sizeof(plane_t));
        reset_Scraft(craft);
        s_craft_group[i] = craft;
    }
    for (int i = 0; i < MAX_BULLET; i++)
    {
        bullet_t *bullet = (bullet_t *)malloc(sizeof(bullet_t));
        memcpy(bullet, &g_bullet, sizeof(bullet_t));
        bullet->active = 0;
        bullet->pos_x = -100; //  子弹全都隐藏到屏幕外
        bullet_group[i] = bullet;
    }
}

int aircraftBattle_init(void)
{
    OLED_Clear();
    // OLED_test();
    OLED_Refresh_GRAM();
    global_create();
    aos_task_new("aircraftBattle_task", aircraftBattle_task, NULL, 1000);
    printf("aos_task_new aircraftBattle_task \n");
    return 0;
}

int aircraftBattle_uninit(void)
{
    // TODO Free!!!!!
    aos_task_delete("aircraftBattle_task");
    printf("aos_task_delete aircraftBattle_task \n");
    return 0;
}

void aircraftBattle_task()
{
    while (1)
    {
        if (g_chance > 0)
        {
            OLED_Clear();
            global_update();
            global_hit_check();
            global_draw();
            OLED_Refresh_GRAM();
            aos_msleep(40);
        }
        else
        {
            OLED_Clear();
            OLED_Show_String(30, 24, "GAME OVER", 16, 1);
            OLED_Refresh_GRAM();
            aos_msleep(1000);
        }
    }
}