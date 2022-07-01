/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013-2016 Kwabena W. Agyeman <kwagyeman@openmv.io>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Fast 9 and 25 bin sort.
 *
 */
#include <stdlib.h>
#include "fsort.h"

// http://pages.ripco.net/~jgamble/nw.html

static void cmpswp(int *a, int *b)
{
    if ((*b) < (*a)) {
        int tmp = *a;
        *a = *b;
        *b = tmp;
    }
}

// Network for N=9, using Best Known Arrangement.

// There are 25 comparators in this network,
// grouped into 9 parallel operations.

// [[0,1],[3,4],[6,7]]
// [[1,2],[4,5],[7,8]]
// [[0,1],[3,4],[6,7],[2,5]]
// [[0,3],[1,4],[5,8]]
// [[3,6],[4,7],[2,5]]
// [[0,3],[1,4],[5,7],[2,6]]
// [[1,3],[4,6]]
// [[2,4],[5,6]]
// [[2,3]]

// This is graphed in 17 columns.

static void fsort9(int *data)
{
    cmpswp(data+0, data+1);
    cmpswp(data+3, data+4);
    cmpswp(data+6, data+7);

    cmpswp(data+1, data+2);
    cmpswp(data+4, data+5);
    cmpswp(data+7, data+8);

    cmpswp(data+0, data+1);
    cmpswp(data+3, data+4);
    cmpswp(data+6, data+7);
    cmpswp(data+2, data+5);

    cmpswp(data+0, data+3);
    cmpswp(data+1, data+4);
    cmpswp(data+5, data+8);

    cmpswp(data+3, data+6);
    cmpswp(data+4, data+7);
    cmpswp(data+2, data+5);

    cmpswp(data+0, data+3);
    cmpswp(data+1, data+4);
    cmpswp(data+5, data+7);
    cmpswp(data+2, data+6);

    cmpswp(data+1, data+3);
    cmpswp(data+4, data+6);

    cmpswp(data+2, data+4);
    cmpswp(data+5, data+6);

    cmpswp(data+2, data+3);
}

// Network for N=25, using Bose-Nelson Algorithm.

// There are 154 comparators in this network,
// grouped into 27 parallel operations.

// [[1,2],[4,5],[7,8],[10,11],[13,14],[16,17],[19,20],[21,22],[23,24]]
// [[0,2],[3,5],[6,8],[9,11],[12,14],[15,17],[18,20],[21,23],[22,24]]
// [[0,1],[3,4],[2,5],[6,7],[9,10],[8,11],[12,13],[15,16],[14,17],[18,19],[22,23],[20,24]]
// [[0,3],[1,4],[6,9],[7,10],[5,11],[12,15],[13,16],[18,22],[19,23],[17,24]]
// [[2,4],[1,3],[8,10],[7,9],[0,6],[14,16],[13,15],[18,21],[20,23],[11,24]]
// [[2,3],[8,9],[1,7],[4,10],[14,15],[19,21],[20,22],[16,23]]
// [[2,8],[1,6],[3,9],[5,10],[20,21],[12,19],[15,22],[17,23]]
// [[2,7],[4,9],[12,18],[13,20],[14,21],[16,22],[10,23]]
// [[2,6],[5,9],[4,7],[14,20],[13,18],[17,22],[11,23]]
// [[3,6],[5,8],[14,19],[16,20],[17,21],[0,13],[9,22]]
// [[5,7],[4,6],[14,18],[15,19],[17,20],[0,12],[8,21],[10,22]]
// [[5,6],[15,18],[17,19],[1,14],[7,20],[11,22]]
// [[16,18],[2,15],[1,12],[6,19],[8,20],[11,21]]
// [[17,18],[2,14],[3,16],[7,19],[10,20]]
// [[2,13],[4,17],[5,18],[8,19],[11,20]]
// [[2,12],[5,17],[4,16],[3,13],[9,19]]
// [[5,16],[3,12],[4,14],[10,19]]
// [[5,15],[4,12],[11,19],[9,16],[10,17]]
// [[5,14],[8,15],[11,18],[10,16]]
// [[5,13],[7,14],[11,17]]
// [[5,12],[6,13],[8,14],[11,16]]
// [[6,12],[8,13],[10,14],[11,15]]
// [[7,12],[9,13],[11,14]]
// [[8,12],[11,13]]
// [[9,12]]
// [[10,12]]
// [[11,12]]

// This is graphed in 89 columns.

