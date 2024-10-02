#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/wait.h>
void decode(char * input_filename, char * type, char * data){
	char deframed_data[100000], decoded_data[100000], uppercase_data[100000];
	char * filename;
	int file_descriptor[6], status;
	pipe(file_descriptor);
	pipe(file_descriptor + 2);
	pipe(file_descriptor + 4);
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		close(file_descriptor[0]);
		// Redirect output to pipe 
		if (dup2(file_descriptor[1], STDOUT_FILENO)<0) {
			perror("Error occured");
			exit(1);
		}
		for (int i = 0; i < 6; i++) {
			close(file_descriptor[i]);
		}
		// Physical Layer - decode data
		if(execlp("./physicalLayer", "./physicalLayer", "Decode", data, NULL) < 0) {
			perror("Error orrured");
			exit(1);
		}
	}
	else{ 
		wait(&status);
		pid = fork();
		if (pid == 0){
			close(file_descriptor[1]);
			close(file_descriptor[2]);
			// Read data from pipe
			read(file_descriptor[0], decoded_data, sizeof(decoded_data));
			// Redirect output to pipe
			if (dup2(file_descriptor[3], STDOUT_FILENO) < 0) {
		        	perror("Error occured");
		        	exit(1);
	       		}
			for (int i = 0; i < 6; i++) {
				close(file_descriptor[i]);
			}
			// Data Link Layer -- deframe data 
			if (execlp("./dataLinkLayer", "dataLinkLayer", "Deframe", decoded_data, NULL ) < 0){
				perror("Error orrured");
				exit(1);
			}
		}
	  	else {
	  		wait(&status);
			if (strcasecmp(type, "Producer") == 0){
				pid = fork();
				if (pid == 0) {
					close(file_descriptor[3]);
					// Read data from pipe
					read(file_descriptor[2], deframed_data, sizeof(deframed_data));
					if (strchr(input_filename,'.') != NULL) {
						filename = strtok(input_filename, ".");
					}
					strcat(filename, ".done");
					for (int i = 0; i < 6; i++) {
						close(file_descriptor[i]);
					}
					// Application Layer -- write data to a file
					if (execlp("./applicationLayer", "applicationLayer", "Write", filename, deframed_data, NULL ) < 0){
						perror("Error orrured");
						exit(1);
					}
				}
			}
			else {
				pid = fork();
				if (pid == 0) {
					close(file_descriptor[3]);
					close(file_descriptor[4]);
					// Read data from pipe
					read(file_descriptor[2], deframed_data, sizeof(deframed_data));
					// Redirect output to pipe
					if (dup2(file_descriptor[5], STDOUT_FILENO) < 0) {
						perror("Error occured");
						exit(1);
			       		}
					for (int i = 0; i < 6; i++) {
						close(file_descriptor[i]);
					}
					// Uppercase conversion
					if (execlp("./toUpperCase", "toUpperCase", deframed_data, NULL ) < 0){
						perror("Error orrured");
						exit(1);
					}
				}
				else {
					wait(&status);
					pid = fork();
					if (pid == 0) {
						close(file_descriptor[5]);
						// Read data from pipe
						read(file_descriptor[4], uppercase_data, sizeof(uppercase_data));
						if (strchr(input_filename,'.') != NULL) {
							filename = strtok(input_filename, ".");
						}
						strcat(filename, ".outf");
						for (int i = 0; i < 6; i++) {
							close(file_descriptor[i]);
						}
						// Application Layer -- write data to a file
						if (execlp("./applicationLayer", "applicationLayer", "Write", filename, uppercase_data, NULL ) < 0){
							perror("Error orrured");
							exit(1);
						}
					}
				}
			}
		}
	}
	// Close file descriptors of pipes
	for (int i = 0; i < 6; i++) {
		close(file_descriptor[i]);
	}
	wait(&status);
	wait(&status);
	wait(&status);
}
void encode(char * input_filename, char * type){
	char data[100000];
	char * filename;
	int file_descriptor[6];
	for (int i = 0; i < 6; i += 2){
		pipe(file_descriptor + i);
	}
	pid_t pid;
	int status;
	pid = fork();
	if (pid == 0) {
		close(file_descriptor[0]);
		// Redirect output to pipe
		if (dup2(file_descriptor[1], STDOUT_FILENO)<0) {
			perror("Error occured");
			exit(1);
		}
		for (int i = 0; i < 6; i++) {
			close(file_descriptor[i]);
		}
		// Application Layer -- read data from file
		if(execlp("./applicationLayer", "applicationLayer", "Read", input_filename, NULL) < 0) {
			perror("Error orrured");
			exit(1);
		}
	}
	else {
		wait(&status);
		pid = fork();
		if (pid == 0){
			close(file_descriptor[1]);
			close(file_descriptor[2]);
			// Read data from pipe
			read(file_descriptor[0], data, sizeof(data));
			// Redirect output to pipe
			if (dup2(file_descriptor[3], STDOUT_FILENO)<0) {
				perror("Error occured");
				exit(1);
			}
			for (int i = 0; i < 6; i++) {
				close(file_descriptor[i]);
			}
			// Data Link Layer -- frame data
			if (execlp("./dataLinkLayer", "dataLinkLayer", "Frame", data, NULL ) < 0){
				perror("Error orrured");
				exit(1);
			}
		}
		else {	
			wait(&status);
			pid = fork();
			if (pid == 0) {
				close(file_descriptor[3]);
				close(file_descriptor[4]);
				// Read data from pipe
			 	read(file_descriptor[2], data, sizeof(data));
			 	// Redirect output to pipe
				if (dup2(file_descriptor[5], STDOUT_FILENO) < 0) {
						perror("Error occured");
						exit(1);
			       	}
		       		for (int i = 0; i < 6; i++) {
					close(file_descriptor[i]);
				}
				// Physical Layer -- encode data
				if (execlp("./physicalLayer", "physicalLayer", "Encode", data, NULL ) < 0){
					perror("Error orrured");
					exit(1);
				}
			}
			else {
			 	wait(&status);
				pid = fork();
				if (pid == 0) {
					close(file_descriptor[5]);
					// Read data from pipe
					read(file_descriptor[4], data, sizeof(data));
					if (strchr(input_filename,'.') != NULL) {
						filename = strtok(input_filename, ".");
					}
					if(strcasecmp(type,"Producer") == 0) {
						strcat(filename, ".binf");
					}
					else {
						strcat(filename, ".chck");
					}
					FILE * file_pointer = fopen(filename, "w");
					if (file_pointer == NULL) {
						perror("Error occured while opening the file");
						exit(1);
					}
					// Write output to file
					fputs(data,file_pointer);
					printf("%s",data);
					fclose(file_pointer);
					for (int i = 0; i < 6; i++) {
						close(file_descriptor[i]);
					}
				}
			}
		}
	}
	// Close file descriptors of pipes
	for (int i = 0; i < 6; i++) {
		close(file_descriptor[i]);
	}
	wait(&status);
	wait(&status);
	wait(&status);
	wait(&status);
}

