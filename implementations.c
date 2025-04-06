#include "utils.h"

struct stat st = {0} ;
 

void add_operation(const char* hunt_id) {
  DIR* directory = opendir(hunt_id);
  
  char newDirectoryPath[50];
  char fileToWrite[150];

  
  char huntLogFile[150] ;
  char rootLogFile[150] ; 

  bool flag = false ; 

  sprintf(rootLogFile , "./%s-%s" , ROOT_LOG_FILE , hunt_id) ;
    
  if(directory == NULL) {

    flag = true ;
    sprintf(newDirectoryPath  ,  "./%s" , hunt_id) ;
   
    if(!create_directory(newDirectoryPath)) {
      return ; 
    }
    
  }

  if(flag) {
    directory = opendir(newDirectoryPath);
    if(directory == NULL) {
      fprintf(stderr , "Error openning directory") ;
      exit(-1) ;  
    }  
    sprintf(fileToWrite , "%s/%s", newDirectoryPath , TREASURE) ;
    sprintf(huntLogFile , "%s/%s" , newDirectoryPath , LOGGED_HUNT);

    if(symlink(huntLogFile , rootLogFile) == -1) {
      fprintf(stderr , "Error creating the symbolic link");
      exit(-1) ;  
    } 
    
  }else {
    sprintf(fileToWrite , "%s/%s" , hunt_id , TREASURE  ) ;
    sprintf(huntLogFile , "%s/%s" , hunt_id , LOGGED_HUNT ) ;
  
  }
  //printf("%s\n" , rootLogFile) ;
  // printf("%s" , huntLogFile) ;
  
  FILE* file = fopen(fileToWrite , "a");
  if(file == NULL) {
    fprintf(stderr , "Error openning the file") ;
    exit(-1) ;  
  }

  Treasure treasure = create_treasure() ;
  if( fwrite(&treasure , sizeof(treasure) , 1 , file) != 1){
    fprintf(stderr , "Error writing treasure") ;
    exit(-1) ;  
  }
  
  FILE* logFile = fopen(huntLogFile , "a") ;
  if(logFile == NULL) {
    fprintf(stderr , "Error openning the log file") ;
    exit(-1); 
  }
  
  if(fprintf(logFile , "Added treasure with the id: %d\n" , treasure.id) < 0) {
    fprintf(stderr , "Error logging operation to: %s", huntLogFile) ;
    exit(-1) ;
    
  }
  
  if(fclose(file)) {
    fprintf(stderr , "Error closing the file") ;
    return ; 
  }
  
  if(fclose(logFile)) {
    fprintf(stderr , "Error closing the log file") ;
    return ;  
  }
  
  
  if(closedir(directory)) {
    fprintf(stderr , "Error closing the directory") ;
    return ; 
  } 
  
  
}

void list_operation(const char* hunt_id) {
  DIR* dir = opendir(hunt_id) ;
  if(dir == NULL) {
    fprintf(stderr , "There insn't any hunt with : %s name" , hunt_id) ;
    exit(-1) ; 
  }

  char huntLogFile[150] ;
  char fileToRead[150];
  
  sprintf(fileToRead , "./%s/treasures" , hunt_id) ;
  sprintf(huntLogFile , "./%s/logged_hunt" , hunt_id) ;  

  FILE* file = fopen(fileToRead , "rb") ; 
  if(file == NULL) {
    fprintf(stderr , "Error openning the treasure file\n") ;
    exit(-1) ;
  }
  
  fseek(file , 0L , SEEK_END) ;
  size_t fileSize = ftell(file) ; 
  rewind(file) ;
  
  printf("File name: %s\nFile size: %lu\n\n" , hunt_id , fileSize ) ; 
  Treasure treasure ; 
  while(fread(&treasure , sizeof(Treasure) , 1 , file)) {
    print_treasure(treasure) ; 
  }
  
  FILE* logFile = fopen(huntLogFile , "a" ); 
  if(logFile == NULL) {
    fprintf(stderr , "Error openning the file") ;
    exit(-1);  
  }
  if(fprintf(logFile , "Treasures from the hunt \"%s\" were listed\n" , hunt_id) < 0) {
    fprintf(stderr , "Error logging operation to: %s" , huntLogFile) ;
    exit(-1) ;  
  } 
  
  if(fclose(file)) {
    fprintf(stderr , "Error closing the file") ;
    return ; 
  }
  
  if(fclose(logFile)) {
    fprintf(stderr , "Error closing the log file") ;
    return ;  
  }
  
  
  if(closedir(dir)) {
    fprintf(stderr , "Error closing the directory") ;
    return ; 
  } 
  
}

void view_operation(const char* hunt_id , int id) {
  DIR* directory = opendir(hunt_id) ;
  if(directory == NULL) {
    fprintf(stderr , "No hunts with : %s name" , hunt_id) ;
    exit(-1) ;  
  }
  char fileToRead[150] ;
  sprintf(fileToRead , "./%s/treasures" ,hunt_id );

  FILE* file = fopen(fileToRead , "rb");
  if(file == NULL) {
    fprintf(stderr , "Error openning the file") ;
    exit(-1) ;  
  }
  Treasure treasure ; 
  while(fread(&treasure , sizeof(Treasure) , 1 , file)) {
    if(treasure.id == id) {
      print_treasure(treasure) ; 
      return ; 
    }  
  }   
  
  
}

