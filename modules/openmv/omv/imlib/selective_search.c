/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Selective search.
 */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include "imlib.h"
#include "fb_alloc.h"
#include "xalloc.h"
#ifdef IMLIB_ENABLE_SELECTIVE_SEARCH

#define THRESHOLD(size, c) (c/size)
typedef struct {
    uint16_t y;
    uint16_t h;
    uint16_t x;
    uint16_t w;
} region;

typedef struct {
    uint16_t p;
    uint16_t rank;
    uint16_t size;
} uni_elt;

typedef struct {
    int num;
    uni_elt *elts;
} universe;

typedef struct {
    float    w;
    uint16_t a;
    uint16_t b;
} edge;

static inline int min (int a, int b) { return (a < b) ? a : b; }
static inline int max (int a, int b) { return (a > b) ? a : b; }
static inline float minf (float a, float b) { return (a < b) ? a : b; }
static inline float maxf (float a, float b) { return (a > b) ? a : b; }
extern uint32_t rng_randint(uint32_t min, uint32_t max);

static universe *universe_create(int elements)
{
    universe * uni = (universe*) fb_alloc(sizeof(universe), FB_ALLOC_NO_HINT);
    uni->elts = (uni_elt*) fb_alloc(sizeof(uni_elt)*elements, FB_ALLOC_NO_HINT);
    uni->num = elements;
    for (int i=0; i<elements; ++i) {
        uni->elts[i].p = i;
        uni->elts[i].rank = 0;
        uni->elts[i].size = 1;
    }
    return uni;
}

static int universe_size(universe * uni, int x)
{
    return uni->elts[x].size;
}

static int universe_num_sets(universe * uni)
{
    return uni->num;
}

static int universe_find(universe * uni, int x)
{
    int y = x;
    while (y != uni->elts[y].p) {
        y = uni->elts[y].p;
    }
    // Path compression
    uni->elts[x].p = y;
    return y;
}

static void universe_join (universe * uni, int x, int y)
{
    if (uni->elts[x].rank > uni->elts[y].rank) {
        uni->elts[y].p = x;
        uni->elts[x].size += uni->elts[y].size;
    } else {
        uni->elts[x].p = y;
        uni->elts[y].size += uni->elts[x].size;
        if (uni->elts[x].rank == uni->elts[y].rank) {
            uni->elts[y].rank++;
        }
    }
    uni->num--;
}

static int universe_get_id(universe * this, int x)
{
    return this->elts[x].rank;
}

static void universe_set_id(universe * this, int x, int id)
{
    this->elts[x].rank = id;
}

static inline float color_similarity (float * hist1, float * hist2)
{
    float sim = 0;
    for (int i = 0; i < 75; ++i) {
        sim += minf(hist1[i], hist2[i]);
    }
    return sim;
}

static inline float size_similarity (int a, int b, int size)
{
    return 1.0f - (a + b)/size;
}

static inline float fill_similarity (region * ra, region * rb, int a, int b, int size)
{
    int width = max(ra->w, rb->w) - min(ra->x, rb->x);
    int height = max(ra->h, rb->h) - min(ra->y, rb->y);
    return 1.0f - (width*height - a - b)/size;
}

static inline float square(float x) { return x*x; };

static inline float diff(image_t *img, int x1, int y1, int x2, int y2)
{
    uint16_t p1 = IMAGE_GET_RGB565_PIXEL(img, x1, y1);
    uint16_t p2 = IMAGE_GET_RGB565_PIXEL(img, x2, y2);
    uint8_t r1  = COLOR_RGB565_TO_R8(p1);
    uint8_t r2  = COLOR_RGB565_TO_R8(p2);

    uint8_t g1  = COLOR_RGB565_TO_G8(p1);
    uint8_t g2  = COLOR_RGB565_TO_G8(p2);

    uint8_t b1  = COLOR_RGB565_TO_B8(p1);
    uint8_t b2  = COLOR_RGB565_TO_B8(p2);
   // dissimilarity measure between pixels
   return  sqrtf((r1-r2) * (r1-r2) + (g1-g2) * (g1-g2) + (b1-b2) * (b1-b2));
}

int comp (const void * elem1, const void * elem2) 
{
    edge *f = (edge*) elem1;
    edge *s = (edge*) elem2;
    if (f->w > s->w) return  1;
    if (f->w < s->w) return -1;
    return 0;
}

static void segment_graph(universe *u, int num_vertices, int num_edges, edge *edges, float c)
{
    qsort (edges, num_edges, sizeof(edge), comp);

    float *threshold = fb_alloc(num_vertices * sizeof(float), FB_ALLOC_NO_HINT);
    for (int i=0; i<num_vertices; i++) {
        threshold[i] = THRESHOLD(1, c);
    }

    for (int i=0; i<num_edges; i++) {
        edge *pedge = edges + i;
        int a = universe_find (u, pedge->a);
        int b = universe_find (u, pedge->b);
        if (a != b) {
            if ((pedge->w <= threshold[a]) && (pedge->w <= threshold[b])) {
                universe_join (u, a, b);
                a = universe_find (u, a);
                threshold[a] = pedge->w + THRESHOLD(universe_size (u, a), c);
            }
        }
    }

    // Free thresholds.
    fb_free();
}

