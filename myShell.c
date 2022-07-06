#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h> 
#include <fcntl.h>
#include <wait.h>


char* Cname = "mysh";
char* tokens[1000];
int exitStatus = 0;
void name(char *ime) {
    Cname = ime;
}
void help() {
    printf("TODO\n");
    fflush(stdout);
}
void status() {
    printf("%d\n", exitStatus);
    fflush(stdout);
}
void cpcat(char** argv, int argc) {
    int in, out;

    if (argc == 1) {
        in = 0;
        out = 1;
    } else if (strcmp(argv[1], "-") == 0) {
        in = 0;
        if (argc == 3) {
            out = open(argv[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
            if (out < 0) {
                perror("");
                exit(errno);
            }
        } else {
            out = 1;
        }
        
    } else {
        in = open(argv[1], O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);
        if (in < 0) {
            perror("");
            exit(errno);
        }

        if (argc == 3) {
            out = open(argv[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
            if (out < 0) {
                perror("");
                exit(errno);
            }
        } else {
            out = 1;
        }

    }
    char *buf[1];
    
    while(read(in, buf, 1) != 0) {
        if (errno != 0) {
            perror("");
            exit(errno);
        }
        write(out, buf, 1);
        if (errno != 0) {
            perror("");
            exit(errno);
        }
    }
}
void izvedi(char** args, int argsIndex) {
    if (strcmp(args[0], "name") == 0) {
        if(argsIndex > 1) {
            name(args[1]);
        } else 
            printf("%s\n",Cname);
            fflush(stdout);
    } else if (strcmp(args[0], "help") == 0) {
        help();
    } else if(strcmp(args[0], "status") == 0) {
        status();
    } else if(strcmp(args[0], "exit") == 0) {
        exit(atoi(args[1]));
    } else if(strcmp(args[0], "print") == 0) {
        for(int i = 1; i < argsIndex; i++) {
            if(i == 1) {
                printf("%s", args[i]);
                fflush(stdout);                
            } else {
                printf(" %s", args[i]);
                fflush(stdout);
            }
        }
    }else if(strcmp(args[0], "echo") == 0) {
        for(int i = 1; i < argsIndex; i++) {
            if(i == 1) {
                printf("%s", args[i]);
                fflush(stdout);                
            } else {
                printf(" %s", args[i]);
                fflush(stdout);
            }
        }
        printf("\n");
        fflush(stdout);
        
    } else if(strcmp(args[0], "pid") == 0) {
        printf("%d\n", getpid());
        fflush(stdout);

    } else if(strcmp(args[0], "dirchange") == 0) {
        if(argsIndex > 1) {
            if (chdir(args[1]) == -1) {
                perror("dirchange");
                exitStatus = errno;
            } else {
                exitStatus= 0;
            }

        } else {
            if (chdir("/") == -1) {
                perror("dirchange");
                exitStatus = errno;
            } else {
                exitStatus= 0;
            }
            
        }
    } else if(strcmp(args[0], "dirwhere") == 0) {
        char currDir[1024];
        if(getcwd(currDir, 1024) == NULL) {
            perror("dirwhere");
            exitStatus = errno;
        } else {
            exitStatus = 0;
        }
        printf("%s\n", currDir);
        fflush(stdout);
    } else if(strcmp(args[0], "dirmake") == 0) {
        //args[1][strlen(args[1])-1] = '\0';
        
        
        if(mkdir(args[1], 0755) != 0) {
            exitStatus = errno;
            fflush(stdout);
            perror("dirmake");
            
        } else {
            exitStatus = 0;
        }
    } else if(strcmp(args[0], "dirremove") == 0) {
        if(rmdir(args[1]) == -1) {
            fflush(stdout);
            perror("dirremove");
            exitStatus = errno;
        } else {
            exitStatus = 0;
        }
    } else if(strcmp(args[0], "dirlist") == 0) {
        int first = 1;
        DIR *d;
        struct dirent *dir;
        char *path = args[1];
        //printf("*******%s\n" ,args[2]);
        if (argsIndex < 2) {
            path = ".";
            
        } 
        
        if((d = opendir(path)) == NULL) {
            perror("dirlist");
            exitStatus = errno;
        } else {
            exitStatus = 0;
            errno = 0;
            while((dir = readdir(d)) != NULL) {
                if (first) {
                    printf("%s", dir->d_name);
                    fflush(stdout);
                    first = 0;
                } else {
                    printf("  %s", dir->d_name);
                    fflush(stdout);
                }
                
            }
            printf("\n");
            fflush(stdout);
            if(errno != 0) {
                exitStatus = errno;
            }
            closedir(d);
        }
    } else if(strcmp(args[0], "rename") == 0) {
        if(rename(args[1], args[2]) != 0) {
            exitStatus = errno;
            perror("rename");
        }
        
    } else if(strcmp(args[0], "linkhard") == 0) {
        if(link(args[1], args[2]) != 0) {
            perror("linkhard");
            exitStatus = errno;
        } else {
            exitStatus = 0;
        }
    } else if(strcmp(args[0], "linksoft") == 0) {
        if(symlink(args[1], args[2]) != 0) {
            perror("linksoft");
            exitStatus = errno;
        } else {
            exitStatus = 0;
        }
    } else if(strcmp(args[0], "linkread") == 0) {
        char* buff = malloc(sizeof(char) * 1024);
        int numRead = readlink(args[1], buff, 1024);
        if( numRead == -1) {
            perror("readlink");
            exitStatus = errno;
        } else {
            buff[numRead] = '\0';
            printf("%s\n", buff);
            fflush(stdout);
            exitStatus = 0;
        }
        
    } else if(strcmp(args[0], "linklist") == 0) {
        struct dirent *file;
        struct stat info;
        DIR *directory;
        ino_t imeIno;
        //char* fileName = malloc(1024 * sizeof(char));
        if(stat(args[1], &info) != 0) {
            perror("stat");
            exitStatus = errno;
        } else {
            imeIno = info.st_ino;
            exitStatus = 0;
        }
        char* currDir = malloc(sizeof(char) * 1024);
        getcwd(currDir, 1024);
        directory = opendir(currDir);
        if (directory == NULL) {
            perror("opendir");
            exitStatus = errno;
        } else {
            exitStatus = 0;
        }
        char* currFile = malloc(1024 * sizeof(char));
        while((file = readdir(directory)) != NULL) {
            if (lstat(file->d_name, &info) != 0) {
                perror("lstat");
                exitStatus = errno;
            } else {
                if (info.st_ino == imeIno) {
                    printf("%s  ", file->d_name);
                    fflush(stdout);
                }
                exitStatus = 0;
            }
            
        }
        printf("\n");
        fflush(stdout);

        

    } else if(strcmp(args[0], "unlink") == 0) {
        if(unlink(args[1]) != 0) {
            perror("unlink");
            exitStatus = errno;
        } else {
            exitStatus = 0;
        }
    } else if(strcmp(args[0], "cpcat") == 0) {
        cpcat(args, argsIndex);
    } else if(strcmp(args[0], "idsli") == 0) {
    
    } else if(strcmp(args[0], "idsli") == 0) {
    
    } else if(strcmp(args[0], "idsli") == 0) {
    
    } else if(strcmp(args[0], "idsli") == 0) {
    
    } else if(strcmp(args[0], "idsli") == 0) {
    
    }else if(strcmp(args[0], "idsli") == 0) {
    
    } else {
        if(fork() == 0) {
            execvp(args[0], args);

        } else {
            wait(NULL);
        }
    }
}

void beri(char* input) {
    char* args[100];
    for(int i = 0; i < 100; i++) {
        args[i] = NULL;
    }
    char* curr = calloc(100 , sizeof(char)); 
    //sscanf(input, "%s", curr);
    int argsIndex = 0;
    int j = 0;
    int jeNarekovaj = -1;
    for (int i = 0; i < strlen(input); i++) {
        char c = input[i];
        // printf("%c %d\n", c, c);
        fflush(stdout);
        if (c == '"') {
            jeNarekovaj*=-1;
            continue;
        }
        if((c != ' ' && c != '\n' && c != '\r') || (jeNarekovaj == 1 && c != '\n')) {
            curr[j] = c;
            j++;
        } else {
            j = 0;
            
            if(strlen(curr) == 0) continue;
            args[argsIndex] = curr;
            //free(curr);
            curr = calloc(100, sizeof(char));
            argsIndex++;
        }
    }

    int background = 0;
    if(strcmp(args[argsIndex-1], "&") == 0) {
        argsIndex--;
        background = 1;
    } 

    
    int numRedirects = 0;
    int fIn = -1;
    int fOut = -1;
    for (int i = 0; i < argsIndex; i++) {
        if(args[i][0] == '<') {
            fIn = dup(0);
            int f = open(args[i]+1, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);
            dup2(f, 0);
            close(f);
            numRedirects++;

        } 
        if(args[i][0] == '>') {
            fOut = dup(1);
            int f = open(args[i]+1, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
            dup2(f, 1);
            close(f);
            numRedirects++;
        }
            
    }
    argsIndex -= numRedirects;
    args[argsIndex] = NULL;
    

    if(background) {
        if(fork() == 0) {
            izvedi(args, argsIndex);
            close(0);
            close(1);
            exit(0);
        }
    } else {
        izvedi(args, argsIndex);
    }
    if(fOut != -1) {
        dup2(fOut, 1);
        close(fOut);
    }
    if(fIn != -1) {
        dup2(fIn, 0);
        close(fIn);

    }

    
    for(int i =0;i<argsIndex;i++){
        args[i] = NULL;
    }
}
int main(int argc, char **argv) {
    char *s = malloc(sizeof(char)*1000);
    size_t size = 1000;
    
    getline(&s, &size,  stdin);
    while(1) {
        if(s[0] == '#' || isspace(s[0])) {
        } else {
            beri(s);
        }
        
       
        
        if(getline(&s, &size, stdin)<0){
            break;
        }
    }
}