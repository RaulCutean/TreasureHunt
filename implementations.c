#include "utils.h"

struct stat st = {0} ;
 
void add_operation(const char* hunt_id) {
  DIR* directory = opendir(hunt_id);
  char newDirectoryPath[50];
  char fileToWrite[150];
  char huntLogFile[150];
  char rootLogFile[150];

  bool flag = false;
  sprintf(rootLogFile , "./%s-%s" , ROOT_LOG_FILE , hunt_id);

  if(directory == NULL) {
    flag = true;
    sprintf(newDirectoryPath  ,  "./%s" , hunt_id);
    if(!create_directory(newDirectoryPath)) return;
  }

  if(flag) {
    directory = opendir(newDirectoryPath);
    if(directory == NULL) {
      const char* msg = "Error opening directory\n";
      write(STDERR_FILENO, msg, strlen(msg));
      exit(-1);
    }
    sprintf(fileToWrite , "%s/%s", newDirectoryPath , TREASURE);
    sprintf(huntLogFile , "%s/%s" , newDirectoryPath , LOGGED_HUNT);
    if(symlink(huntLogFile , rootLogFile) == -1) {
      const char* msg = "Error creating symbolic link\n";
      write(STDERR_FILENO, msg, strlen(msg));
      exit(-1);
    }
  } else {
    sprintf(fileToWrite , "%s/%s" , hunt_id , TREASURE);
    sprintf(huntLogFile , "%s/%s" , hunt_id , LOGGED_HUNT);
  }

  int fd = open(fileToWrite, O_WRONLY | O_CREAT | O_APPEND, 0644);
  if(fd == -1) {
    const char* msg = "Error opening the file\n";
    write(STDERR_FILENO, msg, strlen(msg));
    exit(-1);
  }

  Treasure treasure = create_treasure();
  if(write(fd, &treasure, sizeof(treasure)) != sizeof(treasure)) {
    const char* msg = "Error writing treasure\n";
    write(STDERR_FILENO, msg, strlen(msg));
    exit(-1);
  }
  close(fd);

  int logFd = open(huntLogFile, O_WRONLY | O_CREAT | O_APPEND, 0644);
  if(logFd == -1) {
    const char* msg = "Error opening the log file\n";
    write(STDERR_FILENO, msg, strlen(msg));
    exit(-1);
  }

  char logMsg[100];
  int len = snprintf(logMsg, sizeof(logMsg), "Added treasure with the id: %d\n", treasure.id);
  write(logFd, logMsg, len);
  close(logFd);

  if(closedir(directory)) {
    const char* msg = "Error closing the directory\n";
    write(STDERR_FILENO, msg, strlen(msg));
  }
}


void list_operation(const char* hunt_id) {
  DIR* dir = opendir(hunt_id);
  if(dir == NULL) {
    const char* msg = "There isn't any hunt with that name\n";
    write(STDERR_FILENO , msg , strlen(msg));
    exit(-1);
  }

  char huntLogFile[150];
  char fileToRead[150];
  sprintf(fileToRead , "./%s/treasures" , hunt_id);
  sprintf(huntLogFile , "./%s/logged_hunt" , hunt_id);

  int fd = open(fileToRead, O_RDONLY);
  if(fd == -1) {
    const char* msg = "Error opening the treasure file\n";
    write(STDERR_FILENO , msg , strlen(msg));
    exit(-1);
  }

  struct stat fileStat;
  if (stat(fileToRead, &fileStat) == 0) {
    char timeBuf[100];
    struct tm* timeinfo = localtime(&fileStat.st_mtime);
    strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", timeinfo);
    char modMsg[200];
    int modLen = snprintf(modMsg, sizeof(modMsg), "Last modified: %s\n", timeBuf);
    write(STDOUT_FILENO, modMsg, modLen);
  }


  off_t fileSize = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  char header[200];
  int hlen = snprintf(header, sizeof(header), "File name: %s\nFile size: %ld\n\n", hunt_id, fileSize);
  write(STDOUT_FILENO, header, hlen);

  Treasure treasure;
  while(read(fd, &treasure, sizeof(Treasure)) == sizeof(Treasure)) {
    print_treasure(treasure);
  }
  close(fd);

  int logFd = open(huntLogFile, O_WRONLY | O_CREAT | O_APPEND, 0644);
  if(logFd == -1) {
    const char* msg = "Error opening the log file\n";
    write(STDERR_FILENO , msg , strlen(msg));
    exit(-1);
  }
  char logMsg[150];
  int len = snprintf(logMsg, sizeof(logMsg), "Treasures from the hunt \"%s\" were listed\n", hunt_id);
  write(logFd, logMsg, len);
  close(logFd);

  closedir(dir);
}