int main(int argc, char * argv[]){
	char * input_filename = argv[1];
	// Check if input filename is null
	if (input_filename == NULL) {
		perror("Please enter the input filename");
		exit(1);
	}
	char data[100000];
	int file_descriptor[4], status;
	pipe(file_descriptor);
	pipe(file_descriptor+2);
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		// Producer -- child process
		close(file_descriptor[0]);
		// Redirect output to pipe
		if (dup2(file_descriptor[1], STDOUT_FILENO) < 0) {
			perror("Error occured");
			exit(1);
		}
		close(file_descriptor[1]);
		// Encode & frame data
		encode(input_filename, "Producer");
	}
	else {	
		// Consumer -- parent process
		wait(&status);
		close(file_descriptor[1]);
		// Read data from pipe
		read(file_descriptor[0], data, sizeof(data));
		close(file_descriptor[0]);
		// Decode & deframe data
		decode(input_filename,"Consumer", data);
		pid = fork();
		if (pid == 0) {
			// Consumer -- child process
			close(file_descriptor[2]);
			// Redirect output to pipe
			if (dup2(file_descriptor[3], STDOUT_FILENO) < 0) {
				perror("Error occured");
				exit(1);
		       	}
			if (strchr(input_filename,'.') != NULL) {
				input_filename = strtok(input_filename, ".");
			}
			strcat(input_filename, ".outf");
			close(file_descriptor[3]);
			// Encode & frame data
			encode(input_filename,"Consumer");
		}
		else {
			// Producer -- parent process
			wait(&status);
			close(file_descriptor[3]);
			// Read data from pipe
			read(file_descriptor[2], data, sizeof(data));
			close(file_descriptor[2]);
			// Decode & deframe data
			decode(input_filename,"Producer",data);	
		}
	}
	// Close file descriptors of pipes
	for (int i = 0; i < 4; i++) {
		close(file_descriptor[i]);
	}
	wait(&status);
	wait(&status);
	printf("File content has been converted to uppercase.\n");
	return 0;
}

