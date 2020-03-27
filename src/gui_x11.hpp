/*
 * Copyright (c) 2009 Tias Guns
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#pragma once

#include <vector>
#include <X11/Xlib.h>
#include <functional>
#include <utility>

enum { BLACK=0, WHITE=1, GRAY=2, DIMGRAY=3, RED=4 };
inline const int nr_colors = 5;
/*
 * Number of blocks. We partition the screen into 'num_blocks' x 'num_blocks'
 * rectangles of equal size. We then ask the user to press points that are
 * located at the corner closes to the center of the four blocks in the corners
 * of the screen. The following ascii art illustrates the situation. We partition
 * the screen into 8 blocks in each direction. We then let the user press the
 * points marked with 'O'.
 *
 *   +--+--+--+--+--+--+--+--+
 *   |  |  |  |  |  |  |  |  |
 *   +--O--+--+--+--+--+--O--+
 *   |  |  |  |  |  |  |  |  |
 *   +--+--+--+--+--+--+--+--+
 *   |  |  |  |  |  |  |  |  |
 *   +--+--+--+--+--+--+--+--+
 *   |  |  |  |  |  |  |  |  |
 *   +--+--+--+--+--+--+--+--+
 *   |  |  |  |  |  |  |  |  |
 *   +--+--+--+--+--+--+--+--+
 *   |  |  |  |  |  |  |  |  |
 *   +--+--+--+--+--+--+--+--+
 *   |  |  |  |  |  |  |  |  |
 *   +--O--+--+--+--+--+--O--+
 *   |  |  |  |  |  |  |  |  |
 *   +--+--+--+--+--+--+--+--+
 */
inline const int num_blocks = 8;

class Points {
public:
    struct XY {
        int x;
        int y;
    };
private:
    std::vector<XY> points = {};
    bool mis_click = false;

    void _reset() {
            points.clear();
            mis_click = false;
    }
    void check_misclick() {
        /* TBD */
    }

public:
    void add_click(int x, int y) {
        points.push_back({x, y});
    }
    void reset() { _reset(); }
    std::vector<XY> data() { return points; }
    size_t size() { return points.size(); }

};

/*******************************************
 * X11 class for the the calibration GUI
 *******************************************/
class GuiCalibratorX11
{
public:
    ~GuiCalibratorX11();
    bool mainloop();
    GuiCalibratorX11();

private:
    void give_timer_signal();

    // Data
    double X[4], Y[4];
    int display_width, display_height;
    int time_elapsed;

    // X11 vars
    Display* display;
    int screen_num;
    Window win;
    GC gc;
    XFontStruct* font_info;
    // color mngmt
    unsigned long pixel[nr_colors];


    // Signal handlers
    void on_timer_signal();
    void on_expose_event();
    void on_button_press_event(XEvent event);

    // Helper functions
    void set_display_size(int width, int height);
    void redraw();
    void draw_message(const char* msg);

    Points points;
    static void sigalarm_handler(int num);
    inline static GuiCalibratorX11 *the_instance = nullptr;
    bool return_value;
    bool do_loop;

    std::function<bool(int, int)> add_click_ext = [](int x, int y){ return true; };
    std::function<void(void)> reset_ext = [](){ };

public:
    void set_add_click(std::function<bool(int, int)> f) {
        add_click_ext = f;
    }
    void set_reset(std::function<void(void)> f) {
        reset_ext = f;
    }

    const std::vector<Points::XY> get_points() { return points.data(); }
    std::pair<int, int> get_display_size() { return {display_width,
                                                        display_height}; }
};