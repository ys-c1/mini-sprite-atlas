#include "atlas.h"

#include <stdlib.h>

static void clear_output(Packed *out, int n)
{
    int i;

    for (i = 0; i < n; ++i) {
        out[i].x = 0;
        out[i].y = 0;
        out[i].placed = 0;
    }
}

/* Stable insertion sort of sprite indices by descending width. */
static void sort_by_width(const Sprite *sprites, int *order, int n)
{
    int i;

    for (i = 1; i < n; ++i) {
        int key = order[i];
        int j = i - 1;

        while (j >= 0 && sprites[order[j]].w < sprites[key].w) {
            order[j + 1] = order[j];
            --j;
        }
        order[j + 1] = key;
    }
}

/*
 * Return 1 when the candidate rectangle is closer than padding to an already
 * placed sprite. Rectangles may touch exactly at a padding-sized gap.
 */
static int collides(const Sprite *sprites, const Packed *out, int n,
                    int x, int y, int w, int h, int padding)
{
    int i;

    for (i = 0; i < n; ++i) {
        int sep_x;
        int sep_y;

        if (!out[i].placed) {
            continue;
        }

        sep_x = (out[i].x + sprites[i].w + padding <= x) ||
                (x + w + padding <= out[i].x);
        sep_y = (out[i].y + sprites[i].h <= y) ||
                (y + h <= out[i].y);

        if (!sep_x && !sep_y) {
            return 1;
        }
    }

    return 0;
}

int pack_atlas(const Sprite *sprites, int n, int atlas_w, int padding,
               Packed *out)
{
    int *order;
    int i;
    int all_placed = 1;

    if (n < 0 || atlas_w <= 0 || padding < 0) {
        return -1;
    }
    if (n > 0 && (sprites == NULL || out == NULL)) {
        return -1;
    }
    if (n == 0) {
        return 0;
    }

    clear_output(out, n);

    order = (int *)malloc((size_t)n * sizeof(*order));
    if (order == NULL) {
        return -1;
    }
    for (i = 0; i < n; ++i) {
        order[i] = i;
    }

    sort_by_width(sprites, order, n);

    for (i = 0; i < n; ++i) {
        int idx = order[i];
        int w = sprites[idx].w;
        int h = sprites[idx].h;
        int x;
        int y;
        int placed = 0;

        if (w <= 0 || h <= 0 || w > atlas_w || h > atlas_w) {
            all_placed = 0;
            break;
        }

        for (y = 0; y <= atlas_w - h && !placed; ++y) {
            for (x = 0; x <= atlas_w - w; ++x) {
                if (!collides(sprites, out, n, x, y, w, h, padding)) {
                    out[idx].x = x;
                    out[idx].y = y;
                    out[idx].placed = 1;
                    placed = 1;
                    break;
                }
            }
        }

        if (!placed) {
            all_placed = 0;
            break;
        }
    }

    free(order);

    if (!all_placed) {
        clear_output(out, n);
        return 0;
    }

    return 0;
}
