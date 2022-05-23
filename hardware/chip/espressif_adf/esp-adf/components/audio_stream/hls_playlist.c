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

#include <sys/unistd.h>
#include <sys/stat.h>
#include <string.h>

#include "audio_mem.h"

#include "hls_playlist.h"
#include "esp_log.h"
#include "errno.h"

static const char *TAG = "HLS_PLAYLIST";

#define MAX_PLAYLIST_TRACKS (128)
#define MAX_PLAYLIST_KEEP_TRACKS (18)

typedef struct track_ {
    char *uri;
    bool is_played;
    STAILQ_ENTRY(track_) next;
} track_t;

static void hls_remove_played_entry(playlist_t *playlist)
{
    track_t *track;
    /* Remove head entry if total_entries are > MAX_PLAYLIST_KEEP_TRACKS */
    if (playlist->total_tracks > MAX_PLAYLIST_KEEP_TRACKS) {
        track = STAILQ_FIRST(&playlist->tracks);
        if (track->is_played) {
            STAILQ_REMOVE_HEAD(&playlist->tracks, next);
            audio_free(track->uri);
            audio_free(track);
            playlist->total_tracks--;
        }
    }
}

void hls_playlist_insert(playlist_t *playlist, char *track_uri)
{
    track_t *track;
    const char *host_uri = (const char *) playlist->host_uri;

    while (playlist->total_tracks > MAX_PLAYLIST_TRACKS) {
        track = STAILQ_FIRST(&playlist->tracks);
        if (track == NULL) {
            break;
        }
        STAILQ_REMOVE(&playlist->tracks, track, track_, next);
        ESP_LOGD(TAG, "Remove %s", track->uri);
        audio_free(track->uri);
        audio_free(track);
        playlist->total_tracks --;
    }
    track = audio_calloc(1, sizeof(track_t));
    if (track == NULL) {
        return;
    }
    if (strstr(track_uri, "http") == track_uri) { // Full URI
        track->uri = audio_strdup(track_uri);
    } else if (strstr(track_uri, "//") == track_uri) { // schemeless URI
        if (strstr(host_uri, "https") == host_uri) {
            asprintf(&track->uri, "https:%s", track_uri);
        } else {
            asprintf(&track->uri, "http:%s", track_uri);
        }
    } else if (strstr(track_uri, "/") == track_uri) { // Root uri
        char *url = audio_strdup(host_uri);
        if (url == NULL) {
            return;
        }
        char *host = strstr(url, "//");
        if (host == NULL) {
            audio_free(url);
            return;
        }
        host += 2;
        char *path = strstr(host, "/");
        if (path == NULL) {
            audio_free(url);
            return;
        }
        path[0] = 0;
        asprintf(&track->uri, "%s%s", url, track_uri);
        audio_free(url);
    } else { // Relative URI
        char *url = audio_strdup(host_uri);
        if (url == NULL) {
            return;
        }
        char *pos = strrchr(url, '/'); // Search for last "/"
        if (pos == NULL) {
            audio_free(url);
            return;
        }
        pos[1] = '\0';
        asprintf(&track->uri, "%s%s", url, track_uri);
        audio_free(url);
    }
    if (track->uri == NULL) {
        ESP_LOGE(TAG, "Error insert URI to playlist");
        audio_free(track);
        return;
    }

    track_t *find = NULL;
    STAILQ_FOREACH(find, &playlist->tracks, next) {
        if (strcmp(find->uri, track->uri) == 0) {
            ESP_LOGW(TAG, "URI exist");
            audio_free(track->uri);
            audio_free(track);
            return;
        }
    }

    ESP_LOGD(TAG, "INSERT %s", track->uri);
    STAILQ_INSERT_TAIL(&playlist->tracks, track, next);
    playlist->total_tracks++;
    hls_remove_played_entry(playlist);
}

char *hls_playlist_get_next_track(playlist_t *playlist)
{
    track_t *track;
    hls_remove_played_entry(playlist);
    /* Find not played entry. */
    STAILQ_FOREACH(track, &playlist->tracks, next) {
        if (!track->is_played) {
            track->is_played = true;
            return track->uri;
        }
    }
    return NULL;
}

void hls_playlist_clear(playlist_t *playlist)
{
    track_t *track, *tmp;
    STAILQ_FOREACH_SAFE(track, &playlist->tracks, next, tmp) {
        STAILQ_REMOVE(&playlist->tracks, track, track_, next);
        audio_free(track->uri);
        audio_free(track);
    }

    if (playlist->host_uri) {
        audio_free(playlist->host_uri);
        playlist->host_uri = NULL;
    }
    playlist->is_incomplete = false;
    playlist->total_tracks = 0;
    playlist->total_read = 0;
}