void remove_hunt_operation(const char* hunt_id) {
  DIR* directory = opendir(hunt_id) ;
  if(directory == NULL) {
    fprintf(stderr , "No hunts with: %s name" , hunt_id) ;
    exit(-1) ;  
  }
  char rootLogFile[150] ;
  char huntLogFile[150] ;

  sprintf(huntLogFile , "%s/%s" , hunt_id , LOGGED_HUNT ) ;
  sprintf(rootLogFile , "%s-%s" , ROOT_LOG_FILE , hunt_id) ;
  
  if(unlink(rootLogFile) < 0) {
    fprintf(stderr , "Error deleting symbolic link");
    return ; 
  }
   
  char fileName[300]; 
  struct dirent* entity;
  
 
  while((entity = readdir(directory)) != NULL) {
   
    if(strcmp(entity->d_name , ".") == 0 || strcmp(entity->d_name , "..") == 0) {
      continue ;  
    }
    sprintf(fileName , "./%s/%s" , hunt_id , entity->d_name) ;
    printf("%s\n" , fileName) ; 
    if(remove(fileName) == -1) {
      fprintf(stderr , "Error deleting the file");
      exit(-1); 
    }
    
  }
  
  if(rmdir(hunt_id) == -1) {
    fprintf(stderr , "Error deleting the directory: %s" , hunt_id) ;
    exit(-1) ;  
  }  
  
  closedir(directory) ;  
  
}
void remove_treasure_operation(const char* hunt_id , int id) {

  
   DIR* directory = opendir(hunt_id) ;
   if(directory == NULL){
     fprintf(stderr , "There insn't any hunt with : %s name" , hunt_id);
     exit(-1) ; 
   }
   char fileToRead[150] ;
   
   char copyFile[150] ;

   char huntLogFile[150];

   sprintf(fileToRead  , "./%s/%s" , hunt_id , TREASURE);
   
   sprintf(copyFile  , "./%s/%s-%s" , hunt_id , TREASURE , COPY);

   sprintf(huntLogFile , "./%s/%s" , hunt_id , LOGGED_HUNT) ;
   
   
   FILE* file = fopen(fileToRead , "rb") ;
   if(file == NULL) {
     fprintf(stderr , "Error openning the treasure file") ;
     exit(-1) ; 
   }
   FILE* copyFilePtr = fopen(copyFile , "wb") ;
   if(copyFilePtr == NULL) {
     fprintf(stderr , "Error openning the copy treasure file") ;
     exit(-1) ; 
   }
   FILE* logFile = fopen(huntLogFile, "a");
   if(logFile == NULL) {
     fprintf(stderr , "Error openning the log file") ;
     exit(-1) ;  
   } 
     
   bool found = false ; 
   Treasure treasure ; 
   while(fread(&treasure , sizeof(Treasure) , 1 , file)) {
     if(treasure.id == id) {
       print_treasure(treasure) ;
       found = true ;
       continue ; 
     }
     if(fwrite(&treasure , sizeof(Treasure) , 1 , copyFilePtr) != 1) {
       fprintf(stderr , "Error writing to temp file") ;
       exit(-1) ; 
     }  
   }
   if(!found) {
     printf("Treasure with the id %d doesn't exist\n" , id );
     if(fclose(file) == EOF) {
       fprintf(stderr , "Error closing treasure file") ;
       exit(-1); 
     }
     if(fclose(copyFilePtr) == EOF) {
       fprintf(stderr , "Error closing temp file");
       exit(-1) ; 
     }
     
     fprintf(logFile , "Treasure with the id %d doesn't exist\n" , id) ;
     
     if(remove(copyFile) == -1) {
       fprintf(stderr , "Error removing temporary file %s" , copyFile);
       exit(-1);
     }
     if(fclose(logFile)) {
        fprintf(stderr , "Error closing the logFile");
	exit(-1);
     }
     return ; 
     
   }
   
   fprintf(logFile , "Treasure with the id %d was deleted\n" , id) ;

   if(fclose(file)) {
     fprintf(stderr , "Error closing treasure file") ;
     exit(-1); 
   }
   
   if(fclose(copyFilePtr)) {
     fprintf(stderr , "Error closing temp file");
     exit(-1) ; 
   }
   
   if(fclose(logFile)) {
     fprintf(stderr , "Error closing the logFile");
     exit(-1);
   }
   if(remove(fileToRead) == -1) {
     fprintf(stderr , "Error removing the treasure file") ;
     exit(-1) ; 
   }
   if(rename(copyFile , fileToRead) < 0) {
     fprintf(stderr , "Error renaming the temp file") ;
     exit(-1) ; 
   }
   

  
} 

bool create_directory(const  char* directoryName) {

  if(mkdir(directoryName , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
    fprintf(stderr , "Error creating new hunt directory: %s" , directoryName);
    return false ; 
  } 

  return true ; 
}


Treasure create_treasure() {
  Treasure treasure ; 
  printf("Insert the id: ");
  scanf("%d" , &treasure.id) ;
  
  printf("Insert the username: ");
  scanf("%s" , treasure.username) ;

  printf("Insert the x coordinate: ");
  scanf("%f" , &treasure.coordinates.x) ;

  printf("Insert the y coordinate: ");
  scanf("%f" , &treasure.coordinates.y) ;
  
  
  printf("Insert the clue: ");
  //fgets(treasure.clue , sizeof(treasure.clue) , stdin) ;
  scanf(" %[^\n]" , treasure.clue) ; 

  printf("Insert the value: ");
  scanf("%d" , &treasure.value) ;
  

  return treasure ; 
}

void print_treasure(Treasure treasure) {
    printf("Treasure ID: %d\n", treasure.id);
    printf("Username: %s\n", treasure.username);
    printf("Coordinates: (%.2f, %.2f)\n", treasure.coordinates.x, treasure.coordinates.y);
    printf("Clue: %s\n", treasure.clue);
    printf("Value: %d\n", treasure.value);
    printf("\n") ;  
} 




