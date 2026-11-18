#include "atlas.h"

#include <stdio.h>
#include <string.h>

static int checks = 0;
static int failures = 0;

#define CHECK(cond)                                                          \
    do {                                                                     \
        ++checks;                                                            \
        if (cond) {                                                          \
            printf("PASS: %s\n", #cond);                                     \
        } else {                                                             \
            ++failures;                                                      \
            printf("FAIL: %s (%s:%d)\n", #cond, __FILE__, __LINE__);         \
        }                                                                    \
    } while (0)

static int count_placed(const Packed *out, int n)
{
    int i;
    int count = 0;

    for (i = 0; i < n; ++i) {
        if (out[i].placed) {
            ++count;
        }
    }
    return count;
}

static int all_inside(const Sprite *sprites, const Packed *out, int n,
                      int atlas_w)
{
    int i;

    for (i = 0; i < n; ++i) {
        if (!out[i].placed) {
            return 0;
        }
        if (out[i].x < 0 || out[i].y < 0) {
            return 0;
        }
        if (out[i].x + sprites[i].w > atlas_w ||
            out[i].y + sprites[i].h > atlas_w) {
            return 0;
        }
    }
    return 1;
}

static int all_same_y(const Packed *out, int n)
{
    int i;

    for (i = 1; i < n; ++i) {
        if (out[i].y != out[0].y) {
            return 0;
        }
    }
    return 1;
}

/*
 * Two sprite rectangles are legal only when there is at least padding between
 * them on the x axis or on the y axis. With padding == 0, touching is legal.
 */
static void check_pairwise_padding(const Sprite *sprites, const Packed *out,
                                   int n, int padding)
{
    int i;
    int j;

    for (i = 0; i < n; ++i) {
        for (j = i + 1; j < n; ++j) {
            int sep_x;
            int sep_y;
            int ok;

            ++checks;
            if (!out[i].placed || !out[j].placed) {
                printf("PASS: pair %d-%d (not both placed)\n", i, j);
                continue;
            }

            sep_x = (out[i].x + sprites[i].w + padding <= out[j].x) ||
                    (out[j].x + sprites[j].w + padding <= out[i].x);
            sep_y = (out[i].y + sprites[i].h + padding <= out[j].y) ||
                    (out[j].y + sprites[j].h + padding <= out[i].y);
            ok = sep_x || sep_y;

            if (ok) {
                printf("PASS: pair %d-%d padding respected\n", i, j);
            } else {
                ++failures;
                printf("FAIL: pair %d-%d overlaps after padding "
                       "(a=%d,%d %dx%d b=%d,%d %dx%d pad=%d)\n",
                       i, j,
                       out[i].x, out[i].y, sprites[i].w, sprites[i].h,
                       out[j].x, out[j].y, sprites[j].w, sprites[j].h,
                       padding);
            }
        }
    }
}

static void test_four_16_with_padding(void)
{
    Sprite sprites[4] = {
        {16, 16}, {16, 16}, {16, 16}, {16, 16}
    };
    Packed out[4];
    int rc;

    printf("\n[test_four_16_with_padding]\n");
    memset(out, 0xA5, sizeof(out));
    rc = pack_atlas(sprites, 4, 64, 2, out);

    CHECK(rc == 0);
    CHECK(count_placed(out, 4) == 4);
    CHECK(all_inside(sprites, out, 4, 64));
    check_pairwise_padding(sprites, out, 4, 2);
}

static void test_stable_same_height_order(void)
{
    Sprite sprites[4] = {
        {6, 10}, {4, 10}, {8, 10}, {2, 10}
    };
    Packed out[4];
    int rc;
    int i;
    int j;

    printf("\n[test_stable_same_height_order]\n");
    memset(out, 0xA5, sizeof(out));
    rc = pack_atlas(sprites, 4, 30, 1, out);

    CHECK(rc == 0);
    CHECK(count_placed(out, 4) == 4);
    CHECK(all_same_y(out, 4));

    /* Equal heights must retain input order, so x must be strictly rising. */
    for (i = 0; i < 4; ++i) {
        for (j = i + 1; j < 4; ++j) {
            CHECK(out[i].x < out[j].x);
        }
    }

    check_pairwise_padding(sprites, out, 4, 1);
}

static void test_does_not_fit(void)
{
    Sprite sprites[5] = {
        {40, 40}, {40, 40}, {40, 40}, {40, 40}, {40, 40}
    };
    Packed out[5];
    int rc;

    printf("\n[test_does_not_fit]\n");
    memset(out, 0xA5, sizeof(out));
    rc = pack_atlas(sprites, 5, 64, 0, out);

    CHECK(rc == -1);
    CHECK(count_placed(out, 5) == 0);
}

static void test_padding_zero_tight(void)
{
    Sprite sprites[4] = {
        {16, 16}, {16, 16}, {16, 16}, {16, 16}
    };
    Packed out[4];
    int rc;
    int min_x = 0;
    int min_y = 0;
    int max_x = 0;
    int max_y = 0;
    int i;
    int bbox_area;

    printf("\n[test_padding_zero_tight]\n");
    memset(out, 0xA5, sizeof(out));
    rc = pack_atlas(sprites, 4, 64, 0, out);

    CHECK(rc == 0);
    CHECK(count_placed(out, 4) == 4);
    CHECK(all_inside(sprites, out, 4, 64));
    check_pairwise_padding(sprites, out, 4, 0);

    if (count_placed(out, 4) == 4) {
        min_x = max_x = out[0].x;
        min_y = max_y = out[0].y;
        for (i = 1; i < 4; ++i) {
            if (out[i].x < min_x) {
                min_x = out[i].x;
            }
            if (out[i].y < min_y) {
                min_y = out[i].y;
            }
            if (out[i].x + sprites[i].w > max_x) {
                max_x = out[i].x + sprites[i].w;
            }
            if (out[i].y + sprites[i].h > max_y) {
                max_y = out[i].y + sprites[i].h;
            }
        }
        bbox_area = (max_x - min_x) * (max_y - min_y);
        CHECK(bbox_area == 4 * 16 * 16);
    }
}

static void test_sprite_wider_than_atlas(void)
{
    Sprite sprites[1] = {
        {65, 8}
    };
    Packed out[1];
    int rc;

    printf("\n[test_sprite_wider_than_atlas]\n");
    memset(out, 0xA5, sizeof(out));
    rc = pack_atlas(sprites, 1, 64, 0, out);

    CHECK(rc == -1);
    CHECK(out[0].placed == 0);
}

int main(void)
{
    test_four_16_with_padding();
    test_stable_same_height_order();
    test_does_not_fit();
    test_padding_zero_tight();
    test_sprite_wider_than_atlas();

    printf("\nSUMMARY checks=%d failures=%d\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
