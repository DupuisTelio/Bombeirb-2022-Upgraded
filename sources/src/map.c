/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <map.h>
#include <constant.h>
#include <misc.h>
#include <sprite.h>
#include <window.h>
#include <bomb.h>
#include <player.h>
#include <monster.h>
#include <boss.h>
#include <chargement.h>

struct map {
	int width;
	int height;
	unsigned char* grid;
	struct bomb* tab_bombs;
	struct explosion* tab_explosion;
	struct monster* tab_monster;
	struct boss* boss;

	int speed_monster;
	int timer_monster;
	int lives_monster;
};

#define CELL(i,j) ( (i) + (j) * map->width)

struct map* map_new(int width, int height, int difficulty)
{
	assert(width > 0 && height > 0);

	struct map* map = malloc(sizeof *map);
	if (map == NULL )
		error("map_new : malloc map failed");

	map->width = width;
	map->height = height;

	map->grid = malloc(height * width);
	if (map->grid == NULL) {
		error("map_new : malloc grid failed");
	}

	// Grid cleaning
	int i, j;
	for (i = 0; i < width; i++)
	  for (j = 0; j < height; j++)
	    map->grid[CELL(i,j)] = CELL_EMPTY;

	map->tab_bombs = malloc(9*bomb_get_struct_bomb_size()); //array of 9 bombs because the player can't have more than 9 bombs
	bomb_init_tab_bombs(map->tab_bombs);

	map->tab_explosion = malloc(height*width*bomb_get_struct_explosion_size()); //a maximum of height*widht explosion
	bomb_init_tab_explosion(map->tab_explosion,height*width);

	map->tab_monster = malloc(15*monster_get_struct_monster_size()); // we don't want more than 15 monster so the game can be enjoyable
	monster_init_tab(map->tab_monster);

	map->boss = malloc(boss_get_struct_boss_size()); 
	boss_set_lives(map->boss);

	map->speed_monster=1000-75*difficulty; //the higher the difficulty is, the faster the monsters are
	map->timer_monster=SDL_GetTicks();
	map->lives_monster=difficulty;

	return map;
}

int map_is_inside(struct map* map, int x, int y)
{
	return ((map->width > x )&&(map->height > y )&&(x>=0)&&(y>=0) );
}


void map_free(struct map *map)
{
	if (map == NULL )
		return;
	free(map->grid);
	free(map);
}


int map_get_width(struct map* map)
{
	assert(map != NULL);
	return map->width;
}


int map_get_height(struct map* map)
{
	assert(map);
	return map->height;
}

int map_get_speed_monster(struct map* map)
{
	assert(map);
	return map->speed_monster;
}

void map_set_speed_monster(struct map* map,int speed_monster)
{
	assert(map);
	map->speed_monster=speed_monster;
}

int map_get_timer_monster(struct map* map)
{
	assert(map);
	return map->timer_monster;
}

void map_set_timer_monster(struct map* map,int timer_monster)
{
	assert(map);
	map->timer_monster=timer_monster;
}

int map_get_lives_monster(struct map* map)
{
	assert(map);
	return map->lives_monster;
}

struct bomb* map_get_tab_bombs(struct map* map)
{
	assert(map);
	return map->tab_bombs;
}

struct explosion* map_get_tab_explosion(struct map* map)
{
	assert(map);
	return map->tab_explosion;
}

struct monster* map_get_tab_monster(struct map* map)
{
	assert(map);
	return map->tab_monster;
}

struct boss* map_get_boss(struct map* map)
{
	assert(map);
	return map->boss;
}

enum cell_type map_get_cell_type(struct map* map, int x, int y)
{
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x,y)] & 0xf0;
}

//We could have used a enum compose_type map_get_subtype (...)

enum scenery_type map_get_scenery_subtype(struct map* map, int x, int y)
{
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x,y)] & 0x0f;
}

enum bonus_type map_get_bonus_subtype(struct map* map, int x, int y) //get the subtype of the CELL (ex: for a bomb -> bomb_4)
{
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x,y)] & 0x0f;
}

enum bomb_type map_get_bomb_subtype(struct map* map, int x, int y) //get the subtype of the CELL (ex: for a bomb -> bomb_4)
{
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x,y)] & 0x0f;
}

enum door_type map_get_door_subtype(struct map* map, int x, int y) //get the subtype of the CELL (ex: for a bomb -> bomb_4)
{
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x,y)] & 0x01;
}

enum cell_type map_get_cell_subtype(struct map* map, int x, int y)
{
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x,y)] & 0x0f;
}

void map_set_cell_type(struct map* map, int x, int y, enum cell_type type)
{
	assert(map && map_is_inside(map, x, y));
	map->grid[CELL(x,y)] = type;
}


void display_bonus(struct map* map, int x, int y, unsigned char type)
{
	// bonus is encoded with the 4 most significant bits
	switch (type & 0x0f) {
	case BONUS_BOMB_RANGE_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_INC), x, y);
		break;

	case BONUS_BOMB_RANGE_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_DEC), x, y);
		break;

	case BONUS_BOMB_NB_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_DEC), x, y);
		break;

	case BONUS_BOMB_NB_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_INC), x, y);
		break;
	case BONUS_LIFE:
		window_display_image(sprite_get_banner_life(),x, y);
		break;
	}
}

