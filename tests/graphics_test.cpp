#include "graphics.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(VideoInitCheckerTests, TestChecks) {
    VideoInitChecker init_checker;

    EXPECT_EQ(init_checker.check_sdl_init_code(0), true);
    EXPECT_EQ(init_checker.check_sdl_init_code(-1), false);
    void *window = (void *) 1;
    EXPECT_EQ(init_checker.check_sdl_create_window_code((SDL_Window *)window), true);
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

TEST(VIDEOTests, TestInit) {
    VIDEO video = VIDEO();

    bool success = video.init();
    EXPECT_EQ(success, true);
}
