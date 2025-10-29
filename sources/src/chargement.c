#include <SDL/SDL_image.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE * load_map(char * path ){
    FILE * fp = fopen(path, "r");
    assert(fp!=NULL);
    return fp;
}


int load_map_width(char * path ){
    FILE * fp = load_map(path);
    int count = 0;
    int temporary_tab[sizeof(int)];
    char c = fgetc(fp);
    int width;
    
    while (c>=48 && c<=57){ //as long as we don't encounter a space
        temporary_tab[count]=c-48;
        c=fgetc(fp);
        count++;
    }
    if(count==1){
        width=temporary_tab[0];
    }
    if (count==2){
        width=temporary_tab[0]*10+temporary_tab[1];
    }
    return width;
}

int load_map_heigth(char * path ){
    FILE * fp = load_map(path);
    int count = 0;
    int temporary_tab[sizeof(int)];
    char c = fgetc(fp);
    int heigth;

    while (c>=48 && c<=57){ //as long as we don't encounter a space
        c=fgetc(fp);
    }
    c = fgetc(fp);
    while (c>=48 && c<=57){ //as long as we don't encounter a space
        temporary_tab[count]=c-48;
        c=fgetc(fp);
        count++;
    }
    if(count==1){
        heigth=temporary_tab[0];
    }
    if (count==2){
        heigth=temporary_tab[0]*10+temporary_tab[1];
    }
    return heigth;
}

int * load_tab_map (char * path){
    FILE * fp = load_map(path);
    char c = fgetc(fp);
    int temporary_tab[sizeof(int)];
    int temporary_count;
    int count=0;
    int heigth=load_map_heigth(path);
    int width=load_map_width(path);
    int * tab_map=malloc(width*heigth*sizeof(int));

    //we just skip the first line
    while (c>=48 && c<=57){ //as long as we don't encounter a space
        c=fgetc(fp);
    }
    c = fgetc(fp);
    while (c>=48 && c<=57){ //as long as we don't encounter a space
        c=fgetc(fp);
    }
    while (c!=EOF){
        temporary_count=0;
        while (c>=48 && c<=57){ //as long as we don't encounter a space
            temporary_tab[temporary_count]=c-48;
            c=fgetc(fp);
            temporary_count++;
        }
        if (temporary_count==1){
            tab_map[count]=temporary_tab[0];
        }
        if (temporary_count==2){
            tab_map[count]=temporary_tab[0]*10+temporary_tab[1];
        }
        if (temporary_count==3){
            tab_map[count]=temporary_tab[0]*100+temporary_tab[1]*10+temporary_tab[2];
        }
        if (temporary_count>0){
            count++;
        }
        c=fgetc(fp);
    }
    return tab_map;
}