void display_bomb(struct map* map, int x, int y , unsigned char type)
{
	switch (type & 0x0f) { // sub-types are encoded with the 4 less significant bits
	case BOMB_BOMB_1:
		window_display_image(sprite_get_bomb_1(), x, y);
		break;
	case BOMB_BOMB_2:
		window_display_image(sprite_get_bomb_2(), x, y);
		break;
	case BOMB_BOMB_3:
		window_display_image(sprite_get_bomb_3(), x, y);
		break;
	case BOMB_BOMB_4:
		window_display_image(sprite_get_bomb_4(), x, y);
		break;
	case BOMB_EXPLOSION:
		window_display_image(sprite_get_explosion(),x,y);
		break;
	}
}


void display_scenery(struct map* map, int x, int  y, unsigned char type)
{
	switch (type & 0x0f) { // sub-types are encoded with the 4 less significant bits
	case SCENERY_STONE:
		window_display_image(sprite_get_stone(), x, y);
		break;

	case SCENERY_TREE:
		window_display_image(sprite_get_tree(), x, y);
		break;

	case SCENERY_PRINCESS:
		window_display_image(sprite_get_princess(),x,y);
		break;
	}
}

void display_door(struct map* map, int x, int  y, unsigned char type)
{
	switch (type & 0x01) { // sub-types are encoded with the 4 less significant bits
	case DOOR_CLOSED:
		window_display_image(sprite_get_door_closed(), x, y);
		break;

	case DOOR_OPENED:
		window_display_image(sprite_get_door_opened(), x, y);
		break;
	}
}



void map_display(struct map* map)
{
	assert(map != NULL);
	assert(map->height > 0 && map->width > 0);

	int x, y;
	for (int i = 0; i < map->width; i++) {
	  for (int j = 0; j < map->height; j++) {
	    x = i * SIZE_BLOC;
	    y = j * SIZE_BLOC;

	    unsigned char type = map->grid[CELL(i,j)];

	    switch (type & 0xf0) {
		case CELL_BOMB:
			display_bomb(map, x, y ,type);
			break;
		case CELL_SCENERY:
		  display_scenery(map, x, y, type);
		  break;
	    case CELL_BOX:
	      window_display_image(sprite_get_box(), x, y);
	      break;
	    case CELL_BONUS:
	      display_bonus(map, x, y, type);
	      break;
	    case CELL_KEY:
	      window_display_image(sprite_get_key(), x, y);
	      break;
	    case CELL_DOOR:
	      display_door(map,x,y,type);
	      break;
		case CELL_MONSTER:
			window_display_image(sprite_get_monster(monster_get_direction(monster_get_correct_one(map,i,j))),x,y);
			break;
		case CELL_BOSS:
			struct boss* boss=map_get_boss(map);
			int wait_timer= boss_get_wait_timer(boss);
			if (wait_timer == -1000){ //if the boss is moving
				window_display_image(sprite_get_boss(boss_get_direction(boss)),x,y);
			}
			else { //if the boss is going to blow 
				window_display_image(sprite_get_blowing_boss(boss_get_direction(boss)),x,y);
			}
			break;
	    }
	  }
	}
}

struct map* map_init(short level){
	//char * path = "./map/map_2";
	char path[30];
	sprintf(path,"./map/map_%i",level);
	int width = load_map_width(path);
	int height = load_map_heigth(path);
	struct map* map = map_new(width,height,level);
	int * themap = load_tab_map (path);
	for (int i = 0; i < STATIC_MAP_WIDTH * STATIC_MAP_HEIGHT; i++)
		map->grid[i] = themap[i];

	monsters_get_monsters_of_map(map); //we add every monster on the cells to the tab of monsters of the map
	boss_get_boss_of_map(map);
	return map;
}

struct map* map_get_static(void) //not used anymore
{
	struct map* map = map_new(STATIC_MAP_WIDTH, STATIC_MAP_HEIGHT,8);
	unsigned char themap[STATIC_MAP_WIDTH * STATIC_MAP_HEIGHT] = {
	   CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
	   CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	   CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	   CELL_BOX, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	   CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_PRINCESS,
	   CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	   CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY , CELL_EMPTY, CELL_EMPTY ,CELL_EMPTY, CELL_EMPTY, CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	   CELL_EMPTY, CELL_TREE, CELL_BOX, CELL_TREE, CELL_EMPTY, CELL_EMPTY, CELL_BOSS, CELL_EMPTY,  CELL_EMPTY, CELL_EMPTY,  CELL_EMPTY, CELL_EMPTY,
	   CELL_EMPTY, CELL_TREE, CELL_TREE, CELL_TREE, CELL_STONE, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,  CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	   CELL_BOX_RANGEINC, CELL_BOX_RANGEDEC, CELL_BOX_LIFE, CELL_BOX_BOMBDEC, CELL_BOX_MONSTER, CELL_BOX_MONSTER, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	   CELL_BOX, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE,  CELL_BOX_LIFE, CELL_EMPTY,
	   CELL_BOX,CELL_EMPTY, CELL_DOOR_CLOSED, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_KEY,
	};
	for (int i = 0; i < STATIC_MAP_WIDTH * STATIC_MAP_HEIGHT; i++)
		map->grid[i] = themap[i];

	monsters_get_monsters_of_map(map); //we add every monster on the cells to the tab of monsters of the map
	boss_get_boss_of_map(map);
	return map;
}
