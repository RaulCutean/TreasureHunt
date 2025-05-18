#include "utils.h"
/*
struct sigaction {
               void     (*sa_handler)(int);
               void     (*sa_sigaction)(int, siginfo_t *, void *);
               sigset_t   sa_mask;
               int        sa_flags;
               void     (*sa_restorer)(void);
           };
*/


pid_t monitor_id = -1 ;
volatile sig_atomic_t done = 0 ;
int fds[2] ;

void handle_done() {
  done = 1 ;
}

int filter(const struct dirent *name)
{
  return 1;
}

void print(const char* message) {
   write(STDOUT_FILENO , message , strlen(message)) ;
}


void terminate_handler() {
  while(waitpid(-1 , NULL , WNOHANG) > 0);
  print("Monitor terminated\n") ;

}

void list_hunts() {

  struct dirent** nameList = NULL;

  int n = scandir("." , &nameList , filter , alphasort);
  if(n < 0 ) {
    perror("Directory scan failed") ;
    exit(-1) ;
  }
  while(n--){

    if(strncmp(nameList[n]->d_name, "hunt", strlen("hunt")) == 0){

      write(fds[1] , nameList[n]->d_name , strlen(nameList[n]->d_name));

      struct dirent** nameList2 = NULL ;
      int m = scandir(nameList[n]->d_name , &nameList2  , filter  , alphasort) ;
      if(m < 0 ) {
	perror("Treasure directory failed\n") ;
	exit(-1) ;
      }
      int count = 0 ;
      while(m--) {
	if(strncmp(nameList2[m]->d_name , "treasures" , strlen("treasures")) == 0) {

	  char path[512] ;
	  sprintf(path , "%s/%s" , nameList[n]->d_name , nameList2[m]->d_name) ;

	  int fd = open(path , O_RDONLY );

	  if(fd == -1) {
	    print("Error openning the treasure file") ;
	  }

	  Treasure treasure ;
	  while(read(fd, &treasure, sizeof(Treasure)) == sizeof(Treasure)) {
	    count++ ;

	  }
	  close(fd) ;
	}
      }
       char buffer[128] ;
       int length = sprintf(buffer , " %d treasures\n" , count) ;
       write(fds[1] , buffer , length);


    }
  }

  kill(getppid() , SIGTERM) ;

}

void list_treasures() {
  char buffer[64] ;
  print("Insert the name of the hunt: ") ;
  read(STDIN_FILENO , buffer , sizeof(buffer)) ;
  buffer[strcspn(buffer , "\n")] = '\0' ;
  pid_t process_id = fork() ;

  if(process_id < 0) {
    print("Error creating child process") ;
    return ;
  }

  if(process_id == 0) {
    execlp("gcc" , "gcc" , "-Wall" , "implementations.c" , "main.c" , "-o" , "app" , NULL) ;
    print("Error executing program") ;
  }
  wait(NULL) ;

  pid_t process_id2 = fork() ;

  if(process_id2 < 0) {
    print("Error creating child process") ;
    return ;
  }

  if(process_id2 == 0) {
    close(fds[0]);
    dup2(fds[1] , STDOUT_FILENO);
    close(fds[1]) ;
    execl("./app" , "./app" , "list" , buffer , NULL) ;
    print("Error executing program") ;
    //execlp("./app" , "./app" , "list" , buffer , NULL) ;
    //print("Error executing program") ;
  }
  wait(NULL) ;

  kill(getppid() , SIGTERM) ;

}

void view_treasure() {
  char buffer[64] ;
  char idBuffer[16] ;
  print("Insert the name of the hunt: ") ;
  read(STDIN_FILENO , buffer , sizeof(buffer)) ;
  buffer[strcspn(buffer , "\n")] = '\0' ;

  print("Insert the id of the treasure: ") ;
  read(STDIN_FILENO , idBuffer , sizeof(idBuffer)) ;
  idBuffer[strcspn(idBuffer , "\n")] = '\0' ;


  pid_t process_id = fork() ;

  if(process_id < 0) {
    print("Error creating child process") ;
    return ;
  }

  if(process_id == 0) {
    execlp("gcc" , "gcc" , "-Wall" , "implementations.c" , "main.c" , "-o" , "app" , NULL) ;
    print("Error executing program") ;
  }
  pid_t process_id2 = fork() ;

  if(process_id2 < 0) {
    print("Error creating child process") ;
    return ;
  }

  if(process_id2 == 0) {
    close(fds[0]);
    dup2(fds[1] , STDOUT_FILENO) ;
    close(fds[1])  ;
    execl("./app" , "./app" , "view" , buffer , idBuffer , NULL) ;
    //execlp("./app" , "./app" , "view" , buffer , idBuffer , NULL) ;
    //print("Error executing program") ;
  }

  kill(getppid() , SIGTERM) ;

}

void calculate_score() {

  struct dirent** entity = NULL;
  int n = scandir("." , &entity , filter , alphasort);
  if(n < 0) {
    print("Error scanning directory");
    return ;
  }
  for(int i = 0 ; i < n ; i++){

    if(strncmp(entity[i]->d_name , HUNT , strlen(HUNT) ) == 0) {
      pid_t pid = fork() ;
      if(pid == -1) {
	print("Error creating child process") ;
	exit(-1) ;
      }
      if(pid == 0) {
	close(fds[0]) ;
	dup2(fds[1] , STDOUT_FILENO) ;
	close(fds[1]) ;
	execl("./calc" , "./calc" , entity[i]->d_name , NULL) ;
	print("Error executing program") ;
      }

    }
  }
  kill(getppid() , SIGTERM) ;
}


