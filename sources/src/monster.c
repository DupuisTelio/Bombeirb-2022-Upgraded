#include <SDL/SDL_image.h>
#include <assert.h>
#include<SDL/SDL.h>


#include <game.h>
#include <misc.h>
#include <window.h>
#include <sprite.h>
#include <bomb.h>
#include <time.h>
#include <player.h>
#include <map.h>
#include <dijkstra.h>

struct monster {
	int x, y;
	enum direction direction;
  	int lives;
};

int monster_get_struct_monster_size(){
	return sizeof(struct monster);
}

void monster_set_lives(struct monster* monster, int lives_of_monster){
  assert(monster);
  monster->lives=lives_of_monster;
}

void monster_set_position(struct monster* monster, int x, int y){
  assert(monster);
  monster->x=x;
  monster->y=y;
}

void monster_set_direction(struct monster* monster, enum direction direction){
  assert(monster);
  monster->direction = direction;
}

int monster_get_x(struct monster* monster){
  assert(monster);
  return monster->x;
}

int monster_get_y(struct monster* monster){
  assert(monster);
  return monster->y;
}


enum direction monster_get_direction(struct monster* monster){
	assert(monster);
	return monster->direction;
}

struct monster*  monster_get_correct_one(struct map* map,int x,int y){ 
	struct monster* tab_monster=map_get_tab_monster(map);
	int i=0;
	while ((&tab_monster[i])->lives >0){
		if ( (x==monster_get_x(&tab_monster[i]) )&&(y==monster_get_y(&tab_monster[i]) ) ){  //we match the position of the targeted cell with the position of each monsters
			return (&tab_monster[i]);
		}
		i++;
	}	
	struct monster* monster = malloc(sizeof(monster));
	monster->direction=SOUTH;
	return monster; //just for the case where there is a monster on a CELL and not yet in the tab (at the beginning)
}

void monster_init_tab(struct monster* tab_monster){
	for (int i = 0; i < 14; i++){
		monster_set_lives(&tab_monster[i],0);
	}
}

struct monster* monster_init(int x, int y, int lives) {
	struct monster* monster = malloc(sizeof(monster));
	assert(monster);
	monster->lives=lives ;
	monster->direction = NORTH ;
	monster-> x = x;
	monster->y = y;
	return monster;
}

void insert_monster_in_tab(struct map* map,struct monster* monster){
	struct monster* tab_monster=map_get_tab_monster(map);
	int i = 0;
	while ((&tab_monster[i])->lives >0){
		i++;
	}
	monster_set_lives(&tab_monster[i],monster->lives);
	monster_set_direction(&tab_monster[i],monster->direction);
	monster_set_position(&tab_monster[i],monster->x,monster->y);
}



void monster_spawn(int x, int y,struct map* map){
	map_set_cell_type(map,x,y,CELL_MONSTER);
	insert_monster_in_tab(map,monster_init(x,y,map_get_lives_monster(map)));	
}

void monsters_get_monsters_of_map(struct map* map){
	for (int i = 0; i < map_get_width(map); i++) {
	  for (int j = 0; j < map_get_height(map);j++) {
		  if (map_get_cell_type(map, i, j) == CELL_MONSTER){
			  monster_spawn(i,j,map);
		  }
	  }
	}
}

static int a_door_or_a_princess_is_close (struct map* map, int x, int y){ 
	if (map_is_inside(map, x-1, y))
		if ( (map_get_cell_type(map,x-1,y)==CELL_DOOR)||(map_get_scenery_subtype(map,x-1,y)==SCENERY_PRINCESS) )
			return 1;
	if (map_is_inside(map, x+1, y))
		if ( (map_get_cell_type(map,x+1,y)==CELL_DOOR)|| (map_get_scenery_subtype(map,x+1,y)==SCENERY_PRINCESS) )
			return 1;
	if (map_is_inside(map, x, y-1))
		if ( (map_get_cell_type(map,x,y-1)==CELL_DOOR)|| (map_get_scenery_subtype(map,x,y-1)==SCENERY_PRINCESS) )			
			return 1;
	if (map_is_inside(map, x, y+1))
		if ( (map_get_cell_type(map,x,y+1)==CELL_DOOR)|| (map_get_scenery_subtype(map,x,y+1)==SCENERY_PRINCESS) )			
			return 1;
	return 0;
}


int monster_move_aux(struct map* map, int x, int y) { //return if the monster can or not go on the x y cell of the map
	if (!map_is_inside(map, x, y))
		return 0;

	if (a_door_or_a_princess_is_close(map,x,y)) //if a door is at less than 1 case of the targeted cell, the monster can't enter this cell
		return 0;
	
	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY:
		return 0;
		break;

	case CELL_BOX:
		return 0;
		break;

	case CELL_BONUS:
		return 0;
		break;

	case CELL_MONSTER:
		return 0;
		break;

	case CELL_BOMB:
		if (map_get_bomb_subtype(map,x,y) == BOMB_EXPLOSION){ // (CELL_BOMB & BOMB_EXPLOSION)
			return 1;
		}
		return 0;
		break;
	case CELL_KEY:
		return 0;
		break;
	case CELL_DOOR:
		return 0;
		break;
	case CELL_BOSS:
		return 0;
		break;
	default:
		break;
	}
	// Monster has moved
	return 1;
}

