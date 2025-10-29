#include <window.h>
#include <constant.h>
#include <misc.h>
#include <game.h>
#include <sprite.h>


void screen_gameover_display(){
    window_display_image(sprite_get_gameover(),0,0);
    window_refresh();
}

void screen_win_display(){
    window_display_image(sprite_get_win(),0,0);
    window_refresh();
}

void screen_break_display(){
    window_display_image(sprite_get_break(),100,150);
    window_refresh();
}


