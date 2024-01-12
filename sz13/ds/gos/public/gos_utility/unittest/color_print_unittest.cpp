#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos::color_print ºÚµ•≤‚ ‘ ", "[gos][color_print]")
{
    /// printf("\x1b[0m Origin color\r\n");
    /// printf("\x1b[1;37m White color\r\n");
    /// printf("\x1b[1;34m Blue color\r\n");
    /// printf("\x1b[1;32m Green color\r\n");
    /// printf("\x1b[1;33m Yellow color\r\n");
    /// printf("\x1b[1;31m Red color\r\n");
    /// printf("\x1b[0m Origin color\r\n");

    gos::printf_color(gos::LOG_COLOR_ORIGIN, "Origin test color_print\n");
    gos::printf_color(gos::LOG_COLOR_WHITE, "White test color_print\n");
    gos::printf_color(gos::LOG_COLOR_BLUE, "Blue test color_print\n");
    gos::printf_color(gos::LOG_COLOR_GREEN, "Green test color_print\n");
    gos::printf_color(gos::LOG_COLOR_YELLOW, "Yellow test color_print\n");
    gos::printf_color(gos::LOG_COLOR_RED, "Red test color_print\n");
}
#endif