void start_monitor() {

  if(monitor_id != -1) {
    print("Monitor is already running\n") ;
    return ;
  }
  monitor_id = fork() ;

  if(monitor_id < 0) {
    print("Failed to start monitor\n");
    exit(-1) ;
  }

  if(monitor_id == 0) {
    print("Started monitor\n");

    struct sigaction sg ;

    sg.sa_handler = terminate_handler ;
    sg.sa_flags = 0 ;
    sigemptyset(&sg.sa_mask);
    sigaction(SIGTERM , &sg , NULL) ;


    sg.sa_flags = 0 ;
    sigemptyset(&sg.sa_mask);
    sg.sa_handler = list_hunts ;
    sigaction(SIGUSR1 , &sg , NULL) ;

    sg.sa_handler = list_treasures ;
    sg.sa_flags = 0 ;
    sigemptyset(&sg.sa_mask);
    sigaction(SIGUSR2 , &sg , NULL) ;

    sg.sa_handler = view_treasure ;
    sg.sa_flags = 0 ;
    sigemptyset(&sg.sa_mask);
    sigaction(SIGINT , &sg , NULL) ;

    sg.sa_handler = calculate_score ;
    sg.sa_flags = 0 ;
    sigemptyset(&sg.sa_mask);
    sigaction(SIGILL , &sg , NULL) ;

    while(1) {
      pause() ;
    }
  }

}
void send_signal(int sig)
{
    if (monitor_id == -1)
    {
        print("Monitor isn't running\n");
        return;
    }
    if (kill(monitor_id, sig) == -1)
    {
      print("Error sending signal\n");
    }
}

void stop_monitor()  {
  printf("%d" , monitor_id) ;
  if(monitor_id == -1){
    print("Monitor isn't running\n");
    return ;
  }
  print("Monitor stops\n") ;


  if( kill(monitor_id , SIGTERM) == -1 ) {
    print("Error stopping monitor\n") ;
  }

  monitor_id = -1 ;
  usleep(1000000) ;


}



int main() {
  if (pipe(fds) == -1) {
    print("Error") ;
    exit(-1) ;
  }

  struct sigaction sg;
  sg.sa_handler = handle_done ;
  sg.sa_flags = 0 ;
  sigemptyset(&sg.sa_mask) ;
  sigaction(SIGTERM , &sg , NULL) ;

  char buffer[100];
  char reader[1024];

  while(read(STDIN_FILENO , buffer , sizeof(buffer) ) > 0) {
    buffer[strcspn(buffer , "\n")] = '\0' ;


    if(strcmp(buffer , START_MONITOR) == 0) {
      start_monitor() ;
    }
    else if(strcmp(buffer , LIST_HUNTS) == 0) {
      done = 0 ;
      send_signal(SIGUSR1);
      while(!done) pause();

      size_t bytes = read(fds[0] , reader , sizeof(reader) - 1);
      if(bytes > 0) {
	reader[sizeof(reader) -  1] = '\0' ;
	write(STDOUT_FILENO , reader , bytes) ;
      }
      memset(reader , 0 , sizeof(reader));
    }
    else if(strcmp(buffer , LIST_TREASURES) == 0) {
      done = 0 ;
      send_signal(SIGUSR2);
      while(!done) pause() ;

      size_t bytes = read(fds[0] , reader , sizeof(reader) - 1);
      if(bytes > 0) {
	reader[sizeof(reader) -  1] = '\0' ;
	write(STDOUT_FILENO , reader , bytes) ;
      }
      memset(reader , 0 , sizeof(reader));
    }

    else if(strcmp(buffer , VIEW_TREASURE) == 0) {
      done = 0 ;
      send_signal(SIGINT) ;
      while(!done) pause() ;
      size_t bytes = read(fds[0] , reader , sizeof(reader) - 1);
      if(bytes > 0) {
	reader[sizeof(reader) -  1] = '\0' ;
	write(STDOUT_FILENO , reader , bytes) ;
      }
      memset(reader , 0 , sizeof(reader));
    }
    else if(strcmp(buffer , STOP_MONITOR) == 0) {
      stop_monitor();

    }
    else if(strncmp(buffer , EXIT , strlen(EXIT)) == 0) {
      if(monitor_id == -1) {
	break ;
      }
      print("Monitor is still running\n");
    }else if (strncmp(buffer , "calculate_score" , strlen("calculate_score")) == 0) {
      done = 0 ;
      send_signal(SIGILL) ;
      while(!done) pause() ;

      size_t bytes = read(fds[0] , reader , sizeof(reader) - 1);
      if(bytes > 0) {
	reader[sizeof(reader) -  1] = '\0' ;
	write(STDOUT_FILENO , reader , bytes) ;
      }
      memset(reader , 0 , sizeof(reader));

    }
    else {
      print("Wrong command\n");
    }
  }



  return 0 ;
}
