#ifndef MONSTER_H_
#define MONTSER_H_

#include <map.h>
#include <player.h>

struct monster;

//init the tab
void monster_init_tab(struct monster* tab_monster);

//make a monster spawn on map at x y
void monster_spawn(int x, int y,struct map* map);

//update every monster of the map
void monster_update(struct map* map, struct player* player);

//get direction of a monster
enum direction monster_get_direction(struct monster* monster);

//reduce monster life of 1 or kill it
void monster_hit(struct map* map,int x,int y,int damage);

//get size of struct monster
int monster_get_struct_monster_size();

//get all the monsters displayed on a map
void monsters_get_monsters_of_map(struct map* map);

//return if the monster can access the x y cell of the map
int monster_move_aux( struct map* map, int x, int y) ;

//find the monster that is on the targeted cell
struct monster* monster_get_correct_one(struct map* map, int x, int y);

//get x and y of a monster
int monster_get_x(struct monster* monster);
int monster_get_y(struct monster* monster);





#endif /* MONSTER_H_ */
