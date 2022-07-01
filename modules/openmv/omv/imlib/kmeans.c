/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Kmeans clustering.
 */
#include <float.h>
#include <limits.h>
#include <arm_math.h>
#include <stdio.h>
#include "imlib.h"
#include "array.h"
#include "xalloc.h"

extern uint32_t rng_randint(uint32_t min, uint32_t max);

static cluster_t *cluster_alloc(int cx, int cy)
{
    cluster_t *c=NULL;
    c = xalloc(sizeof(*c));
    if (c != NULL) {
        c->x = cx;
        c->y = cy;
        c->w = 0;
        c->h = 0;
        array_alloc(&c->points, NULL);
    }
    return c;
}

static void cluster_free(void *c)
{
    cluster_t *cl = c;
    array_free(cl->points);
    xfree(cl);
}

static void cluster_reset(array_t *clusters, array_t *points)
{
    // Reset all clusters
    for (int j=0; j<array_length(clusters); j++) {
         cluster_t *cl = array_at(clusters, j);
         while (array_length(cl->points)) {
             array_push_back(points, array_pop_back(cl->points));
         }
         array_free(cl->points);
         array_alloc(&cl->points, NULL);
    }
}

static int cluster_update(array_t *clusters)
{
    // Update clusters
    for (int j=0; j<array_length(clusters); j++) {
        cluster_t *cl = array_at(clusters, j);
        int cx = cl->x, cy = cl->y;
        int cl_size = array_length(cl->points);

        // Sum all points in this cluster
        for (int i=0; i<cl_size; i++) {
            kp_t *p = array_at(cl->points, i);
            cl->x += p->x;
            cl->y += p->y;
            // Find out the max x and y while we're at it
            if (p->x > cl->w) {
                cl->w = p->x;
            }
            if (p->y > cl->h) {
                cl->h = p->y;
            }
        }

        // Update centroid
        cl->x = cl->x/cl_size;
        cl->y = cl->y/cl_size;
        // Update cluster size
        cl->w = (cl->w - cl->x) * 2;
        cl->h = (cl->h - cl->y) * 2;
        if (cl->x == cx && cl->y == cy) {
            // Cluster centroid did not change
            return 0;
        }
    }

    return 1;
}

static void cluster_points(array_t *clusters, array_t *points, cluster_dist_t dist_func)
{
    // Add objects to cluster
    while (array_length(points)) {
        float distance = FLT_MAX;
        cluster_t *cl_nearest = NULL;
        kp_t *p = array_pop_back(points);

        for (int j=0; j<array_length(clusters); j++) {
            cluster_t *cl = array_at(clusters, j);
            float d = dist_func(cl->x, cl->y, p);
            if (d < distance) {
                distance = d;
                cl_nearest = cl;
            }
        }
        // Add pointer to point to cluster.
        // Note: Objects in the cluster are not free'd
        array_push_back(cl_nearest->points, p);
    }
}

array_t *cluster_kmeans(array_t *points, int k, cluster_dist_t dist_func)
{
    // Alloc clusters array
    array_t *clusters=NULL;
    array_alloc(&clusters, cluster_free);

    // Select K clusters randomly
    for (int i=0; i<k; i++) {
        int pidx = rng_randint(0, array_length(points)-1);
        kp_t *p = array_at(points, pidx);
        array_push_back(clusters, cluster_alloc(p->x, p->y));
    }

    int cl_changed = 1;
    do {
        // Reset clusters
        cluster_reset(clusters, points);

        // Add points to clusters
        cluster_points(clusters, points, dist_func);

        // Update centroids
        cl_changed = cluster_update(clusters);

    } while (cl_changed);

    return clusters;
}
