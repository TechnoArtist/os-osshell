#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>
#include <thread>
#include <sys/wait.h>

void splitString(std::string text, char d, std::vector<std::string>& result);
void vectorOfStringsToArrayOfCharArrays(std::vector<std::string>& list, char ***result);
void freeArrayOfCharArrays(char **array, size_t array_length);
void recordCommand(std::string command, std::vector<std::string>& history, int* counter); 

int main (int argc, char **argv)
{
	// Get list of paths to binary executables
	std::vector<std::string> os_path_list;
	char* os_path = getenv("PATH");
	splitString(os_path, ':', os_path_list);
	
	// Welcome message
	printf("Welcome to OSShell! Please enter your commands ('exit' to quit).\n");
	
	std::vector<std::string> command_list; // to store command user types in, split into its variour parameters
	char** command_list_exec; // command_list converted to an array of character arrays
	char** command_history = new char*[128]; 
	for(int i = 0; i < 128; i++) {
		command_history[i] = NULL; 
	}
	int history_counter = 0;
	
	
	// Repeat:
	//  Print prompt for user input: "osshell> " (no newline)
	//  Get user input for next command
	//  If command is `exit` exit loop / quit program
	//  If command is `history` print previous N commands
	//  For all other commands, check if an executable by that name is in one of the PATH directories
	//   If yes, execute it
	//   If no, print error statement: "<command_name>: Error command not found" (do include newline)
	
	bool running = true; 
	while (running) {
		
		//printf("Making command var...\n"); 
		std::string command; 
		std::string first_command;
		
		//printf("Prompting for input...\n"); 
		printf("osshell> "); 
		std::getline(std::cin, command); 
		
		//printf("Splitting string...\n"); 
		splitString(command, ' ', command_list); 
		vectorOfStringsToArrayOfCharArrays(command_list, &command_list_exec);
		
		//printf("Recording command...\n");
		if(command_history[history_counter] == NULL) {
			command_history[history_counter] = new char[command.length()]; 
		}
		strcpy(command_history[history_counter], command.c_str()); 
		
		//printf("Incrementing counter...\n");
		history_counter = (history_counter + 1) % 128;
		
		//printf("Processing command...\n");
		
        if(command_list_exec[0] != NULL) first_command = command_list_exec[0];
        
		if(command == "") {
			//Do nothing, reprompt
			command_history[history_counter] = NULL; 
			history_counter--; 
			
		} else if (first_command == "exit") {
			exit(1);
			
		} else if (first_command == "history") { 
			//Print the history
			
			//Set the defaults
			int beginning, end; 
			
			end = history_counter - 1; 
			
			if(command_history[127] == NULL) {
				beginning = 0; //If linear
			} else {
				beginning = (history_counter + 1) % 128; //If looping
			}
			
			//Check special settings
			if(command_list_exec[1] != NULL) { 
				if(strcmp(command_list_exec[1], "clear") == 0) {
					//If it's a clear command
					beginning = end; 
					history_counter = 0; 
					
				} else if (isdigit(command_list_exec[1][0])) {
					//If it's an offset command
					beginning = end - atoi(command_list_exec[1]); 
					
				}
			}
			
			//Print
			if(beginning > end) end += 128; 
			for(int i = beginning; i < end; i++) {
				printf("%3d: %s\n", i+1, command_history[i%128]); 
			}
			
		} else if (command.front() == '.' || command.front() == '/') {
			//Search for and run the command
			
            struct stat buffer;
            std::string commandLoc = first_command;
            if(stat(command.c_str(), &buffer) == 0){
					int pid = fork();

					//Child Process (Thread)
					if (pid == 0){
					    //Execute program
                        execv(command.c_str(), command_list_exec);                  
					}

                    //Parent
                    else{
                        int status;
                        waitpid(pid, &status, 0);
                    }    
				}
            else{
                std::cout << command << ": Error command not found" << std::endl;
            }
            
        } else {
			int i;
            int done = 0;
			for (i = 0; i < os_path_list.size(); i++)
			{
				struct stat buf;
				std::string pathloc = os_path_list[i];
				pathloc += ("/" + first_command);
				if(stat(pathloc.c_str(), &buf) == 0 && done == 0){
					int pid = fork();

					//Child Process (Thread)
					if (pid == 0){
					    //Execute program
                        execv(pathloc.c_str(), command_list_exec);                  
					}

                    //Parent
                    else{
                        int status;
                        waitpid(pid, &status, 0);
                        done = 1;
                    }    
				}
                else if(i == os_path_list.size() - 1 && done == 0){
                    std::cout << command << ": Error command not found" << std::endl;
                }
			}
		}
		
		freeArrayOfCharArrays(command_list_exec, command_list.size() + 1);
		
	}
	
	return 0;
}

/*
   text: string to split
   d: character delimiter to split `text` on
   result: vector of strings - result will be stored here
*/
void splitString(std::string text, char d, std::vector<std::string>& result)
{
    enum states { NONE, IN_WORD, IN_STRING } state = NONE;

    int i;
    std::string token;
    result.clear();
    for (i = 0; i < text.length(); i++)
    {
        char c = text[i];
        switch (state) {
            case NONE:
                if (c != d)
                {
                    if (c == '\"')
                    {
                        state = IN_STRING;
                        token = "";
                    }
                    else
                    {
                        state = IN_WORD;
                        token = c;
                    }
                }
                break;
            case IN_WORD:
                if (c == d)
                {
                    result.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
            case IN_STRING:
                if (c == '\"')
                {
                    result.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
        }
    }
    if (state != NONE)
    {
        result.push_back(token);
    }
}

/*
   list: vector of strings to convert to an array of character arrays
   result: pointer to an array of character arrays when the vector of strings is copied to
*/
void vectorOfStringsToArrayOfCharArrays(std::vector<std::string>& list, char ***result)
{
    int i;
    int result_length = list.size() + 1;
    *result = new char*[result_length];
    for (i = 0; i < list.size(); i++)
    {
        (*result)[i] = new char[list[i].length() + 1];
        strcpy((*result)[i], list[i].c_str());
    }
    (*result)[list.size()] = NULL;
}

/*
   array: list of strings (array of character arrays) to be freed
   array_length: number of strings in the list to free
*/
void freeArrayOfCharArrays(char **array, size_t array_length)
{
    int i;
    for (i = 0; i < array_length; i++)
    {
        if (array[i] != NULL)
        {
            delete[] array[i];
        }
    }
    delete[] array;
}






