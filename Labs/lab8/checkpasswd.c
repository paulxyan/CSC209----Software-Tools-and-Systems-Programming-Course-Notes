#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>


#define MAXLINE 256

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];

  /* The user will type in a user name on one line followed by a password 
     on the next.
     DO NOT add any prompts.  The only output of this program will be one 
	 of the messages defined above.
     Please read the comments in validate carefully
   */

  if (fgets(user_id, MAXLINE, stdin) == -1) {

    perror("fgets");
    exit(1);

  }

  if (fgets(password, MAXLINE, stdin) == -1) {

    perror("fgets");
    exit(1);

  }


  int fd[2], r;
  

  if (pipe(fd) == -1) {

    perror("Pipe");
    exit(1);
  }

  if ((r = fork()) == -1) {

    perror("Fork");
    exit(1);

  }

  if (r == 0) {

    if (close(fd[1]) == -1) {

      perror("Close");
      exit(1);
    }

    if (dup2(fd[0], fileno(stdin)) != -1) {

      perror("dup2");
      exit(1);
    }

    if (close(fd[0]) == -1) {

      perror("Close");
      exit(1);

    }

    execlp("./validate", "./validate", NULL); // Has no arguments but reads from stdin

    perror("Execlp");

    exit(1);

  }

  else if (r > 0) {

    if (close(fd[0]) == -1) {

      perror("Close");
      exit(1);
    }
  
    if (write(fd[1], &user_id, 10) != 10 ) {

      perror("Write");
      exit(1);

    }

    if (write(fd[1], &password, 10) != 10 ) {

      perror("Write");
      exit(1);
      
    }

  }
  

  int status;

  if (wait(&status) == -1) {

    perror("Wait");
    exit(1);


  }

  if (!WIFEXITED(&status)) {

    exit(1);

  }

  int exit_status = WEXITSTATUS(status);

  if (exit_status == 1) {

    fprintf(stderr, "Process error");

  }

  else if (exit_status == 0) {

    printf(SUCCESS);
  }

  else if (exit_status == 2) {

    printf(INVALID);


  }

  else if (exit_status == 3) {


    printf(NO_USER);

  }

  
  return 0;
}