void view_operation(const char* hunt_id , int id) {
  DIR* directory = opendir(hunt_id);
  if(directory == NULL) {
    const char* msg = "No hunts with that name\n";
    write(STDERR_FILENO , msg , strlen(msg));
    exit(-1);
  }

  char fileToRead[150];
  sprintf(fileToRead , "./%s/treasures" ,hunt_id);
  int fd = open(fileToRead , O_RDONLY);
  if(fd == -1) {
    const char* msg = "Error opening the file\n";
    write(STDERR_FILENO , msg , strlen(msg));
    exit(-1);
  }
  Treasure treasure;
  while(read(fd, &treasure, sizeof(Treasure)) == sizeof(Treasure)) {
    if(treasure.id == id) {
      print_treasure(treasure);
      close(fd);
      closedir(directory);
      return;
    }
  }
  close(fd);
  closedir(directory);
}

void remove_hunt_operation(const char* hunt_id) {
  DIR* directory = opendir(hunt_id);
  if(directory == NULL) {
    char msg[200];
    int len = snprintf(msg, sizeof(msg), "No hunts with: %s name\n", hunt_id);
    write(STDERR_FILENO, msg, len);
    exit(-1);
  }

  char rootLogFile[150];
  char huntLogFile[150];

  sprintf(huntLogFile , "%s/%s" , hunt_id , LOGGED_HUNT);
  sprintf(rootLogFile , "%s-%s" , ROOT_LOG_FILE , hunt_id);

  if(unlink(rootLogFile) < 0) {
    const char* msg = "Error deleting symbolic link\n";
    write(STDERR_FILENO, msg, strlen(msg));
    return;
  }

  char fileName[300];
  struct dirent* entity;

  while((entity = readdir(directory)) != NULL) {
    if(strcmp(entity->d_name , ".") == 0 || strcmp(entity->d_name , "..") == 0) {
      continue;
    }
    sprintf(fileName , "./%s/%s" , hunt_id , entity->d_name);
    if(unlink(fileName) == -1) {
      const char* msg = "Error deleting the file\n";
      write(STDERR_FILENO , msg , strlen(msg));
      exit(-1);
    }
  }

  if(rmdir(hunt_id) == -1) {
    char msg[200];
    int len = snprintf(msg, sizeof(msg), "Error deleting the directory: %s\n", hunt_id);
    write(STDERR_FILENO , msg , len);
    exit(-1);
  }

  closedir(directory);
}

