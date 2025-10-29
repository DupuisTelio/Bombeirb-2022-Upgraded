
#ifndef BOMB_H_
#define BOMB_H_

#include <player.h>
#include <map.h>

struct bomb ;
struct explosion ;

//start a bomb
void bomb_start(struct player* player,struct map* map);

//deal with one case for an explosion (used in boss.c)
int bomb_explosion_aux(struct player* player, struct map* map,int x, int y,int damage);

//makes a bomb exploses
void bomb_explosion(struct bomb* bomb,struct map* map, int x, int y);

//update all bombs
void bomb_update (struct map* map,struct player* player);

//update all explosion
void bomb_explosion_update(struct map* map);

//update the timer of every bomb after a break to prevent them to explose at the end of the break
void bomb_timer_break_update (int break_duration,struct map* map);

//start the tab bombs and the tab explosion
void bomb_init_tab_bombs(struct bomb* tab_bombs);
void bomb_init_tab_explosion(struct explosion* tab_explosion,int size);

// get size of struct explosion and struct bomb
int bomb_get_struct_explosion_size();
int bomb_get_struct_bomb_size();

#endif /* BOMB_H_ */
