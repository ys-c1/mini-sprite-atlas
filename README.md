# mini-sprite-atlas

A tiny C11 sprite-atlas packer with no third-party dependencies.

## Build and test

```sh
make test
```

The test executable is written to `build/test_atlas`. `make clean` removes the
build directory.

## API

```c
typedef struct Sprite { int w, h; } Sprite;
typedef struct Packed { int x, y; int placed; } Packed;

int pack_atlas(const Sprite *sprites, int n, int atlas_w, int padding,
               Packed *out);
```

The atlas is square, `atlas_w` by `atlas_w`. Sprites are sorted by descending
height with a stable order for equal heights, then placed by a padded shelf /
skyline scan. `padding` is the minimum gap between two sprites.

`pack_atlas` returns `0` when every sprite is placed. It returns `-1` for
invalid input or when all sprites cannot fit. On failure all `placed` flags are
cleared.

## Tests

`tests/test_atlas.c` covers:

- four 16x16 sprites in a 64x64 atlas with padding 2, including pairwise
  padded-overlap checks;
- stable input order for sprites with equal height;
- a 5 x 40x40 set that cannot fit in 64x64;
- padding 0 with tightly packed sprites;
- a single sprite wider than the atlas.