void monster_hit_on_player(struct player* player,int x, int y){
	if ((x==player_get_x(player)&&(y==player_get_y(player))))
		player_dec_nb_lives(player,1);
}

void monster_move(struct monster* monster, struct map* map,struct player* player) {
	int x = monster->x;
	int y = monster->y;
	switch (monster->direction) {
	case NORTH:
		if (monster_move_aux( map, x, y - 1)) {
			monster_hit_on_player(player,x,y-1);
			map_set_cell_type(map,x,y,CELL_EMPTY);
			map_set_cell_type(map,x,y-1,CELL_MONSTER);
			monster->y--;
		}
		break;
	case SOUTH:
		if (monster_move_aux( map, x, y + 1)) {
			monster_hit_on_player(player,x,y+1);
			map_set_cell_type(map,x,y,CELL_EMPTY);
			map_set_cell_type(map,x,y+1,CELL_MONSTER);
			monster->y++;
		}
		break;
	case WEST:
		if (monster_move_aux(map, x - 1, y)) {
			monster_hit_on_player(player,x-1,y);
			map_set_cell_type(map,x,y,CELL_EMPTY);
			map_set_cell_type(map,x-1,y,CELL_MONSTER);
			monster->x--;
		}
		break;
	case EAST:
		if (monster_move_aux( map, x + 1, y)) {
			monster_hit_on_player(player,x+1,y);
			map_set_cell_type(map,x,y,CELL_EMPTY);
			map_set_cell_type(map,x+1,y,CELL_MONSTER);
			monster->x++;
		}
		break;
	}
}


		

int monster_random_select_available_cells(struct map* map,struct monster* monster){ //to avoid the case of a monster moving in a wall (or something he can't walk on) all the time, even if because of probability he won't get stuck this way
	int x=monster_get_x(monster);
	int y=monster_get_y(monster);
	int North =monster_move_aux(map,x,y-1);
	int South =monster_move_aux(map,x,y+1);
	int West =monster_move_aux(map,x-1,y);
	int East =monster_move_aux(map,x+1,y);	
	if (North+South+West+East){  //if at least on cell is available around the monster
		while(1){
			int Random = rand()%4;
			if (North&&(Random==0)){
				return Random;
			}
			if (South&&(Random==1)){
				return Random;
			}
			if (East&&(Random==2)){
				return Random;
			}
			if (West&&(Random==3)){
				return Random;
			}
		}
	}
	return (monster->direction); //else he is stuck so he doesn't move
}

void monster_choose_random_path(struct map* map, struct monster* monster,struct player* player){
	int direction=monster_random_select_available_cells(map,monster);
	if(direction==0){
		monster_set_direction(monster,NORTH);
	} //Case North
	if(direction==1){
		monster_set_direction(monster,SOUTH);
	} //Case SOUTH
	if(direction==2){
		monster_set_direction(monster,EAST);
	} //Case EAST
	if(direction==3){
		monster_set_direction(monster,WEST);
	} //Case WEST
	monster_move(monster,map,player);

}

void monster_dijkstra_choose_path(struct map* map, struct monster* monster,struct player* player,int x,int y){
	int x_direction=x-monster_get_x(monster);
	int y_direction=y-monster_get_y(monster);
	if(y_direction==-1){
		monster_set_direction(monster,NORTH);
	} //Case North
	if(y_direction==1){
		monster_set_direction(monster,SOUTH);
	} //Case SOUTH
	if(x_direction==1){
		monster_set_direction(monster,EAST);
	} //Case EAST
	if(x_direction==-1){
		monster_set_direction(monster,WEST);
	} //Case WEST
	monster_move(monster,map,player);

}



void monster_update(struct map* map, struct player* player){ //update all the bombs
	struct monster* tab_monster= map_get_tab_monster(map);
	for (int i = 0; i < 14; i++){
		if ((&tab_monster[i])->lives >0){
			struct dijkstra* dijkstra= dijkstra_choose_path(map,monster_get_x(&tab_monster[i]),monster_get_y(&tab_monster[i]),player);
			if (dijkstra_get_Choose_of_path(dijkstra)){                        //if the monster can reach the player we use dijsktra to choose a path
				int x= dijkstra_get_x(dijkstra);
				int y= dijkstra_get_y(dijkstra);
				monster_dijkstra_choose_path(map,&tab_monster[i],player,x,y);
			}
			else {
				monster_choose_random_path(map, &tab_monster[i],player);  //else we pick a random path
			}
		}
	}
}


void monster_hit(struct map* map,int x,int y,int damage){
	struct monster* monster = monster_get_correct_one(map,x,y);
	assert(monster);
	if ((monster->lives)<=damage){
		monster->lives=0; //kill the monster
	}
	else {
		monster->lives--;
	}
}




