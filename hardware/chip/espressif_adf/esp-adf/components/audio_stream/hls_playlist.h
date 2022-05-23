/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _HLS_PLAYLIST_H_
#define _HLS_PLAYLIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "stdbool.h"
#include "sys/queue.h"

struct track_; // Forward declaration
typedef STAILQ_HEAD(track_list, track_) track_list_t;

typedef struct {
    char            *host_uri;
    char            *data;
    int             index;
    int             remain;
    int             total_read;
    track_list_t    tracks;
    int             total_tracks;
    int             content_length;
    bool            is_incomplete;      /* Indicates if playlist is live stream and must be fetched again */
} playlist_t;

/**
 * @brief       Insert a track into hls_playlist
 *
 * @param       playlist playlist handle
 * @param       track_uri Track URI to be inserted in playlist
 *
 */
void hls_playlist_insert(playlist_t *playlist, char *track_uri);

/**
 * @brief       Get next not-played track from playlist
 *
 * @param       playlist playlist handle
 *
 * @return
 *      - NULL if no playable track
 *      - Playable track
 *
 * @note        returned track must `not` be freed by application
 */
char *hls_playlist_get_next_track(playlist_t *playlist);

/**
 * @brief       Clear all the tracks from playlist
 *
 * @param       playlist playlist handle
 *
 */
void hls_playlist_clear(playlist_t *playlist);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _HLS_PLAYLIST_H_ */
