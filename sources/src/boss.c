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
#include <monster.h>
#include <map.h>
#include <dijkstra.h>

struct boss {
	int x, y;
	enum direction direction;
  	int lives;
    int timer_before_blow;
	int wait_timer;
    int damage;
};

int boss_get_max_size_of_map(struct map* map){ //to get the biggest between height and width of a map
	if (map_get_width(map)>map_get_height(map)){
		return map_get_width(map);
	}
	else {
		return map_get_height(map);
	}
}

int boss_get_struct_boss_size(){
	return sizeof(struct boss);
}

void boss_set_lives(struct boss* boss, int lives_of_boss){
  assert(boss);
  boss->lives=lives_of_boss;
}

void boss_set_position(struct boss* boss, int x, int y){
  assert(boss);
  boss->x=x;
  boss->y=y;
}

void boss_set_direction(struct boss* boss, enum direction direction){
  assert(boss);
  boss->direction = direction;
}

void boss_set_damage(struct boss* boss,int damage){
	assert(boss);
	boss->damage = damage;
}

void boss_set_timer_before_blow(struct boss* boss,int timer){
	assert(boss);
	boss->timer_before_blow = timer;
}

void boss_set_wait_timer(struct boss* boss,int timer){
	assert(boss);
	boss->wait_timer= timer;
}

int boss_get_x(struct boss* boss){
  assert(boss);
  return boss->x;
}

int boss_get_y(struct boss* boss){
  assert(boss);
  return boss->y;
}

enum direction boss_get_direction(struct boss* boss){
	assert(boss);
	return boss->direction;
}

int boss_get_damage(struct boss* boss){
	assert(boss);
	return boss->damage;
}

int boss_get_wait_timer(struct boss* boss){
	assert(boss);
	return boss->wait_timer;
}

void boss_init(struct map* map,int i, int j){
	boss_set_lives(map_get_boss(map),10);
	boss_set_direction(map_get_boss(map),NORTH);
	boss_set_position(map_get_boss(map),i,j);
	boss_set_damage(map_get_boss(map),3);
	boss_set_timer_before_blow(map_get_boss(map),SDL_GetTicks()+10000); //We want to let at least 10s before the first boss blow
	boss_set_wait_timer(map_get_boss(map),-1000); // we want the boss to move until the first blow (have a look on boss update line 278)
}

void boss_get_boss_of_map(struct map* map){
	for (int i = 0; i < map_get_width(map); i++) {
	    for (int j = 0; j < map_get_height(map);j++) {
		    if (map_get_cell_type(map, i, j) == CELL_BOSS){
	            boss_init(map,i,j);	
		    }
	    }
	}
}

void boss_hit_on_player(struct player* player,int x, int y,int damage){
	if ((x==player_get_x(player)&&(y==player_get_y(player))))
		player_dec_nb_lives(player,damage);
}

void boss_move(struct boss* boss, struct map* map,struct player* player) {
	int x = boss->x;
	int y = boss->y;
	int damage = boss->damage;
	switch (boss->direction) {
	case NORTH:
		if (monster_move_aux( map, x, y - 1)) {
			boss_hit_on_player(player,x,y-1,damage);
			map_set_cell_type(map,x,y,CELL_EMPTY);
			map_set_cell_type(map,x,y-1,CELL_BOSS);
			boss->y--;
		}
		break;
	case SOUTH:
		if (monster_move_aux( map, x, y + 1)) {
			boss_hit_on_player(player,x,y+1,damage);
			map_set_cell_type(map,x,y,CELL_EMPTY);
			map_set_cell_type(map,x,y+1,CELL_BOSS);
			boss->y++;
		}
		break;
	case WEST:
		if (monster_move_aux(map, x - 1, y)) {
			boss_hit_on_player(player,x-1,y,damage);
			map_set_cell_type(map,x,y,CELL_EMPTY);
			map_set_cell_type(map,x-1,y,CELL_BOSS);
			boss->x--;
		}
		break;
	case EAST:
		if (monster_move_aux( map, x + 1, y)) {
			boss_hit_on_player(player,x+1,y,damage);
			map_set_cell_type(map,x,y,CELL_EMPTY);
			map_set_cell_type(map,x+1,y,CELL_BOSS);
			boss->x++;
		}
		break;
	}
}

int boss_random_select_available_cells(struct map* map,struct boss* boss){ //to avoid the case of a monster moving in a wall (or something he can't walk on) all the time, even if because of probability he won't get stuck this way
	int x=boss_get_x(boss);
	int y=boss_get_y(boss);
	int North =monster_move_aux(map,x,y-1); // the boss have access at the same cases of monsters
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
	return (boss->direction); //else he is stuck so he doesn't move
}

void boss_choose_random_path(struct map* map, struct boss* boss,struct player* player){
	int direction=boss_random_select_available_cells(map,boss);
	if(direction==0){
		boss_set_direction(boss,NORTH);
	} //Case North
	if(direction==1){
		boss_set_direction(boss,SOUTH);
	} //Case SOUTH
	if(direction==2){
		boss_set_direction(boss,EAST);
	} //Case EAST
	if(direction==3){
		boss_set_direction(boss,WEST);
	} //Case WEST
	boss_move(boss,map,player);

}

void boss_dijkstra_choose_path(struct map* map, struct boss* boss,struct player* player,int x,int y){
	int x_direction=x-boss_get_x(boss);
	int y_direction=y-boss_get_y(boss);
	if(y_direction==-1){
		boss_set_direction(boss,NORTH);
	} //Case North
	if(y_direction==1){
		boss_set_direction(boss,SOUTH);
	} //Case SOUTH
	if(x_direction==1){
		boss_set_direction(boss,EAST);
	} //Case EAST
	if(x_direction==-1){
		boss_set_direction(boss,WEST);
	} //Case WEST
	boss_move(boss,map,player);
}

