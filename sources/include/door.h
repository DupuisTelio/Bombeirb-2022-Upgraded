#ifndef DOOR_H_
#define DOOR_H_

int door_player_encounter_door(struct player* player, struct map* map, int x, int y);
void door_is_player_on_door(struct game* game);

#endif /* DOOR_H_ */
