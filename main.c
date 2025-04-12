#include "utils.h"
int main(int argc , char* argv[] ) {

    if (argc < 2) {
        const char* msg = "Usage: ./program <command> [<parameter>] \n";
        write(STDERR_FILENO, msg, strlen(msg));
        exit(-1);
    }

    if (strcmp(argv[1] , ADD) == 0) {
        if (argc < 3) {
            const char* msg = "Usage: ./program add <hunt_id>\n";
            write(STDERR_FILENO, msg, strlen(msg));
            exit(-2);
        }
        add_operation(argv[2]);

    } else if (strcmp(argv[1] , LIST) == 0) {
        if (argc < 3) {
            const char* msg = "Usage: ./program list <hunt_id>\n";
            write(STDERR_FILENO, msg, strlen(msg));
            exit(-3);
        }
        list_operation(argv[2]);

    } else if (strcmp(argv[1] , VIEW) == 0) {
        if (argc < 4) {
            const char* msg = "Usage: ./program view <hunt_id> <id>\n";
            write(STDERR_FILENO, msg, strlen(msg));
            exit(-4);
        }
        view_operation(argv[2], atoi(argv[3]));

    } else if (strcmp(argv[1] , REMOVE_HUNT) == 0 ) {
        if (argc < 3) {
            const char* msg = "Usage: ./program remove_hunt <hunt_id>\n";
            write(STDERR_FILENO, msg, strlen(msg));
            exit(-5);
        }
        remove_hunt_operation(argv[2]);

    } else if (strcmp(argv[1] , REMOVE_TREASURE) == 0) {
        if (argc < 4) {
            const char* msg = "Usage: ./program remove_treasure <hunt_id> <id>\n";
            write(STDERR_FILENO, msg, strlen(msg));
            exit(-6);
        }
        remove_treasure_operation(argv[2], atoi(argv[3]));

    } else {
        const char* msg = "Unknown command\n";
        write(STDERR_FILENO, msg, strlen(msg));
        exit(-7);
    }

    return 0;
}
