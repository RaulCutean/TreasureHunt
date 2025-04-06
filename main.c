#include "utils.h"


int main(int argc , char* argv[] ) {

    if (argc < 2) {
        fprintf(stderr , "Usage: %s <command> [<parameter>] \n", argv[0]);
        exit(-1) ;
    }
    if (strcmp(argv[1] , ADD) == 0) {
        if (argc < 3) {
            fprintf(stderr , "Usage: %s <add> <hunt_id>\n" , argv[0]) ;
            exit(-2) ;
        }
        add_operation(argv[2]) ;
	
    }else if (strcmp(argv[1] , LIST) == 0) {
        if (argc < 3) {
            fprintf(stderr , "Usage: %s <list> <hunt_id>\n" , argv[0]) ;
            exit(-3) ;
        }
	list_operation(argv[2]);
    }
    else if (strcmp(argv[1] , VIEW) == 0) {
      if(argc < 4) {
	fprintf(stderr , "Usage: %s <view> <hunt_id> <id> " , argv[0]);
	exit(-4) ; 
      }
      view_operation(argv[2] , atoi(argv[3])); 
    }
    else if (strcmp(argv[1] , REMOVE_HUNT) == 0 ) {
      if(argc < 3) {
	fprintf(stderr , "Usage: %s <remove_hunt> <hunt_id> " , argv[0]);
	exit(-5) ; 
      }
      remove_hunt_operation(argv[2]) ; 
    }
    else if (strcmp(argv[1] , REMOVE_TREASURE) == 0) {
      if(argc < 4) {
	fprintf(stderr , "Usage: %s <remove_treasure> <hunt_id> <id> " , argv[0]);
	exit(-6) ; 
      }
      remove_treasure_operation(argv[2] , atoi(argv[3]));
      
    }else {
        fprintf(stderr , "Unknown command") ;
        exit(-7) ;
    }


    return 0;
}
