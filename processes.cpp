/*
Processes.cpp
@auther: Pavel Samsonov
4/8/2018

This program behaves the same way as 
$ ps -A | grep ARGV[1] | wc -l
command
This program when run with an argument will inform the user how many programs are running that share that string in their name
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>

using namespace std;

int main(int argc, char** argv){
	enum {RD, WR};		//read,write
	pid_t pid;

	if((pid = fork()) < 0){
		perror("You just got forked! Forking failed.");
	}

	//starting the forking process
	if(pid == 0){ // I am CHILD

		int fd_child[2];	//child-grandchild pipe
		pid_t pid_child;

		if(pipe(fd_child) < 0){
			perror("Problems piping the child");
		}
		else if((pid_child = fork()) < 0){
			perror("Problems forking the child");
		}

		if(pid_child == 0){ // I am GRANDCHILD
			dup2(fd_child[WR],1);

			int fd_grandChild[2]; 	//grandchild-greatgrandchild pipe
			pid_t pid_grandChild;

			if(pipe(fd_grandChild) < 0){
				perror("Problems piping grand children");
			}
			else if((pid_grandChild = fork()) < 0){
				perror("Problems forking with the grand children");
			}

			if(pid_grandChild == 0){ // I AM THE GREAT GRAND CHILD
				dup2(fd_grandChild[WR],1);
				close(fd_grandChild[RD]);
				execlp("ps", "ps", "-A", NULL); // run ps -A
				close(fd_grandChild[WR]);

			}

			else{ // I AM THE GRAND CHILD
				dup2(fd_grandChild[RD], 0);
				close(fd_grandChild[WR]);
				close(fd_child[RD]);
				wait(NULL); //wait for ps -A output
				execlp("grep", "grep", argv[1], NULL); // run grep ARGV[1] after getting ps - A results
				close(fd_child[WR]);
			}
		}

		else{ // I am child
			dup2(fd_child[RD], 0);
			close(fd_child[WR]);
			wait(NULL); // wait for grep results
			execlp("wc", "wc", "-l", NULL); // run wc -l
		}

	}

	else{// AM ADULT
		wait(NULL);
	}
	exit(EXIT_SUCCESS);
}
