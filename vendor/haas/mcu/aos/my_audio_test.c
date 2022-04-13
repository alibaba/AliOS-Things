#include <stdint.h>
#include "genie_test.h"
#include "audio_player.h"
#include "aos/vfs.h"
#include "aos/kernel.h"

static aos_task_t g_audio_test_task;

static aos_sem_t bes_audio_sem;

static void bes_play_finish_cb()
{
    aos_sem_signal(&bes_audio_sem);
}

static void bes_play_prompt_init(void)
{
    audio_player_file_opt_t file_opt;
    file_opt.open = aos_open;
    file_opt.close = aos_close;
    file_opt.read = aos_read;
    audio_player_file_ops_register(&file_opt);
    if(0 != aos_sem_new(&bes_audio_sem, 1)){
        printf("aos new bes audio sem err\r\n");
        return -1;
    }
    printf("bes audio sem @ %p\r\n", &bes_audio_sem);
    aos_sem_wait(&bes_audio_sem, AOS_WAIT_FOREVER);
}

static void bes_play_prompt_denint(void)
{
    aos_sem_free(&bes_audio_sem);
}

static void bes_play_prompt_test(const char * path)
{
    if(!aos_sem_is_valid(&bes_audio_sem))
        return;
#if 1
    if( 0 == audio_player_play_file(path, bes_play_finish_cb)){
        aos_sem_wait(&bes_audio_sem, AOS_WAIT_FOREVER);
    }
#endif

#if 0
    AG_audio_decoder_mp3_play_file(path);
#endif

#if 0
    uint8_t * tmp_buf = (uint8_t *)malloc(30 * 1024);
    if(!tmp_buf){
        printf("mem alloc err\n");
        return;
    }
    int fd = audio_fs_open(path, 2);
    if(fd < 0){
        printf("open file err\n");
        return;
    }
    int len = audio_fs_read(fd, tmp_buf, 30*1024);
    printf("len %d\n", len);
    AG_audio_decoder_mp3_play_mem(tmp_buf, len);
    free(tmp_buf);
    audio_fs_close(fd);
#endif
}

static void bes_audio_test_task(void)
{
    bes_play_prompt_init();

    while(1)
    {
        //printf("use AGAudioDecoder and AGAudioOutput interface to play music\r\n");
        //AG_audio_decoder_test();
#if 1
        printf("use BES audio player to play music\r\n");
        bes_play_prompt_test("/data/prompt/cjml.wav");
        bes_play_prompt_test("/data/prompt/1.wav");
        bes_play_prompt_test("/data/prompt/1.mp3");
        bes_play_prompt_test("/data/prompt/2.mp3");
#endif
#if 0
        bes_play_prompt_test("/data/prompt/4.mp3");
        bes_play_prompt_test("/data/prompt/5.mp3");
        bes_play_prompt_test("/data/prompt/6.mp3");
        bes_play_prompt_test("/data/prompt/7.mp3");
        bes_play_prompt_test("/data/prompt/8.mp3");
        bes_play_prompt_test("/data/prompt/9.mp3");
        bes_play_prompt_test("/data/prompt/10.mp3");
        bes_play_prompt_test("/data/prompt/12.mp3");
        bes_play_prompt_test("/data/prompt/13.mp3");
        bes_play_prompt_test("/data/prompt/15.mp3");
        bes_play_prompt_test("/data/prompt/17.mp3");
        bes_play_prompt_test("/data/prompt/18.mp3");
        bes_play_prompt_test("/data/prompt/20.mp3");
        bes_play_prompt_test("/data/prompt/21.mp3");
        bes_play_prompt_test("/data/prompt/23.mp3");
        bes_play_prompt_test("/data/prompt/40.mp3");
        bes_play_prompt_test("/data/prompt/41.mp3");
        bes_play_prompt_test("/data/prompt/42.mp3");
        bes_play_prompt_test("/data/prompt/44.mp3");
        bes_play_prompt_test("/data/prompt/45.mp3");
        bes_play_prompt_test("/data/prompt/50.mp3");
        bes_play_prompt_test("/data/prompt/64.mp3");
        bes_play_prompt_test("/data/prompt/65.mp3");
        bes_play_prompt_test("/data/prompt/66.mp3");
        bes_play_prompt_test("/data/prompt/94.mp3");
        bes_play_prompt_test("/data/prompt/141.mp3");
        bes_play_prompt_test("/data/prompt/151.mp3");
#endif
    }
}

void do_bes_audio_test(void)
{
    aos_task_new_ext(&g_audio_test_task, "audio_test_task", bes_audio_test_task, NULL, (32*1024), 32);

    // krhino_task_dyn_create(&g_audio_test_task, "audio_test_task", 0, 32, 0, (32*1024), (task_entry_t)bes_audio_test_task, 1);
}
