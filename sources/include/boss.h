#ifndef BOSS_H_
#define BOSS_H_

#include <map.h>
#include <player.h>

struct boss;

//set lives of boss
void boss_set_lives(struct boss* boss);

//update boss
void boss_update(struct map* map, struct player* player);

//get direction of a boss
enum direction boss_get_direction(struct boss* boss);

//reduce boss life of 1 or kill it
void boss_hit(struct map* map,int damage);

//get size of struct boss
int boss_get_struct_boss_size();

//get the boss displayed on the map (if there is one)
void boss_get_boss_of_map(struct map* map);




//get x and y,damage,wait_timer of the boss
int boss_get_x(struct boss* boss);
int boss_get_y(struct boss* boss);
int boss_get_damage(struct boss* boss);
int boss_get_wait_timer(struct boss* boss);




#endif /* BOSS_H_ */
