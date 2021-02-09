/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "hal_oled.h"
#include "aos/hal/pwm.h"
#include "musicbox.h"

MENU_COVER_TYP musicbox_cover = {MENU_COVER_FUNC, NULL, NULL, musicbox_cover_draw, 0};
MENU_TASK_TYP musicbox_tasks = {
    musicbox_init,
    musicbox_uninit};
MENU_TYP musicbox = {
    "musicbox",
    &musicbox_cover,
    &musicbox_tasks,
    musicbox_key_handel,
    NULL};

#define NOTE_SPACE_RATIO 1.3

music_t *music_list[] = {
    &Imperial_March,
    &liang_zhi_lao_hu,
};

player_t musicbox_player = {music_list, 2, 0, 0, 0, 0};

void musicbox_cover_draw(int *draw_index)
{
    int floating_y[] = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, -1, -2, -3, -4};
    OLED_Clear();

    // printf("floating_y %d", floating_y[*draw_index]);

    OLED_Icon_Draw(48, 16 + floating_y[*draw_index], &icon_note_32_32, 0);

    OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
    OLED_Icon_Draw(122, 24, &icon_skip_right, 0);

    (*draw_index)++;
    if ((*draw_index) >= 20)
    {
        (*draw_index) = 0;
    }

    OLED_Refresh_GRAM();
    aos_msleep(100);
    return 0;
}

int musicbox_init(void)
{
    OLED_Clear();
    OLED_Refresh_GRAM();

    musicbox_player.isPlaying = 0;
    musicbox_player.cur_music_note = 0;
    set_music_time();

    aos_task_new("musicbox_task", musicbox_task, NULL, 1000);
    printf("aos_task_new musicbox_task \n");
    return 0;
}

int musicbox_uninit(void)
{
    noTone(0);
    aos_task_delete("musicbox_task");
    printf("aos_task_delete musicbox_task \n");
    return 0;
}

void set_music_time()
{
    for (int i = 0; i < musicbox_player.music_list_len; i++)
    {
        for (int n = 0; n < musicbox_player.music_list[i]->noteLength; n++)
        {
            int noteDuration = 1000 / musicbox_player.music_list[i]->noteDurations[n];
            noteDuration = (noteDuration < 0) ? (-noteDuration * 1.5) : noteDuration;
            musicbox_player.music_list[i]->musicTime += (noteDuration + (int)(noteDuration * NOTE_SPACE_RATIO));
        }
    }
}

void next_song()
{
    musicbox_player.cur_music_index = ((musicbox_player.cur_music_index == musicbox_player.music_list_len - 1) ? (0) : (musicbox_player.cur_music_index + 1));
    printf("cur_music_index %d\n", musicbox_player.cur_music_index);
    musicbox_player.cur_music_note = 0;
    musicbox_player.cur_music_time = 0;
    musicbox_player.isPlaying = 1;
}

void previous_song()
{
    musicbox_player.cur_music_index = ((musicbox_player.cur_music_index == 0) ? (musicbox_player.music_list_len - 1) : (musicbox_player.cur_music_index - 1));
    printf("cur_music_index %d\n", musicbox_player.cur_music_index);
    musicbox_player.cur_music_note = 0;
    musicbox_player.cur_music_time = 0;
    musicbox_player.isPlaying = 1;
}

void pause_resume()
{
    musicbox_player.isPlaying = !musicbox_player.isPlaying;
}

void musicbox_key_handel(key_code_t key_code)
{
    // TODO 播放音乐之后没响应？
    switch (key_code)
    {
    case 0b0001: // pre
        previous_song();
        aos_msleep(1000);
        break;
    case 0b0100: // next
        next_song();
        aos_msleep(1000);
        break;
    case 0b1000: // pause/resume
        pause_resume();
        break;
    default:
        break;
    }
}

void tone(uint16_t port, uint16_t frequency, uint16_t duration)
{
    pwm_dev_t pwm = {port, {0.8, frequency}, NULL};
    if (frequency > 0)
    {
        hal_pwm_init(&pwm);
        hal_pwm_start(&pwm);
    }
    if (duration != 0)
    {
        aos_msleep(duration);
    }
    if (frequency > 0 && duration > 0)
    {
        hal_pwm_stop(&pwm);
        hal_pwm_finalize(&pwm);
    }
}

void noTone(uint16_t port)
{
    pwm_dev_t pwm = {port, {0.8, 1}, NULL};
    hal_pwm_stop(&pwm);
    hal_pwm_finalize(&pwm);
}

void musicbox_task()
{
    while (1)
    {
        OLED_Clear();
        music_t *cur_music = musicbox_player.music_list[musicbox_player.cur_music_index];

        char show_song_name[14] = {0};
        sprintf(show_song_name, "%-13.13s", cur_music->name);
        OLED_Show_String(14, 4, show_song_name, 16, 1);

        if (musicbox_player.isPlaying)
        {
            if (musicbox_player.cur_music_note < cur_music->noteLength)
            {
                int noteDuration = 1000 / cur_music->noteDurations[musicbox_player.cur_music_note];
                noteDuration = (noteDuration < 0) ? (-noteDuration * 1.5) : noteDuration;
                printf("note[%d] = %d\t delay %d ms\n", musicbox_player.cur_music_note, cur_music->noteDurations[musicbox_player.cur_music_note], noteDuration);
                int note = cur_music->notes[musicbox_player.cur_music_note];
                tone(0, note, noteDuration);
                aos_msleep((int)(noteDuration * NOTE_SPACE_RATIO));
                musicbox_player.cur_music_time += (noteDuration + (int)(noteDuration * NOTE_SPACE_RATIO));
                musicbox_player.cur_music_note++;
            }
            else
            {
                noTone(0);
                aos_msleep(1000);
                next_song();
            }
            OLED_Icon_Draw(54, 36, &icon_pause_24_24, 1);
        }
        else
        {
            OLED_Icon_Draw(54, 36, &icon_resume_24_24, 1);
            aos_msleep(500);
        }

        OLED_DrawLine(14, 26, 14, 29, 1);
        OLED_DrawLine(117, 26, 117, 29, 1);
        OLED_DrawLine(16, 27, (int)(16 + 99.0 * (musicbox_player.cur_music_time * 1.0 / cur_music->musicTime)), 27, 1);
        OLED_DrawLine(16, 28, 16 + 99.0 * (musicbox_player.cur_music_time * 1.0 / cur_music->musicTime), 28, 1);
        OLED_Icon_Draw(94, 36, &icon_next_song_24_24, 1);
        OLED_Icon_Draw(14, 36, &icon_previous_song_24_24, 1);
        OLED_Refresh_GRAM();
    }
}
