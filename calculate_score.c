//
// Created by Raul on 18/05/2025.
//
#include "utils.h"


typedef struct {
    char username[50];
    int score;
}UserScore;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <hunt_name>\n", argv[0]);
        return -1;
    }

    char hunt_name[50] ;

    strncpy(hunt_name , argv[1] , sizeof(hunt_name) - 1) ;
    hunt_name[sizeof(hunt_name) - 1] = '\0' ;

    char file_path[100] ;
    sprintf(file_path , "./%s/treasures" , hunt_name) ;


    int file_descriptor ;

    file_descriptor = open(file_path , O_RDONLY);

    if(file_descriptor < 0) {
        perror("Error openning the treasure file") ;
        return -1 ;
    }

    Treasure treasure ;
    UserScore users[100] ;
    size_t bytes ;
    int user_count  = 0 ;

    while( (bytes = read(file_descriptor , &treasure , sizeof(Treasure)) )  == sizeof(Treasure)) {
        int user_found = false ;
        int i = 0 ;
        while(i < user_count){
            if(strcmp(users[i].username , treasure.username ) == 0) {
                users[i].score += treasure.value ;
                user_found = true ;
                break ;
            }
            i++ ;
        }

        if(!user_found && user_count < 100) {
            strncpy( users[user_count].username , treasure.username , sizeof(treasure.username - 1)) ;
            users[user_count].username[sizeof(treasure.username) - 1] = '\0' ;
            users[user_count].score = treasure.value ;
            user_count++ ;
        }



    }
    close(file_descriptor) ;
    if(user_count == 0) {
        perror("No users found") ;
        return -1 ;
    }

    for(int i = 0 ; i < user_count ; i++) {
        char message[100];
        int length = snprintf(message ,sizeof(message), "Username: %s , Total score: %d\n" , users[i].username , users[i].score) ;
        write(STDOUT_FILENO , message , length ) ;
    }

    return 0;
}
