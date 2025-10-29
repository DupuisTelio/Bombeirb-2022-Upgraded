/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>
#include<SDL/SDL.h>

#include <player.h>
#include <game.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <unistd.h>
#include <time.h>
#include <boss.h>
#include <door.h>

struct player
{
	int x, y;
	enum direction direction;
	int bombs;
	int lives;
	int range;
	int key;
	int invicibility;
	int win_condition;
	int damage;
};

struct player* player_init(int bombs, int lives, int range,int damage)
{
	struct player* player = malloc(sizeof(*player));
	if (!player)
		error("Memory error");

	player->direction = NORTH;
	player->bombs = bombs;
	player->lives = lives;
	player->range = range ;
	player->damage=damage;
	player-> key = 0;
	player->invicibility =0;
	player->win_condition=0;
	return player;
}


int player_get_win_condition(struct player* player) {
	assert(player != NULL);
	return player->win_condition;
}

void player_set_win_condition(struct player* player,int win_condition) {
	assert(player);
	player->win_condition =win_condition;
}

int player_get_invicibility(struct player* player) {
	assert(player != NULL);
	return player->invicibility;
}

void player_set_invicibility(struct player *player, int Time_of_invicibility) {
	assert(player);
	player->invicibility = Time_of_invicibility;
}


void player_set_position(struct player *player, int x, int y)
{
	assert(player);
	player->x = x;
	player->y = y;
}

void player_set_keys(struct player* player,int key){
	assert(player);
	player->key=key;
}

void player_free(struct player* player) {
	assert(player);
	free(player);
}

int player_get_x(struct player* player) {
	assert(player != NULL);
	return player->x;
}

int player_get_y(struct player* player) {
	assert(player != NULL);
	return player->y;
}

int player_get_damage(struct player* player) {
	assert(player != NULL);
	return player->damage;
}

int  player_get_player_lives(struct player* player) {
	assert(player != NULL);
	return player->lives;
}

void player_start_invicibility(struct player* player){
	assert(player);
	player_set_invicibility(player,SDL_GetTicks()+1000);
}

void player_update_invicibility(struct player* player){
	assert(player);
	int Time_of_invicibility=player_get_invicibility(player)-SDL_GetTicks();
	if (Time_of_invicibility<=0){
		player_set_invicibility(player,0);
	}
}

void player_dec_nb_lives(struct player* player,int damage) {
	assert(player);
	if (player-> invicibility <=0){
		if (player->lives <=damage){
			player->win_condition=-1;
			player->lives = 0;
		}
		else {
			player->lives -= damage;
			player_start_invicibility(player);
		}
	}
}

int  player_get_player_range(struct player* player) {
	assert(player != NULL);
	return player->range;
}

void player_set_current_way(struct player* player, enum direction way) {
	assert(player);
	player->direction = way;
}

int player_get_nb_bomb(struct player* player) {
	assert(player);
	return player->bombs;
}

int player_get_range(struct player* player) {
	assert(player);
	return player->range;
}

void player_inc_nb_bomb(struct player* player) {
	assert(player);
	if((player->bombs)<9){ //else we go further than the maximum number of bombs than can be displayed in the banner
		player->bombs += 1;
	}
}

void player_dec_nb_bomb(struct player* player) {
	assert(player);
	player->bombs -= 1;
}

int player_get_nb_key(struct player* player) {
	assert(player);
	return player->key;
}

static int move_box(struct player* player, struct map* map, int x, int y){
	int a = player->x;
	int b = player->y;
	a=2*x-a; //corresponds to the absissa of the cell just after the box according to the direction of movement of the player, the direction of push
	b=2*y-b; // the same for the ordinate
	if (!map_is_inside(map, a, b)){                       //if the cell is not in the map, nothing happens
		return 0;
		}
	if (map_get_cell_type(map, a, b) == CELL_EMPTY){      //if the cell is empty, the box is moved to the cell and the old box is freed to move the player there
		map_set_cell_type(map,a,b,CELL_BOX);
		map_set_cell_type(map,x,y,CELL_EMPTY);
		return 1;
		}
	return 0;
}

static int player_encounter_bonus(struct player* player, struct map* map, int x, int y)
{
	switch (map_get_bonus_subtype(map,x,y)) { // sub-types are encoded with the 4 less significant bits

	case BONUS_BOMB_RANGE_INC:
		if (player->range <9){
			player->range++;
		}
		break;

	case BONUS_BOMB_RANGE_DEC:
		if (player->range >1){  //always a range >=1
			player->range--;
		}
		break;

	case BONUS_BOMB_NB_INC:  //0x0f & CELL_BONUS_BOMBINC
		if(player->bombs <9){
			player->bombs++;
		}
		break;

	case BONUS_BOMB_NB_DEC :
		if(player->bombs >1){    //always a number of bombs >=1
			player->bombs--;
		}
		break;

	case BONUS_LIFE :
		if(player->lives <9){
			player->lives++;
		}
		break;
	
	default :
		break;
	}
	map_set_cell_type(map,x,y,CELL_EMPTY);
	return 1;
}


static int player_move_aux(struct player* player, struct map* map, int x, int y) {
	
	if (!map_is_inside(map, x, y))
		return 0;

	switch (map_get_cell_type(map, x, y)) {

		case CELL_SCENERY:
			if (map_get_scenery_subtype(map,x,y)==SCENERY_PRINCESS){
				player->win_condition++;
				return 1;
			}
			return 0;
			break;

		case CELL_BOX:
			return move_box(player, map,x,y);
			break;

		case CELL_BONUS:
			return player_encounter_bonus(player, map, x, y);
			break;

		case CELL_MONSTER:
			player_dec_nb_lives(player,1);
			return 0;
			break;
		
		case CELL_BOSS:
			player_dec_nb_lives(player,boss_get_damage(map_get_boss(map)));
			return 0;
			break;

		case CELL_BOMB:
			if (map_get_bomb_subtype(map,x,y) == BOMB_EXPLOSION){ // (CELL_BOMB & BOMB_EXPLOSION)
				return 1;
			}
			return 0;
			break;
		case CELL_KEY:
			player->key++;
			map_set_cell_type(map,x,y,CELL_EMPTY);
			break;
		case CELL_DOOR:
			return 1;
			break;
		default:
			break;
	}

	// Player has moved
	return 1;
}

int player_move(struct player* player, struct map* map) {
	int x = player->x;
	int y = player->y;
	int move = 0;

	switch (player->direction) {
	case NORTH:
		if (player_move_aux(player, map, x, y - 1)) {
			player->y--;
			move = 1;
		}
		break;

	case SOUTH:
		if (player_move_aux(player, map, x, y + 1)) {
			player->y++;
			move = 1;
		}
		break; 

	case WEST:
		if (player_move_aux(player, map, x - 1, y)) {
			player->x--;
			move = 1;
		}
		break;

	case EAST:
		if (player_move_aux(player, map, x + 1, y)) {
			player->x++;
			move = 1;
		}
		break;
	}
	return move;
}



void player_display(struct player* player) {
	assert(player);
	window_display_image(sprite_get_player(player->direction),
			player->x * SIZE_BLOC, player->y * SIZE_BLOC);
}
