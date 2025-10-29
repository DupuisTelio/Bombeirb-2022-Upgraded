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
#include <game.h>

void player_encounter_closed_door(struct player* player, struct map* map, int x, int y){
	if(player_get_nb_key(player)>0){
        enum cell_type correct_opened_door = map_get_cell_type(map,x,y) +map_get_cell_subtype(map,x,y)+1; //cell type-> door, subtype -> the level indicated by the door, +1-> to open the door
        map_set_cell_type(map,x,y,correct_opened_door); //we set the new door 
		player_set_keys(player,player_get_nb_key(player)-1);
	}
}

void door_is_player_on_door(struct game* game){
	struct player* player = game_get_player(game);
	struct map* map = game_get_current_map(game);
	int x = player_get_x(player);
	int y = player_get_y(player);
    if ((map_get_cell_type(map,x,y)==CELL_DOOR)&&(map_get_door_subtype(map,x,y)==DOOR_CLOSED)){
        player_encounter_closed_door( player, map, x,  y);
    }
	if ((map_get_cell_type(map,x,y)==CELL_DOOR)&&(map_get_door_subtype(map,x,y)==DOOR_OPENED)){
        //printf(" %i\n",(map_get_cell_subtype(map,x,y) & 0x0e));
        switch (map_get_cell_subtype(map,x,y) & 0x0e){ //door level are coded with xxxx 000x the 0 there so the 2nd, third and 4th bits
            case 0:
                printf("0\n");
                game_change_of_level(game,0);
                break;
            case 2:
                printf("1\n");
                game_change_of_level(game,1);
                break;
            case 4:
                printf("2\n");
                game_change_of_level(game,2);
                break;
            case 6:
                printf("3\n");
                game_change_of_level(game,3);
                break;
            case 8:
                printf("4\n");
                game_change_of_level(game,4);
                break;
            case 10:
                printf("5\n");
                game_change_of_level(game,5);
                break;
            case 12:
                printf("6\n");
                game_change_of_level(game,6);
                break;
            case 14:
                printf("7\n");
                game_change_of_level(game,7);
                break;
        }
	}
}
