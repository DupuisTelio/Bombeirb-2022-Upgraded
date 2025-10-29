#include <SDL/SDL_image.h>
#include <assert.h>

#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <map.h>
#include <stdlib.h>
#include <monster.h>
#include <boss.h>
#include <game.h>

struct bomb {
	int x, y; //position
	int timer;
	int range;
	int damage;
};

struct explosion {
	int x, y; //position
	int timer;
};


/* Solution linked list

struct Element
{
    struct bomb bomb;
    struct Element *next;
};
typedef struct Element Element_t;


Element_t *Start_the_chain_bomb()
{
    Element_t *element = malloc(sizeof(*element));
    assert(element);
    element->bomb = {0};
    element->next = NULL;
    return element;
}

void insert_bomb_in_chain(Element_t *chain_of_bomb, struct bomb bomb)
{
    Element_t *new_bomb = malloc(sizeof(*new_bomb));
	assert(new_bomb);
    new_bomb->bomb = bomb;
	new_bomb->next = chain_of_bomb;
}

*/











//EXPLOSION PART

int bomb_get_struct_explosion_size(){
	return sizeof(struct explosion);
}

void explosion_set_timer(struct explosion* explosion,int x) {
	assert(explosion);
	explosion->timer=x;
};


void explosion_set_position(struct explosion* explosion,int x,int y) {
	assert(explosion);
	explosion->x=x;
	explosion->y=y;
};

struct explosion* explosion_init(int x, int y) {
	struct explosion* explosion = malloc(sizeof(explosion));
	assert(explosion);
	explosion->timer = 500+SDL_GetTicks(); // we want the explosion to last 0.5s
	explosion-> x = x;
	explosion->y = y;
	return explosion;
}

void bomb_init_tab_explosion(struct explosion* tab_explosion,int size){
	for (int i = 0; i < 50/*size*/; i++){
		explosion_set_timer(&tab_explosion[i],0);
	}
}

void insert_explosion_in_tab(struct map* map,struct explosion* explosion){
	struct explosion* tab_explosion=map_get_tab_explosion(map);
	int i = 0;
	while ((&tab_explosion[i])->timer >0){
		i++;
	}
	explosion_set_timer(&tab_explosion[i],explosion->timer);
	explosion_set_position(&tab_explosion[i],explosion->x,explosion->y);
}

void explosion_of_a_box(struct map* map, int x, int y){
	switch (map_get_bonus_subtype(map,x,y) ) { // sub-types are encoded with the 4 less significant bits
	case BONUS_BOMB_RANGE_INC:
		map_set_cell_type(map,x,y,CELL_BONUS_RANGEINC);
		break;

	case BONUS_BOMB_RANGE_DEC:
		map_set_cell_type(map,x,y,CELL_BONUS_RANGEDEC);
		break;

	case BONUS_BOMB_NB_INC:  //0x0f & CELL_BONUS_BOMBINC
		map_set_cell_type(map,x,y,CELL_BONUS_BOMBINC);
		break;

	case BONUS_BOMB_NB_DEC :
		map_set_cell_type(map,x,y,CELL_BONUS_BOMBDEC);
		break;

	case BONUS_LIFE :
		map_set_cell_type(map,x,y,CELL_BONUS_LIFE);
		break;

	case BONUS_MONSTER: 
		monster_spawn( x, y, map);
		break;
		//monster_add();
	default :
		insert_explosion_in_tab(map,explosion_init(x,y));
		map_set_cell_type(map,x,y,CELL_EXPLOSION);
		break;
	}
}

int bomb_explosion_aux(struct player* player, struct map* map,int x, int y,int damage){
	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY: //we stop the propagation of the explosion in this direction
		if (map_get_scenery_subtype(map,x,y)==SCENERY_PRINCESS){
			player_set_win_condition(player,-1);
			map_set_cell_type(map,x,y,CELL_EXPLOSION);
		}
		return 1;
		break;
	case CELL_BOX: //make a bonus or a monster spawn
		explosion_of_a_box(map,x,y);
		return 1;
		break;
	case CELL_BONUS:
		insert_explosion_in_tab(map,explosion_init(x,y));
		map_set_cell_type(map,x,y,CELL_EXPLOSION); //the bonus disappears
		return 1;
		break;
	case CELL_MONSTER:
		insert_explosion_in_tab(map,explosion_init(x,y));
		map_set_cell_type(map,x,y,CELL_EXPLOSION);
		monster_hit(map,x,y,damage);
		return 0; //let the propagation
		break;
	case CELL_BOSS:
		insert_explosion_in_tab(map,explosion_init(x,y));
		map_set_cell_type(map,x,y,CELL_EXPLOSION);
		boss_hit(map,damage);
		return 0; //let the propagation
		break;
	case CELL_BOMB:  //stop the propagation and make the bomb that is on the targeted cell explose
		/*
		while ((!(tab_bombs[i]->x==x))&&(!(tab_bombs[i]->y==y))){  //we are looking for the bomb that is on the targeted Cell
			i++;
		}
		*/
		//bomb_explosion(tab_bombs[i],player,map);
		return 1;
		break;
	case CELL_EMPTY:
		if ((player_get_x(player)==x)&&(player_get_y(player)==y)){
			player_dec_nb_lives(player,damage);
		}
		insert_explosion_in_tab(map,explosion_init(x,y));
		map_set_cell_type(map,x,y,CELL_EXPLOSION);
		return 0;
		break;
	default :  //case of doors and keys, so we stop the propagation of the explosion
		return 1;
		break;
	}
}

