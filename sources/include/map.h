/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef MAP_H_
#define MAP_H_


enum cell_type {
	CELL_EMPTY=0x00,   		//  0000 0000     0
	CELL_SCENERY=0x10, 		//  0001 0000     16
	CELL_BOX=0x20,   		//  0010 0000     32
	CELL_DOOR=0x30,      	//  0011 0000	  48			
	CELL_KEY=0x40,       	//  0100 0000	  64
	CELL_BONUS=0x50, 		// 	0101 0000	  80	
	CELL_MONSTER=0x60, 		// 	0110 0000     96
	CELL_BOMB=0x70, 	   	// 	0111 0000	  112	
	CELL_BOSS=0x80, 		// 	1000 0000	  128		
};

enum door_type {
	DOOR_CLOSED = 0 , //0000
	DOOR_OPENED = 1 , //0001
};

enum bomb_type {
	BOMB_BOMB_1 = 1, //0001
	BOMB_BOMB_2 = 2, //0010
	BOMB_BOMB_3 = 4, //0100
	BOMB_BOMB_4 = 8, //1000
	BOMB_EXPLOSION = 9, //1001
} ;

enum bonus_type { // 80
	BONUS_BOMB_RANGE_DEC=1, //0001	81
	BONUS_BOMB_RANGE_INC=2, //0010	82
	BONUS_BOMB_NB_DEC=3,    //00	83
	BONUS_BOMB_NB_INC=4,    //0100	84
	BONUS_LIFE=5, 			//0101	85
	BONUS_MONSTER=6,		//0110	86
};

enum scenery_type {
	SCENERY_STONE = 1,    // 0001
	SCENERY_TREE  = 2,    // 0010
	SCENERY_PRINCESS = 3  // 0011
};

enum compose_type {
	CELL_TREE     = CELL_SCENERY | SCENERY_TREE,
	CELL_STONE    = CELL_SCENERY | SCENERY_STONE,
	CELL_PRINCESS = CELL_SCENERY | SCENERY_PRINCESS,

	CELL_DOOR_CLOSED = CELL_DOOR | DOOR_CLOSED,
	CELL_DOOR_OPENED = CELL_DOOR | DOOR_OPENED,


	CELL_BOMB_1   = CELL_BOMB | BOMB_BOMB_1,
	CELL_BOMB_2   = CELL_BOMB | BOMB_BOMB_2,
	CELL_BOMB_3   = CELL_BOMB | BOMB_BOMB_3,
	CELL_BOMB_4   = CELL_BOMB | BOMB_BOMB_4,
	CELL_EXPLOSION = CELL_BOMB | BOMB_EXPLOSION ,

  	CELL_BOX_RANGEINC = CELL_BOX | BONUS_BOMB_RANGE_INC,
  	CELL_BOX_RANGEDEC = CELL_BOX | BONUS_BOMB_RANGE_DEC,
	CELL_BOX_BOMBINC  = CELL_BOX | BONUS_BOMB_NB_INC,
  	CELL_BOX_BOMBDEC  = CELL_BOX | BONUS_BOMB_NB_DEC,
  	CELL_BOX_LIFE     = CELL_BOX | BONUS_LIFE,
  	CELL_BOX_MONSTER  = CELL_BOX | BONUS_MONSTER,

	CELL_BONUS_RANGEINC = CELL_BONUS | BONUS_BOMB_RANGE_INC,
 	CELL_BONUS_RANGEDEC = CELL_BONUS | BONUS_BOMB_RANGE_DEC,
	CELL_BONUS_BOMBINC  = CELL_BONUS | BONUS_BOMB_NB_INC,
  	CELL_BONUS_BOMBDEC  = CELL_BONUS | BONUS_BOMB_NB_DEC,
 	CELL_BONUS_LIFE     = CELL_BONUS | BONUS_LIFE,
};

struct map;

// Create a new empty map
struct map* map_new(int width, int height,int difficulty); //difficulty goes from 1 to 8
void map_free(struct map* map);


// Return the height and width of a map
int map_get_width(struct map* map);
int map_get_height(struct map* map);

// Return or set the speed, the timer and lives of the monsters
int map_get_speed_monster(struct map* map);
void map_set_speed_monster(struct map* map,int speed_monster);
int map_get_timer_monster(struct map* map);
void map_set_timer_monster(struct map* map,int timer_monster);
int map_get_lives_monster(struct map* map);

// Return the tab_bombs, the tab_explosion, the tab_monster and the boss of a map
struct bomb* map_get_tab_bombs(struct map* map);
struct explosion* map_get_tab_explosion(struct map* map);
struct monster* map_get_tab_monster(struct map* map);
struct boss* map_get_boss(struct map* map);

// Return the type or the subtype of a cell
enum cell_type map_get_cell_type(struct map* map, int x, int y);
enum bomb_type map_get_bomb_subtype(struct map* map, int x, int y);
enum bonus_type map_get_bonus_subtype(struct map* map, int x, int y);
enum door_type map_get_door_subtype(struct map* map, int x, int y) ; 
enum scenery_type map_get_scenery_subtype(struct map* map, int x, int y);
enum cell_type map_get_cell_subtype(struct map* map, int x, int y);

// Set the type of a cell
void  map_set_cell_type(struct map* map, int x, int y, enum cell_type type);

// Test if (x,y) is within the map
int map_is_inside(struct map* map, int x, int y);

// Return a default static map
struct map* map_get_static();

// Display the map on the screen
void map_display(struct map* map);

//temporary
struct map* map_init(short level);

#endif /* MAP_H_ */