static void image_scale(image_t *src, image_t *dst)
{
    int x_ratio = (int)((src->w<<16)/dst->w) +1;
    int y_ratio = (int)((src->h<<16)/dst->h) +1;

	for (int y=0; y<dst->h; y++) {
        int sy = (y*y_ratio)>>16;
		for (int x=0; x<dst->w; x++) {
            int sx = (x*x_ratio)>>16;
			((uint16_t*)dst->pixels)[y*dst->w+x] = ((uint16_t*)src->pixels)[sy*src->w+sx];
		}
	}
}

array_t *imlib_selective_search(image_t *src, float t, int min_size, float a1, float a2, float a3)
{
    int i,j;
    int num = 0;
    int width=0, height=0;
    image_t *img = NULL;

    fb_alloc_mark();

    if ((src->w * src->h) <= (80 * 60)) {
        img    = src;
        width  = src->w;
        height = src->h;
    } else {
        // Down scale image
        width  = src->w / 4;
        height = src->h / 4;
        img = fb_alloc(sizeof(image_t), FB_ALLOC_NO_HINT);
        img->w = width;
        img->h = height;
        img->pixels = fb_alloc(width * height * 2, FB_ALLOC_NO_HINT);
        image_scale(src, img); 
    }

    // Region proposals array
    array_t *proposals;
    array_alloc(&proposals, xfree);

    universe *u = universe_create (width * height);
    edge *edges = (edge*) fb_alloc(width * height * sizeof(edge) * 4, FB_ALLOC_NO_HINT);

    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            if (x < width-1) {
                edges[num].a = y * width + x;
                edges[num].b = y * width + (x+1);
                edges[num].w = diff(img, x, y, x+1, y);
                num++;
            }

            if (y < height-1) {
                edges[num].a = y * width + x;
                edges[num].b = (y+1) * width + x;
                edges[num].w = diff(img, x, y, x, y+1);
                num++;
            }

            if ((x < width-1) && (y < height-1)) {
                edges[num].a = y * width + x;
                edges[num].b = (y+1) * width + (x+1);
                edges[num].w = diff(img, x, y, x+1, y+1);
                num++;
            }

            if ((x < width-1) && (y > 0)) {
                edges[num].a = y * width + x;
                edges[num].b = (y-1) * width + (x+1);
                edges[num].w = diff(img, x, y, x+1, y-1);
                num++;
            }
        }
    }

    segment_graph(u, width * height, num, edges, t);
    
    for (i=0; i<num; i++) {
        int a = universe_find(u, edges[i].a);
        int b = universe_find(u, edges[i].b);
        if ((a != b) && ((universe_size(u, a) < min_size) || (universe_size(u, b) < min_size)))
            universe_join (u, a, b);
    }

    // Free graph edges
    fb_free();

    int num_ccs = universe_num_sets(u);
    region * regions = (region*) fb_alloc(num_ccs * sizeof(region), FB_ALLOC_NO_HINT);
    for (i=0; i<num_ccs; i++) {
        regions[i].x = width;
        regions[i].w = 0;
        regions[i].y = height;
        regions[i].h = 0;
    }

    int next_component = 0;
    int   *counts = (int*) fb_alloc0(num_ccs * sizeof(int), FB_ALLOC_NO_HINT);
    int   *components= (int*) fb_alloc(num_ccs * sizeof(int), FB_ALLOC_NO_HINT);
    float *histogram = (float*) fb_alloc0(num_ccs * sizeof(float) * 75, FB_ALLOC_NO_HINT);

    // Calc histograms
    for (int y=0; y<height; y++) {
        for (int x = 0; x<width; x++) {
            int component_id = -1;
            int comp = universe_find(u, y * width + x);
            for (i=0; i<next_component; i++) {
                if (components[i] == comp) {
                    component_id = i;
                    break;
                }
            }
            if (i == next_component) {
                components[next_component] = comp;
                component_id = next_component;
                ++next_component;
            }
            universe_set_id(u, y * width + x, component_id);
            region * r = regions + component_id;
            r->y = min(r->y, y);
            r->h = max(r->h, y);
            r->x = min(r->x, x);
            r->w = max(r->w, x);
    
            uint16_t p = IMAGE_GET_RGB565_PIXEL(img, x, y);
            int r_bin = min(COLOR_RGB565_TO_R8(p), 240)/10;
            int g_bin = min(COLOR_RGB565_TO_G8(p), 240)/10;
            int b_bin = min(COLOR_RGB565_TO_B8(p), 240)/10;

            histogram[75*component_id +  0 + r_bin]++;
            histogram[75*component_id + 25 + g_bin]++;
            histogram[75*component_id + 50 + b_bin]++;
            counts[component_id]++;
        }
    }

    // Normalize histograms
    for (i=0; i<num_ccs; i++) {
        float max_val = 0;
        for (j=0; j<75; j++) {
            max_val = max(max_val, histogram[75*i + j]);
        }
        for (j=0; j<75; j++) {
            histogram[75*i + j] /= max_val;
        }
    }

    uint8_t * adjacency = (uint8_t*) fb_alloc0(num_ccs * num_ccs * sizeof(uint8_t), FB_ALLOC_NO_HINT);
    for (int y=0; y<height-1; ++y) {
        for (int x=0; x<width-1; ++x) {
            int component1 = universe_get_id(u, y * width + x);
            int component2 = universe_get_id(u, y * width + x + 1);
            int component3 = universe_get_id(u, y * width + x + width);

            if (component1 != component2) {
                adjacency[component1 * num_ccs + component2] = 1;
                adjacency[component2 * num_ccs + component1] = 1;
            }

            if (component1 != component3) {
                adjacency[component1 * num_ccs + component3] = 1;
                adjacency[component3 * num_ccs + component1] = 1;
            }
        }
    }

    int size = height * width;
    float * similarity_table = (float*) fb_alloc(num_ccs * num_ccs * sizeof(float), FB_ALLOC_NO_HINT);
    for (i = 0; i < num_ccs; ++i) {
        for (j = i + 1; j < num_ccs; ++j) {
            float color_sim = a1 * color_similarity (histogram + 75 * i, histogram + 75 * j);
            float size_sim  = a2 * size_similarity (counts[i], counts[j], size);
            float fill_sim  = a3 * fill_similarity (regions + i, regions + j, counts[i], counts[j], size);
            float similarity = color_sim + size_sim + fill_sim;
            similarity_table[i * num_ccs + j] = similarity;
            similarity_table[j * num_ccs + i] = similarity;
        }
    }

    int remaining = num_ccs;
    while (remaining > 1) {
        int best_i = -1;
        int best_j = -1;
        float best_similarity = 0;
        for (i=0; i<num_ccs; i++) {
            for (j=i+1; j<num_ccs; j++) {
                if (adjacency[i * num_ccs + j] == 0) {
                    continue;
                }
                float similarity = similarity_table[i * num_ccs + j];
                if (similarity > best_similarity) {
                    best_similarity = similarity;
                    best_i = i;
                    best_j = j;
                }
            }
        }

        if (best_i == -1) {
            printf("failed to build tree\n");
            break;
        }

        // update regions, histograms, counts, adjacency, similarity
        regions[best_i].x = min(regions[best_i].x, regions[best_j].x);
        regions[best_i].y = min(regions[best_i].y, regions[best_j].y);
        regions[best_i].w = max(regions[best_i].w, regions[best_j].w);
        regions[best_i].h = max(regions[best_i].h, regions[best_j].h);

        bool add = true;
        for (i=0; i<array_length(proposals); i++) {
            rectangle_t *r = array_at(proposals, i);
            if (regions[best_i].x == r->x && regions[best_i].y == r->y &&
                regions[best_i].w == r->w && regions[best_i].h == r->h) {
                add = false;
                break;
            }
        }
        if (add) {
            array_push_back(proposals, rectangle_alloc(regions[best_i].x,
                        regions[best_i].y, regions[best_i].w, regions[best_i].h));
        }


        for (i=0; i<75; i++) {
            histogram[75*best_i + i] = (counts[best_i] * histogram[75*best_i + i]
                    + counts[best_j] * histogram[75*best_j + i])/(counts[best_i] + counts[best_j]);
        }
        counts[best_i] += counts[best_j];

        for (i=0; i<num_ccs; i++) {
            adjacency[best_i * num_ccs + i] |= adjacency[best_j * num_ccs + i];
            adjacency[i * num_ccs + best_i] |= adjacency[i * num_ccs + best_j];
            adjacency[best_j * num_ccs + i]  = adjacency[i * num_ccs + best_j] = 0;
        }
        adjacency[best_i * num_ccs + best_i]  = 0;

        for (i=0; i<num_ccs; i++) {
            if (adjacency[best_i * num_ccs + i] == 0) {
                continue;
            }
            float color_sim = a1 * color_similarity (histogram + 75 * i, histogram + 75 * best_i);
            float size_sim  = a2 * size_similarity (counts[i], counts[best_i], size);
            float fill_sim  = a3 * fill_similarity (regions + i, regions + best_i, counts[i], counts[best_i], size);
            float similarity = color_sim + size_sim + fill_sim;
            similarity_table[i * num_ccs + best_i] = similarity;
            similarity_table[best_i * num_ccs + i] = similarity;
        }
        --remaining;
    }

    for (int i=0; i<array_length(proposals); i++) {
        rectangle_t *r = array_at(proposals, i);
        r->w = r->w - r->x;
        r->h = r->h - r->y;
        if ((src->w * src->h) > (80 * 60)) {
            r->x *=4;
            r->y *=4;
            r->w *=4;
            r->h *=4;
        }
    }
    fb_alloc_free_till_mark();
    return proposals;
}
#endif //IMLIB_ENABLE_SELECTIVE_SEARCH
