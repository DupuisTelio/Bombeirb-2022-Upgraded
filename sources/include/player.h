/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef PLAYER_H_
#define PLAYER_H_

#include <map.h>
#include <SDL/SDL.h>
#include <constant.h>

struct player;

// Creates a new player with a given number of available bombs and lives
struct player* player_init(int bomb_number, int lives, int range,int damage);
void   player_free(struct player* player);

// Set the position, win condition of the player
void player_set_position(struct player *player, int x, int y);
void player_set_win_condition(struct player* player,int win_condition) ;


// Returns the current position, the lives, win condition, nb of key, range, nb of bombs, damage of the player 
int player_get_x(struct player* player);
int player_get_y(struct player* player);
int  player_get_player_lives(struct player* player);
int  player_get_player_range(struct player* player);
int player_get_win_condition(struct player* player) ;
int player_get_nb_key(struct player* player);
int player_get_range(struct player* player);
int  player_get_nb_bomb(struct player * player);
int player_get_damage(struct player* player);

// Set the nb of keys of the player
void player_set_keys(struct player* player,int key);

// Set the direction of the next move of the player
void player_set_current_way(struct player * player, enum direction direction);

// Decrease the number of lives of the player
void player_dec_nb_lives(struct player* player, int damage);


// Increase, Decrease the number of bomb that player can put

void player_inc_nb_bomb(struct player * player);
void player_dec_nb_bomb(struct player * player);


// Move the player according to the current direction
int player_move(struct player* player, struct map* map);

// Display the player on the screen
void player_display(struct player* player);

//Update the invicibility
void player_update_invicibility(struct player* player);

#endif /* PLAYER_H_ */
