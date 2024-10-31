#include "rterm.h"

void before_cursor_move(rterm_t *rterm) {
    // printf("Before cursor update: %d:%d\n",rterm->cursor.x,rterm->cursor.y);
}
void after_cursor_move(rterm_t *rterm) {

    // rterm->cursor.x++;
}
void before_key_press(rterm_t *rterm) {

    // if(rterm->key.c == 65 && rterm->key.escape){
    //     rterm->key.c = 66;
    //}
}
void tick(rterm_t *rt) {
    static char status_text[1024];
    status_text[0] = 0;
    sprintf(status_text, "\rp:%d:%d | k:%c:%d | i:%ld ", rt->cursor.x + 1, rt->cursor.y + 1, rt->key.c == 0 ? '0' : rt->key.c, rt->key.c,
            rt->iterations);
    rt->status_text = status_text;
}

int main() {
    rterm_t rt;
    rterm_init(&rt);
    rt.show_cursor = true;
    rt.before_key_press = before_key_press;
    rt.before_cursor_move = before_cursor_move;
    rt.after_cursor_move = after_cursor_move;
    rt.tick = tick;
    rterm_loop(&rt);
    return 0;
}