void remove_treasure_operation(const char* hunt_id , int id) {
  DIR* directory = opendir(hunt_id);
  if(directory == NULL){
    char msg[200];
    int len = snprintf(msg, sizeof(msg), "There isn't any hunt with: %s name\n", hunt_id);
    write(STDERR_FILENO, msg, len);
    exit(-1);
  }

  char fileToRead[150];
  char copyFile[150];
  char huntLogFile[150];

  sprintf(fileToRead  , "./%s/%s" , hunt_id , TREASURE);
  sprintf(copyFile  , "./%s/%s-%s" , hunt_id , TREASURE , COPY);
  sprintf(huntLogFile , "./%s/%s" , hunt_id , LOGGED_HUNT);

  int fd = open(fileToRead, O_RDONLY);
  if(fd == -1) {
    const char* msg = "Error opening the treasure file\n";
    write(STDERR_FILENO , msg , strlen(msg));
    exit(-1);
  }
  int copyFd = open(copyFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if(copyFd == -1) {
    const char* msg = "Error opening the copy treasure file\n";
    write(STDERR_FILENO , msg , strlen(msg));
    exit(-1);
  }
  int logFd = open(huntLogFile, O_WRONLY | O_CREAT | O_APPEND, 0644);
  if(logFd == -1) {
    const char* msg = "Error opening the log file\n";
    write(STDERR_FILENO , msg , strlen(msg));
    exit(-1);
  }

  bool found = false;
  Treasure treasure;
  while(read(fd, &treasure, sizeof(Treasure)) == sizeof(Treasure)) {
    if(treasure.id == id) {
      print_treasure(treasure);
      found = true;
      continue;
    }
    if(write(copyFd, &treasure, sizeof(Treasure)) != sizeof(Treasure)) {
      const char* msg = "Error writing to temp file\n";
      write(STDERR_FILENO , msg , strlen(msg));
      exit(-1);
    }
  }

  if(!found) {
    char msg[150];
    int len = snprintf(msg, sizeof(msg), "Treasure with the id %d doesn't exist\n", id);
    write(STDOUT_FILENO, msg, len);
    close(fd);
    close(copyFd);
    len = snprintf(msg, sizeof(msg), "Treasure with the id %d doesn't exist\n", id);
    write(logFd, msg, len);
    unlink(copyFile);
    close(logFd);
    return;
  }

  char logMsg[150];
  int len = snprintf(logMsg, sizeof(logMsg), "Treasure with the id %d was deleted\n", id);
  write(logFd, logMsg, len);

  close(fd);
  close(copyFd);
  close(logFd);

  if(unlink(fileToRead) == -1) {
    const char* msg = "Error removing the treasure file\n";
    write(STDERR_FILENO , msg , strlen(msg));
    exit(-1);
  }
  if(rename(copyFile , fileToRead) < 0) {
    const char* msg = "Error renaming the temp file\n";
    write(STDERR_FILENO , msg , strlen(msg));
    exit(-1);
  }
  closedir(directory);
}


void prompt(const char* msg) {
    write(STDOUT_FILENO, msg, strlen(msg));
}


void read_line(char* buffer, size_t size) {
    size_t i = 0;
    char ch;
    while (i < size - 1) {
        ssize_t n = read(STDIN_FILENO, &ch, 1);
        if (n <= 0 || ch == '\n') break;
        buffer[i++] = ch;
    }
    buffer[i] = '\0';
}

Treasure create_treasure() {
    Treasure treasure;
    char buffer[128];


    do {
        prompt("Insert the id (positive integer): ");
        read_line(buffer, sizeof(buffer));
        treasure.id = atoi(buffer);
    } while (treasure.id <= 0);


    do {
        prompt("Insert the username (non-empty): ");
        read_line(treasure.username, sizeof(treasure.username));
    } while (strlen(treasure.username) == 0);


    do {
        prompt("Insert the x coordinate (non-negative): ");
        read_line(buffer, sizeof(buffer));
        treasure.coordinates.x = atof(buffer);
    } while (treasure.coordinates.x < 0);


    do {
        prompt("Insert the y coordinate (non-negative): ");
        read_line(buffer, sizeof(buffer));
        treasure.coordinates.y = atof(buffer);
    } while (treasure.coordinates.y < 0);


    do {
        prompt("Insert the clue (non-empty): ");
        read_line(treasure.clue, sizeof(treasure.clue));
    } while (strlen(treasure.clue) == 0);


    do {
        prompt("Insert the value (>= 0): ");
        read_line(buffer, sizeof(buffer));
        treasure.value = atoi(buffer);
    } while (treasure.value < 0);

    return treasure;
}

/*
void print_treasure(Treasure treasure) {
    printf("Treasure ID: %d\n", treasure.id);
    printf("Username: %s\n", treasure.username);
    printf("Coordinates: (%.2f, %.2f)\n", treasure.coordinates.x, treasure.coordinates.y);
    printf("Clue: %s\n", treasure.clue);
    printf("Value: %d\n", treasure.value);
    printf("\n") ;
    }*/


void print_treasure(Treasure treasure) {
    char buffer[512];

    int len = snprintf(buffer, sizeof(buffer),
        "Treasure ID: %d\n"
        "Username: %s\n"
        "Coordinates: (%.2f, %.2f)\n"
        "Clue: %s\n"
        "Value: %d\n\n",
        treasure.id,
        treasure.username,
        treasure.coordinates.x, treasure.coordinates.y,
        treasure.clue,
        treasure.value
    );

    write(STDOUT_FILENO, buffer, len);
}


bool create_directory(const char* directoryName) {
    if (mkdir(directoryName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
        const char* msg = "Error creating new hunt directory\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return false;
    }
    return true;
}


