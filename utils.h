//
// Created by Raul on 04/04/2025.
//

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>


#define ADD  "add"
#define LIST "list"
#define REMOVE_HUNT "remove_hunt"
#define REMOVE_TREASURE "remove_treasure"
#define VIEW "view"

#define TREASURE "treasures"
#define LOGGED_HUNT "logged_hunt"
#define EXTENSION "./"
#define ROOT_LOG_FILE "root_log_file"
#define COPY "copy"

#define LIST_HUNTS "list_hunts"
#define START_MONITOR "start_monitor"
#define LIST_TREASURES "list_treasures"
#define VIEW_TREASURE "view_treasure"
#define STOP_MONITOR "stop_monitor"
#define EXIT "exit"

typedef struct {
    float x ;
    float  y ;
}Coordinates ;

typedef struct {
    int id ;
    char username[50] ;
    Coordinates coordinates ;
    char clue[100];
    int value ;
}Treasure;


void print_treasure(Treasure) ;

void add_operation(const char*);

void list_operation(const char* ) ;

void view_operation(const char * , int ) ;

void remove_treasure_operation(const char * , int); 

void remove_hunt_operation(const char*) ; 

bool create_directory(const  char*) ;

Treasure create_treasure() ; 



#endif //UTILS_H
