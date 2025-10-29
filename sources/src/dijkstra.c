#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <player.h>
#include <map.h>
#include <monster.h>



struct dijkstra {
	int Choose_of_path; // 0 for random, 1 for djikstra
	int x, y;
};

struct tool { //a struct to get the information
	int distance; 
	int x, y;
	int end;
};

int dijkstra_get_Choose_of_path(struct dijkstra* dijkstra){
	assert(dijkstra);
  	return dijkstra->Choose_of_path;
};

int dijkstra_get_x(struct dijkstra* dijkstra){
	assert(dijkstra);
  	return dijkstra->x;
};

int dijkstra_get_y(struct dijkstra* dijkstra){
	assert(dijkstra);
  	return dijkstra->y;
};

struct dijkstra* dijkstra_init(int Choose_of_path,int x, int y) {
	struct dijkstra* dijkstra = malloc(sizeof(*dijkstra));
	assert(dijkstra);
	dijkstra->Choose_of_path=Choose_of_path;
	dijkstra->x = x;
	dijkstra->y = y;
	return dijkstra;
}

struct tool* tool_init(int distance,int x, int y,int end) {
	struct tool* tool = malloc(sizeof(*tool));
	assert(tool);
	tool->distance=distance;
	tool->x = x;
	tool->y = y;
	tool->end=end;
	return tool;
}





void comparison_with_neighboor(struct map* map, int x, int y ,int neighboor_x, int neighboor_y, int *map_of_distances, int *map_of_visited_cells,int distance) {
	
	int width= map_get_width(map);
	
	int neighboor_distance ;
	if (map_is_inside(map,neighboor_x,neighboor_y)) { //if the neighboor is inside the map
		if (map_of_visited_cells[neighboor_x+neighboor_y*width] == 0) { //if it hasn't been visited yet
			neighboor_distance = map_of_distances[neighboor_x+neighboor_y*width] ;
			distance = map_of_distances[x+y*width] ;
			if ((distance + 1) < neighboor_distance ) {
				map_of_distances[neighboor_x+neighboor_y*width] = distance + 1 ;
			}
		}
	}
}

	
struct tool*  next_cell(struct map* map, int *map_of_distances, int *map_of_visited_cells, int distance){
	
	int width= map_get_width(map);
	int height=map_get_height(map);
	int number_of_cell= height*width;
	int infinity =number_of_cell+1; //so we are sure that there is no path of this longer

	int i,j, i_of_min,j_of_min ;
	int end = 0 ;
	int min_distance = infinity ; //we start min_distance at infinity
	
	for(i = 0 ; i < width; i++)
		for (j = 0 ; j < height ; j++) {
			if (map_of_visited_cells[i+j*width] == 0) {
				distance = map_of_distances[i+j*width] ;
				if (distance < min_distance) {
					min_distance = distance ; //we change the minimum
					i_of_min = i ;          
					j_of_min = j ;
					end = 1 ;
				}
			}
		
	}
	struct tool* tool= tool_init(min_distance,i_of_min,j_of_min,end);
     //we set the new distance and position of the new minimum */
	return (tool) ; // if we didn't change of minimum, we need to stop there, so end =0 else end=1 and we continue
}




int *make_a_map_of_distances (struct map* map, int x, int y){

	int width= map_get_width(map);
	int height=map_get_height(map);
	int number_of_cell= height*width;
	int infinity =number_of_cell+1; //so we are sure that there is no path of this longer

	int * map_of_distances ;
	int * map_of_visited_cells;
	int end = 1 ;
	int distance ;
	int i=0;
	int j=0;

	
	map_of_visited_cells = (int *)(malloc(sizeof(int) * number_of_cell)); //we make a map of 0 for the cell accessible for the monster, and 1 for the others cells
	for (int k = 0 ; k < number_of_cell ; k++){
		int temporary_i =k%width;
		int temporary_j =(k-temporary_i)/width;
		if (!monster_move_aux(map,temporary_i,temporary_j)) //test if the CELL is a CELL where a monster can't access
			map_of_visited_cells[k] = 1 ;
		else
			map_of_visited_cells[k] = 0 ;								
	}

	map_of_distances = (int *)(malloc(sizeof(int) * number_of_cell)); //we init the distances of every cell at "infinity" or unreachable 
	for (int k = 0 ; k < number_of_cell ; k++) 
		map_of_distances[k] = infinity ;
	map_of_distances[x + y * width] = 0; //the distance of the starting cell is inited at 0, we start at the target (the player)

	while (end == 1 )  {
		struct tool* tool= next_cell(map, map_of_distances, map_of_visited_cells,distance); 
		end=tool->end;
		i=tool->x;
		j=tool->y;
		distance=tool->distance;
	
		if (end == 1) { //if the programs must continue (end =1) we test each neighboors of the cell
			comparison_with_neighboor(map,i,j,i-1, j, map_of_distances, map_of_visited_cells,distance) ;
			comparison_with_neighboor(map,i,j,i+1, j, map_of_distances, map_of_visited_cells,distance) ;
			comparison_with_neighboor(map,i,j,i, j-1, map_of_distances, map_of_visited_cells,distance) ;
			comparison_with_neighboor(map,i,j,i, j+1, map_of_distances, map_of_visited_cells,distance) ;
			map_of_visited_cells[i+j*width] = 1 ;	//now the cell has been visited
		}
	}
	free(map_of_visited_cells) ; // we don't need anymore this map
	return(map_of_distances) ;
}




struct dijkstra* dijkstra_choose_path (struct map* map,int monster_x,int monster_y, struct player* player){
	int width=map_get_width(map);
	int height=map_get_height(map);
	int x_min, y_min;
	int distance_min = width*height+1; // so we are sure every normal distance are smaller
	int x=player_get_x(player);
	int y=player_get_y(player);
	int *map_of_distances = make_a_map_of_distances (map, x, y);
	
	
	if (map_is_inside(map,monster_x-1,monster_y)){
		if(map_of_distances[monster_x+monster_y*width]<=distance_min){
			x_min=monster_x-1;
			y_min=monster_y;
			distance_min=map_of_distances[monster_x-1+monster_y*width];

		}
	}

	if (map_is_inside(map,monster_x+1,monster_y)){
		if(map_of_distances[monster_x+1+monster_y*width]<=distance_min){
			x_min=monster_x+1;
			y_min=monster_y;
			distance_min=map_of_distances[monster_x+1+monster_y*width];
		}
	}

	if (map_is_inside(map,monster_x,monster_y-1)){
		if(map_of_distances[monster_x+(monster_y-1)*width]<=distance_min){
			x_min=monster_x;
			y_min=monster_y-1;
			distance_min=map_of_distances[monster_x+(monster_y-1)*width];
		}
	}

	if (map_is_inside(map,monster_x,monster_y+1)){
		if(map_of_distances[monster_x+(monster_y+1)*width]<=distance_min){
			x_min=monster_x;
			y_min=monster_y+1;
			distance_min=map_of_distances[monster_x+(monster_y+1)*width];
		}
	}
	if (distance_min>height*width){
		return dijkstra_init(0,0,0);
	}
	return dijkstra_init(1,x_min,y_min);
}