static void fsort25(int *data)
{
    cmpswp(data+1,  data+2);
    cmpswp(data+4,  data+5);
    cmpswp(data+7,  data+8);
    cmpswp(data+10, data+11);
    cmpswp(data+13, data+14);
    cmpswp(data+16, data+17);
    cmpswp(data+19, data+20);
    cmpswp(data+21, data+22);
    cmpswp(data+23, data+24);

    cmpswp(data+0,  data+2);
    cmpswp(data+3,  data+5);
    cmpswp(data+6,  data+8);
    cmpswp(data+9,  data+11);
    cmpswp(data+12, data+14);
    cmpswp(data+15, data+17);
    cmpswp(data+18, data+20);
    cmpswp(data+21, data+23);
    cmpswp(data+22, data+24);

    cmpswp(data+0,  data+1);
    cmpswp(data+3,  data+4);
    cmpswp(data+2,  data+5);
    cmpswp(data+6,  data+7);
    cmpswp(data+9,  data+10);
    cmpswp(data+8,  data+11);
    cmpswp(data+12, data+13);
    cmpswp(data+15, data+16);
    cmpswp(data+14, data+17);
    cmpswp(data+18, data+19);
    cmpswp(data+22, data+23);
    cmpswp(data+20, data+24);

    cmpswp(data+0,  data+3);
    cmpswp(data+1,  data+4);
    cmpswp(data+6,  data+9);
    cmpswp(data+7,  data+10);
    cmpswp(data+5,  data+11);
    cmpswp(data+12, data+15);
    cmpswp(data+13, data+16);
    cmpswp(data+18, data+22);
    cmpswp(data+19, data+23);
    cmpswp(data+17, data+24);

    cmpswp(data+2,  data+4);
    cmpswp(data+1,  data+3);
    cmpswp(data+8,  data+10);
    cmpswp(data+7,  data+9);
    cmpswp(data+0,  data+6);
    cmpswp(data+14, data+16);
    cmpswp(data+13, data+15);
    cmpswp(data+18, data+21);
    cmpswp(data+20, data+23);
    cmpswp(data+11, data+24);

    cmpswp(data+2,  data+3);
    cmpswp(data+8,  data+9);
    cmpswp(data+1,  data+7);
    cmpswp(data+4,  data+10);
    cmpswp(data+14, data+15);
    cmpswp(data+19, data+21);
    cmpswp(data+20, data+22);
    cmpswp(data+16, data+23);

    cmpswp(data+2,  data+8);
    cmpswp(data+1,  data+6);
    cmpswp(data+3,  data+9);
    cmpswp(data+5,  data+10);
    cmpswp(data+20, data+21);
    cmpswp(data+12, data+19);
    cmpswp(data+15, data+22);
    cmpswp(data+17, data+23);

    cmpswp(data+2,  data+7);
    cmpswp(data+4,  data+9);
    cmpswp(data+12, data+18);
    cmpswp(data+13, data+20);
    cmpswp(data+14, data+21);
    cmpswp(data+16, data+22);
    cmpswp(data+10, data+23);

    cmpswp(data+2,  data+6);
    cmpswp(data+5,  data+9);
    cmpswp(data+4,  data+7);
    cmpswp(data+14, data+20);
    cmpswp(data+13, data+18);
    cmpswp(data+17, data+22);
    cmpswp(data+11, data+23);

    cmpswp(data+3,  data+6);
    cmpswp(data+5,  data+8);
    cmpswp(data+14, data+19);
    cmpswp(data+16, data+20);
    cmpswp(data+17, data+21);
    cmpswp(data+0,  data+13);
    cmpswp(data+9,  data+22);

    cmpswp(data+5,  data+7);
    cmpswp(data+4,  data+6);
    cmpswp(data+14, data+18);
    cmpswp(data+15, data+19);
    cmpswp(data+17, data+20);
    cmpswp(data+0,  data+12);
    cmpswp(data+8,  data+21);
    cmpswp(data+10, data+22);

    cmpswp(data+5,  data+6);
    cmpswp(data+15, data+18);
    cmpswp(data+17, data+19);
    cmpswp(data+1,  data+14);
    cmpswp(data+7,  data+20);
    cmpswp(data+11, data+22);

    cmpswp(data+16, data+18);
    cmpswp(data+2,  data+15);
    cmpswp(data+1,  data+12);
    cmpswp(data+6,  data+19);
    cmpswp(data+8,  data+20);
    cmpswp(data+11, data+21);

    cmpswp(data+17, data+18);
    cmpswp(data+2,  data+14);
    cmpswp(data+3,  data+16);
    cmpswp(data+7,  data+19);
    cmpswp(data+10, data+20);

    cmpswp(data+2,  data+13);
    cmpswp(data+4,  data+17);
    cmpswp(data+5,  data+18);
    cmpswp(data+8,  data+19);
    cmpswp(data+11, data+20);

    cmpswp(data+2,  data+12);
    cmpswp(data+5,  data+17);
    cmpswp(data+4,  data+16);
    cmpswp(data+3,  data+13);
    cmpswp(data+9,  data+19);

    cmpswp(data+5,  data+16);
    cmpswp(data+3,  data+12);
    cmpswp(data+4,  data+14);
    cmpswp(data+10, data+19);

    cmpswp(data+5,  data+15);
    cmpswp(data+4,  data+12);
    cmpswp(data+11, data+19);
    cmpswp(data+9,  data+16);
    cmpswp(data+10, data+17);

    cmpswp(data+5,  data+14);
    cmpswp(data+8,  data+15);
    cmpswp(data+11, data+18);
    cmpswp(data+10, data+16);

    cmpswp(data+5,  data+13);
    cmpswp(data+7,  data+14);
    cmpswp(data+11, data+17);

    cmpswp(data+5,  data+12);
    cmpswp(data+6,  data+13);
    cmpswp(data+8,  data+14);
    cmpswp(data+11, data+16);

    cmpswp(data+6,  data+12);
    cmpswp(data+8,  data+13);
    cmpswp(data+10, data+14);
    cmpswp(data+11, data+15);

    cmpswp(data+7,  data+12);
    cmpswp(data+9,  data+13);
    cmpswp(data+11, data+14);

    cmpswp(data+8,  data+12);
    cmpswp(data+11, data+13);

    cmpswp(data+9, data+12);

    cmpswp(data+10, data+12);

    cmpswp(data+11, data+12);
}

static int fsort_compare(const void *a, const void *b)
{
    return (*((int *) a)) - (*((int *) b));
}

void fsort(int *data, int n)
{
    switch(n) {
        case 1: return;
        case 9: fsort9(data); return;
        case 25: fsort25(data); return;
        default: qsort(data, n, sizeof(int), fsort_compare);
    }
}
