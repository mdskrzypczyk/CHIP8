#include "graphics.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(VideoInitCheckerTests, TestChecks) {
    VideoInitChecker init_checker;

    EXPECT_EQ(init_checker.check_sdl_init_code(0), true);
    EXPECT_EQ(init_checker.check_sdl_init_code(-1), false);
    void *window = (void *) 1;
    EXPECT_EQ(init_checker.check_sdl_create_window_code((SDL_Window *) window),
              true);
    EXPECT_EQ(init_checker.check_sdl_create_window_code(nullptr), false);
}

TEST(VIDEOTests, TestConstructor) {
    VIDEO video = VIDEO();

    EXPECT_EQ(video.get_pixel_width(), WINDOW_WIDTH / SCREEN_WIDTH);
    EXPECT_EQ(video.get_pixel_height(), WINDOW_HEIGHT / SCREEN_HEIGHT);
    EXPECT_EQ(video.get_window_width(), WINDOW_WIDTH);
    EXPECT_EQ(video.get_window_height(), WINDOW_HEIGHT);
    EXPECT_EQ(video.get_background_color(), BLACK);
    EXPECT_EQ(video.get_foreground_color(), WHITE);
}

TEST(VIDEOTests, DISABLED_TestInit) {
    VIDEO video = VIDEO();

    bool success = video.init();
    EXPECT_EQ(success, true);
}

TEST(VIDEOTests, DISABLED_TestGetColor) {
    VIDEO video = VIDEO();
    video.init();

    EXPECT_EQ(video.get_color(0, 0, 0), BLACK);
    EXPECT_EQ(video.get_color(255, 255, 255), WHITE);
}

TEST(VIDEOTests, DISABLED_TestClear) {
    VIDEO video = VIDEO();
    video.init();

    video.clear();

    // Check that pixel map and video memory initialized to background color
    uint32_t bg_color = video.get_background_color();
    uint32_t *vid_mem = video.get_vid_mem();
    uint32_t(*pix_map)[SCREEN_WIDTH] = video.get_pix_map();
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            EXPECT_EQ(vid_mem[y * SCREEN_WIDTH + x], bg_color);
            EXPECT_EQ(pix_map[y][x], bg_color);
        }
    }
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            EXPECT_EQ(vid_mem[y * SCREEN_WIDTH + x], bg_color);
        }
    }
}

TEST(VIDEOTests, DISABLED_TestDrawPixel) {
    VIDEO video = VIDEO();
    video.init();

    video.clear();

    uint8_t test_x = 0;
    uint8_t test_y = 0;
    uint32_t test_rgb = 0;
    video.draw_pixel(test_x, test_y, test_rgb);

    uint32_t *vid_mem = video.get_vid_mem();
    uint32_t pixel_width = video.get_pixel_width();
    uint32_t pixel_height = video.get_pixel_height();
    uint32_t bg_color = video.get_background_color();

    // Check that only the pixels corresponding to test_x, test_y are colored
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            if (((test_y + 1) * pixel_height > y >= test_y * pixel_height) &&
                ((test_x + 1) * pixel_width > x >= test_x * pixel_width)) {
                EXPECT_EQ(vid_mem[y * SCREEN_WIDTH + x], test_rgb);
            } else {
                EXPECT_EQ(vid_mem[y * SCREEN_WIDTH + x], bg_color);
            }
        }
    }

    // Check that drawing out of bounds does not change anything
    video.draw_pixel(SCREEN_WIDTH, SCREEN_HEIGHT, test_rgb);
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            if (((test_y + 1) * pixel_height > y >= test_y * pixel_height) &&
                ((test_x + 1) * pixel_width > x >= test_x * pixel_width)) {
                EXPECT_EQ(vid_mem[y * SCREEN_WIDTH + x], test_rgb);
            } else {
                EXPECT_EQ(vid_mem[y * SCREEN_WIDTH + x], bg_color);
            }
        }
    }
}

