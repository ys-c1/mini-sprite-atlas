#ifndef MINI_SPRITE_ATLAS_H
#define MINI_SPRITE_ATLAS_H

#ifdef __cplusplus
extern "C" {
#endif

/* A source sprite. The atlas is square: atlas_w by atlas_w. */
typedef struct Sprite {
    int w;
    int h;
} Sprite;

/* Result for one sprite. x/y are the sprite's top-left corner in the atlas. */
typedef struct Packed {
    int x;
    int y;
    int placed;
} Packed;

/*
 * Pack n sprites into a square atlas_w x atlas_w atlas.
 * Sprites are ordered by descending height (stable for equal heights), then
 * placed with a padded shelf/skyline scan. padding is the minimum gap between
 * two sprite rectangles.
 *
 * Returns 0 when every sprite was placed. Returns -1 when the input is invalid
 * or not all sprites fit; on failure every out[i].placed is reset to 0.
 */
int pack_atlas(const Sprite *sprites, int n, int atlas_w, int padding,
               Packed *out);

#ifdef __cplusplus
}
#endif

#endif /* MINI_SPRITE_ATLAS_H */
