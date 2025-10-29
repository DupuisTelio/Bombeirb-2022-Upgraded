#ifndef DIJKSTRA_H_
#define DIJKSTRA_H_

struct dijkstra;

//get x y choose_of_path of dijkstra
int dijkstra_get_x(struct dijkstra* dijkstra);
int dijkstra_get_y(struct dijkstra* dijkstra);
int dijkstra_get_Choose_of_path(struct dijkstra* dijkstra);

//choose the correct way to move (random or dijkstra) and apply it
struct dijkstra* dijkstra_choose_path (struct map* map,int monster_x,int monster_y, struct player* player);


 
#endif /* DIJKSTRA_H_ */