TEST(VIDEOTests, DISABLED_TestXorColor) {
    VIDEO video = VIDEO();
    video.init();
    video.clear();

    // Check that drawing out of bounds does nothing
    EXPECT_EQ(video.xor_color(SCREEN_WIDTH - 1, SCREEN_HEIGHT), false);
    EXPECT_EQ(video.xor_color(SCREEN_WIDTH, SCREEN_HEIGHT - 1), false);
    EXPECT_EQ(video.xor_color(SCREEN_WIDTH, SCREEN_HEIGHT), false);

    uint32_t(*pix_map)[SCREEN_WIDTH] = video.get_pix_map();
    uint32_t fg_color = video.get_foreground_color();
    uint32_t bg_color = video.get_background_color();

    // Pixels are initially background color so expect false
    uint8_t test_x = 0;
    uint8_t test_y = 0;
    EXPECT_EQ(pix_map[test_y][test_x], bg_color);
    EXPECT_EQ(video.xor_color(test_x, test_y), false);
    EXPECT_EQ(pix_map[test_y][test_x], fg_color);

    // When XORing a foreground color we return true
    EXPECT_EQ(video.xor_color(test_x, test_y), true);
    EXPECT_EQ(pix_map[test_y][test_x], bg_color);
}

TEST(VIDEOTests, DISABLED_TestDrawPixMap) {
    VIDEO video = VIDEO();
    video.init();
    video.clear();

    uint32_t(*pix_map)[SCREEN_WIDTH] = video.get_pix_map();

    uint8_t test_x = 0;
    uint8_t test_y = 0;
    uint32_t test_rgb = 0;

    pix_map[test_y][test_x] = test_rgb;
    video.draw_pix_map();

    uint32_t *vid_mem = video.get_vid_mem();
    uint32_t pixel_width = video.get_pixel_width();
    uint32_t pixel_height = video.get_pixel_height();
    uint32_t bg_color = video.get_background_color();

    // Check that only the pixel we changed is updated with foreground color
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            if (((test_y + 1) * pixel_height > y >= test_y * pixel_height) &&
                ((test_x + 1) * pixel_width > x >= test_x * pixel_width)) {
                EXPECT_EQ(vid_mem[y * SCREEN_WIDTH + x], test_rgb);
            } else {
                EXPECT_EQ(vid_mem[y * SCREEN_WIDTH + x], bg_color);
            }
        }
    }
}

TEST(VIDEOTests, DISABLED_TestRandColorScheme) {
    VIDEO video = VIDEO();
    video.init();
    video.clear();

    uint32_t(*pix_map)[SCREEN_WIDTH] = video.get_pix_map();
    uint32_t fg_color = video.get_foreground_color();
    uint32_t bg_color = video.get_background_color();

    // Pixels are initially background color so expect false
    uint8_t test_x = 0;
    uint8_t test_y = 0;
    EXPECT_EQ(pix_map[test_y][test_x], bg_color);
    EXPECT_EQ(video.xor_color(test_x, test_y), false);
    EXPECT_EQ(pix_map[test_y][test_x], fg_color);

    // Change color scheme
    video.rand_color_scheme();

    // Check that colors updated in pixel map
    uint32_t new_fg_color = video.get_foreground_color();
    uint32_t new_bg_color = video.get_background_color();
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if ((x == test_x) && (y == test_y)) {
                EXPECT_EQ(pix_map[y][x], new_fg_color);
            } else {
                EXPECT_EQ(pix_map[y][x], new_bg_color);
            }
        }
    }

    uint32_t *vid_mem = video.get_vid_mem();
    uint32_t pixel_width = video.get_pixel_width();
    uint32_t pixel_height = video.get_pixel_height();

    // Check that colors updated in video memory
    // Iterate through pixels that make up larger Chip-8 pixel and color
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            if (((test_y + 1) * pixel_height > y >= test_y * pixel_height) &&
                ((test_x + 1) * pixel_width > x >= test_x * pixel_width)) {
                // TODO: Figure out why this causes tests to hang
                // EXPECT_EQ(vid_mem[y * SCREEN_WIDTH + x], new_fg_color);
            } else {
                EXPECT_EQ(vid_mem[y * SCREEN_WIDTH + x], bg_color);
            }
        }
    }

    // Check that XORing colors applies new one
    EXPECT_EQ(video.xor_color(test_x, test_y), true);
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            EXPECT_EQ(pix_map[y][x], new_bg_color);
        }
    }

    // Iterate through pixels that make up larger Chip-8 pixel and color
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            EXPECT_EQ(vid_mem[y * SCREEN_WIDTH + x], bg_color);
        }
    }
}
