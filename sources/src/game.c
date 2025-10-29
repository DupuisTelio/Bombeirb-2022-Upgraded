/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <assert.h>
#include <time.h>

#include <game.h>
#include <misc.h>
#include <window.h>
#include <sprite.h>
#include <bomb.h>
#include <time.h>
#include <map.h>
#include <monster.h>
#include <boss.h>
#include <screen.h>
#include <screen.h>
#include <door.h>

#include <constant.h>
#include <unistd.h>
#include <time.h>


struct game {
	struct map** maps;       // the game's map
	short levels;        // nb maps of the game
	short level;
	struct player* player;
};


void game_change_of_level(struct game* game, short level ){
	assert(game);
	game->level=level;
	player_set_position(game->player, 1, 0);
}

struct game* game_new(void) {
	sprite_load(); // load sprites into process memory
	int levels = 8;
	struct game* game = malloc(sizeof(*game));
	game->maps = malloc(levels*sizeof(struct map*));
	game->levels = levels;
	for (int i=0; i<levels;i++){
		game->maps[i] = map_init(i+1);
	}
	game->level = 7;

	game->player = player_init(3,3, 2,5);
	// Set default location of the player
	player_set_position(game->player, 1, 0);

	return game;
}

void game_free(struct game* game) {
	assert(game);

	player_free(game->player);
	for (int i = 0; i < game->levels; i++)
		map_free(game->maps[i]);
}

struct map* game_get_current_map(struct game* game) {
	assert(game);
	return game->maps[game->level];
}


struct player* game_get_player(struct game* game) {
	assert(game);
	return game->player;
}

void game_banner_display(struct game* game) {
	assert(game);

	struct map* map = game_get_current_map(game);

	int y = (map_get_height(map)) * SIZE_BLOC;
	for (int i = 0; i < map_get_width(map); i++)
		window_display_image(sprite_get_banner_line(), i * SIZE_BLOC, y);

	int white_bloc = ((map_get_width(map) * SIZE_BLOC) - 9 * SIZE_BLOC) / 4;
	int x = white_bloc;
	y = (map_get_height(map) * SIZE_BLOC) + LINE_HEIGHT;
	window_display_image(sprite_get_banner_life(), x, y);

	x = white_bloc + SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_player_lives(game->player)), x, y); //affichage de la vie

	x = 2 * white_bloc + 2 * SIZE_BLOC;
	window_display_image(sprite_get_banner_bomb(), x, y);

	x = 2 * white_bloc + 3 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_nb_bomb(game_get_player(game))), x, y);

	x = 3 * white_bloc + 4 * SIZE_BLOC;
	window_display_image(sprite_get_banner_range(), x, y);

	x = 3 * white_bloc + 5 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_player_range(game->player)), x, y);

	x = 3 * white_bloc + 7 * SIZE_BLOC;
	window_display_image(sprite_get_key(), x, y);

	x = 3 * white_bloc + 8 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_nb_key(game->player)), x, y);
}

void game_display(struct game* game) {
	assert(game);

	window_clear();
	game_banner_display(game);
	map_display(game_get_current_map(game));
	player_display(game->player);

	window_refresh();
}

int game_break(struct map* map){
	int t_0 = SDL_GetTicks();
	int i = 1;
	int answer=0;
	screen_break_display();
	SDL_Event event;
	while (i){
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {	
						case SDLK_ESCAPE:
							i=0;
							answer =1;
							break;		
						case SDLK_p:
							i=0;
							break;
						default:
							break;
					}
				default: 
					break;
			}
			break ;
		}
	}
	int break_duration = SDL_GetTicks()-t_0;
	bomb_timer_break_update(break_duration, map) ;
	return answer;
}

static short input_keyboard(struct game* game) {
	SDL_Event event;
	struct player* player = game_get_player(game);
	struct map* map = game_get_current_map(game);
	
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return 1;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				return 1;
			case SDLK_UP:
				player_set_current_way(player, NORTH);
				player_move(player, map);
				break;
			case SDLK_DOWN:
				player_set_current_way(player, SOUTH);
				player_move(player, map);
				break;
			case SDLK_RIGHT:
				player_set_current_way(player, EAST);
				player_move(player, map);
				break;
			case SDLK_LEFT:
				player_set_current_way(player, WEST);
				player_move(player, map);
				break;
			case SDLK_SPACE:
				bomb_start(player, map);
				break;
			case SDLK_p: //Break
				return game_break(map);		
			default:
				break;
			}

			break;
		}
	}
	return 0;
}

int game_over(struct game* game){
	int win_condition = player_get_win_condition(game_get_player(game));
	if (win_condition>0){
		screen_win_display();
		sleep(5);
		return 1;
	}
	if (win_condition==-1){ //(lives_player<=0)||(win_condition==-1)
		screen_gameover_display();
		sleep(5);
		return 1;
	}
	return 0;
}

int game_update(struct game* game) {
	if (input_keyboard(game))
		return 1; // exit game
	struct map* map=game_get_current_map(game);
	struct player* player=game_get_player(game);
	bomb_update(map,player);
	bomb_explosion_update(map);
	player_update_invicibility( player);
	door_is_player_on_door(game);
	int timer_monster=map_get_timer_monster(map);
	if (timer_monster<=SDL_GetTicks()){
		monster_update(map, player);
		boss_update(map,player);
		map_set_timer_monster(map,SDL_GetTicks()+map_get_speed_monster(map));
	}
	return (game_over(game));
	

}