int is_this_case_explosable(struct map* map, int x, int y){
	switch (map_get_cell_type(map, x, y)) {
		case CELL_MONSTER:
			return 0;
			break;
		case CELL_EMPTY:
			return 0;
			break;
		default:
			return 1;
			break;
	}
}

int there_is_a_player_on_a_direct_line(struct map* map,int player_x,int player_y,struct boss* boss){
	int N = 0; //North
	int S = 0; //South
	int W = 0; //West
	int E = 0; //East
	int boss_x=boss_get_x(boss);
	int boss_y=boss_get_y(boss);
	for (int i = 1; i<= boss_get_max_size_of_map(map); i++){ //let's make a the vision cross of the boss

		if (map_is_inside(map,boss_x,boss_y-i) && (N==0)){
			N+=is_this_case_explosable(map,boss_x,boss_y-i);      //as soon as you encounter a door, a key, a bonus, a box, a tree, a rock, ... we don't consider anymore this direction
			if ((player_x==boss_x)&&(player_y==boss_y-i)){
				boss_set_direction(boss,NORTH);
				return 1;
			}
		}

		if (map_is_inside(map,boss_x,boss_y+i) && (S==0)){
			S+=is_this_case_explosable(map,boss_x,boss_y+i);
			if ((player_x==boss_x)&&(player_y==boss_y+i)){
				boss_set_direction(boss,SOUTH);
				return 1;
			}
		}

		if (map_is_inside(map,boss_x-i,boss_y) && (W==0)){
			W+=is_this_case_explosable(map,boss_x-i,boss_y);
			if ((player_x==boss_x-i)&&(player_y==boss_y)){
				boss_set_direction(boss,WEST);
				return 1;
			}
		}

		if (map_is_inside(map,boss_x+i,boss_y) && (E==0)){
			E+=is_this_case_explosable(map,boss_x+i,boss_y);
			if ((player_x==boss_x+i)&&(player_y==boss_y)){
				boss_set_direction(boss,EAST);
				return 1;
			}
		}
	}
	return 0;
}

int can_the_boss_blow(struct map* map, struct player* player,struct boss* boss){
	if (boss->timer_before_blow <=SDL_GetTicks()){ //we only want the boss to be able to blow occasionnally		
		int player_x=player_get_x(player);
		int player_y=player_get_y(player);
		if (there_is_a_player_on_a_direct_line(map,player_x,player_y,boss)){
			boss_set_wait_timer(boss,1000+SDL_GetTicks()); //we want the boss to stay in this positon, and wait 2s before the blow
			return 0;
		}
	}
	return 1; //the timer for the blow is still in cooldown
}



void boss_blow(struct map* map, struct player* player,struct boss* boss){
	enum direction direction=boss_get_direction(boss);
	int end =boss_get_max_size_of_map(map);
	int stop= 0;
	int boss_x=boss_get_x(boss);
	int boss_y=boss_get_y(boss);
	if (direction%2==0){ //North or South
		int coeff=direction-1; // -1 if North, 1 if South
		for (int i = 1; i<= end; i++){
			if (map_is_inside(map,boss_x,boss_y+coeff*i) && (stop==0)){
				stop+= bomb_explosion_aux(player, map,boss_x,boss_y+coeff*i,boss->damage);
			}
		}
	}
	else{
		int coeff=2-direction; // -1 if West, 1 if East
		for (int i = 1; i<= end; i++){
			if (map_is_inside(map,boss_x+coeff*i,boss_y) && (stop==0)){
				stop+=bomb_explosion_aux(player, map,boss_x+coeff*i,boss_y,boss->damage);
			}
		}
	}

}

void boss_update(struct map* map, struct player* player){ //update all the bombs
	struct boss* boss = map_get_boss(map);
    assert(boss);
	if (boss->lives >0){
		int delay=boss->wait_timer -SDL_GetTicks();
		// printf ("wait timer %i\n",boss->wait_timer );
		// printf ("timer before next blow %i\n",boss->timer_before_blow-SDL_GetTicks());
		// printf ("wait timer-ticks %i\n",delay);
		// printf ("\n");
		if ((delay<=0)&&(delay>=-1000)) { //if it's time for the blow explosion (we know the game is actualisated more often than every second so 1000 ticks is fine)
			// printf ("BLOW \n");
			boss_blow(map,player,boss);
			boss_set_wait_timer(boss,-1000); //now the boss can move again
			boss_set_timer_before_blow(map_get_boss(map),SDL_GetTicks()+10000); //10secondes delay before next blow
		}
		if ((boss->wait_timer ==-1000)) { //if the blow has happenned
			if(can_the_boss_blow(map,player,boss)){ //if the boss can't blow (else we don't want him to move)
				struct dijkstra* dijkstra= dijkstra_choose_path(map,boss_get_x(boss),boss_get_y(boss),player);
				if (dijkstra_get_Choose_of_path(dijkstra)){                        //if the monster can reach the player we use dijsktra to choose a path
					int x= dijkstra_get_x(dijkstra);
					int y= dijkstra_get_y(dijkstra);
					boss_dijkstra_choose_path(map,boss,player,x,y);
				}
				else {
					boss_choose_random_path(map, boss,player);  //else we pick a random path
				}	
			}
		}
	}
}

void boss_hit(struct map* map,int damage){
	struct boss* boss = map_get_boss(map);
	assert(boss);
	if ((boss->lives)<=damage){
		boss->lives=0; //kill the boss
	}
	else {
		boss->lives--;
	}
}