void bomb_explosion(struct bomb* bomb, struct map* map, struct player* player){
	bomb->timer =0; //it means the bomb no longer exists
	int x = bomb->x;
	int y = bomb->y;
	int N = 0; //North
	int S = 0; //South
	int W = 0; //West
	int E = 0; //East
	map_set_cell_type(map,x,y,CELL_EXPLOSION); //we put a explosion_cell on the grid
	player_inc_nb_bomb(player);                //we give back his bomb to the player
	insert_explosion_in_tab(map,explosion_init(bomb->x,bomb-> y));  //we stock the explosion in a tab to deal with the evolution througout the time
	if ((player_get_x(player)==x)&&(player_get_y(player)==y)){  //if the player is touched by the explosion
		player_dec_nb_lives(player,bomb->damage);
	}
	for (int i = 1; i<= bomb->range ; i++){ //we made the first explosion, now it's time for the cross

		if (map_is_inside(map,x,y+i) && (N==0)){
			N+=bomb_explosion_aux(player, map,x,y+i,3);      //as soon as you encounter a door, a key, a bonus, a box, a tree, a rock, ... stop the explosion in that direction
		}

		if (map_is_inside(map,x,y-i) && (S==0)){
			S+=bomb_explosion_aux( player, map,x,y-i,3);
		}

		if (map_is_inside(map,x-i,y) && (W==0)){
			W+=bomb_explosion_aux(player,map,x-i,y,3);
		}

		if (map_is_inside(map,x+i,y) && (E==0)){
			E+=bomb_explosion_aux(player,map,x+i,y,3);
		}
	}
}

void explosion_update_aux (struct explosion* explosion, struct map* map){ //update a explosion cell
	int timer = (explosion->timer)-SDL_GetTicks();
	if (timer<=0){
		map_set_cell_type(map,explosion->x,explosion->y,CELL_EMPTY);
		explosion->timer=0; //the explosion no longer exists
	}

}

void bomb_explosion_update (struct map* map){ //update all the explosion CELLS
	struct explosion* tab_explosion= map_get_tab_explosion(map);
	for (int i = 0; i < 30 /*map_get_width(map)*map_get_height(map)*/ ; i++){
		if ((&tab_explosion[i])->timer >0){
			explosion_update_aux(&tab_explosion[i],map);
		}
	}
}

















//BOMB PART

int bomb_get_struct_bomb_size(){
	return sizeof(struct bomb);
}

void bomb_set_timer(struct bomb* bomb,int x) {
	assert(bomb);
	bomb->timer=x;
};

void bomb_set_range(struct bomb* bomb,int x) {
	assert(bomb);
	bomb->range=x;
};

void bomb_set_position(struct bomb* bomb,int x,int y) {
	assert(bomb);
	bomb->x=x;
	bomb->y=y;
};


void bomb_init_tab_bombs(struct bomb* tab_bombs){
	for (int i = 0; i < 9; i++){
		bomb_set_timer(&tab_bombs[i],0);
	}
}

struct bomb* bomb_init(int x, int y, int range,int damage) {
	struct bomb* bomb = malloc(sizeof(bomb));
	assert(bomb);
	bomb->timer = 4000+SDL_GetTicks(); //we start the timer of the bomb at the current date + 4s
	bomb->range = range ;
	bomb-> x = x;
	bomb->y = y;
	bomb->damage=damage;
	return bomb;
}

void insert_bomb_in_tab(struct map* map,struct bomb* bomb){
	struct bomb* tab_bombs=map_get_tab_bombs(map);
	int i = 0;
	while ((&tab_bombs[i])->timer >0){
		i++;
	}
	bomb_set_timer(&tab_bombs[i],bomb->timer);
	bomb_set_range(&tab_bombs[i],bomb->range);
	bomb_set_position(&tab_bombs[i],bomb->x,bomb->y);
}

void bomb_start(struct player* player,struct map* map){
	if (player_get_nb_bomb(player)>=1){
		int x = player_get_x(player);
		int y = player_get_y(player);
		if( map_get_cell_type(map,x,y)== CELL_EMPTY){ //to avoid the case where there is already a bomb on the targeted cell
			player_dec_nb_bomb(player);
			map_set_cell_type(map,x,y,CELL_BOMB_4);
			insert_bomb_in_tab(map,bomb_init(x,y,player_get_range(player),player_get_damage(player)));
		}
	}
}


void bomb_update_aux (struct bomb* bomb, struct map* map,struct player* player){ //update one bomb
	int timer= bomb->timer;
	timer-=SDL_GetTicks();
	int x = bomb->x;
	int y = bomb->y;
	if ((timer<=3000)&(timer>2000)){
		map_set_cell_type(map,x,y,CELL_BOMB_3);
	}
	if ((timer<=2000)&(timer>1000)){
		map_set_cell_type(map,x,y,CELL_BOMB_2);
	}
	if ((timer<=1000)&(timer>0)){
		map_set_cell_type(map,x,y,CELL_BOMB_1);
	}
	if (timer<=0){
		bomb_explosion(bomb,map,player);
	}
}


void bomb_update(struct map* map, struct player* player){ //update all the bombs
	struct bomb* tab_bombs= map_get_tab_bombs(map);
	for (int i = 0; i < 9; i++){
		if ((&tab_bombs[i])->timer >0){
			bomb_update_aux(&tab_bombs[i],map,player);
		}
	}
}

void bomb_timer_break_update (int break_duration, struct map* map){
	struct bomb* tab_bombs = map_get_tab_bombs(map);
	for (int i = 0; i < 9; i++){
		if ((&tab_bombs[i])->timer >0){         //timer =0 means it's not anymore a bomb or it hasn't be initialized
		(&tab_bombs[i])->timer+= break_duration; //we increase the timer of each bomb of the duration of the break, so when we will compare the timers to SDLGetTicks() (which will have increased of break_duration) we won't notice a change
		}
	}
}
