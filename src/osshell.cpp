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

    
    /************************************************************************************
     *   Example code - remove in actual program                                        *
     ************************************************************************************/
    // Shows how to loop over the directories in the PATH environment variable
    /*
    int i;
    for (i = 0; i < os_path_list.size(); i++)
    {
        printf("PATH[%2d]: %s\n", i, os_path_list[i].c_str());
    }
    */
    /************************************************************************************
     *   End example code                                                               *
     ************************************************************************************/


    // Welcome message
    printf("Welcome to OSShell! Please enter your commands ('exit' to quit).\n");

    std::vector<std::string> command_list; // to store command user types in, split into its variour parameters
    char **command_list_exec; // command_list converted to an array of character arrays
    char **command_history; 
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
    	/*
    	Recommended using the cis/stat library when checking if a file is available
    	this will allow `struct stat;` to query file vs dir, rwx perms, etc (linux specific)
    	Or std::filesystem
    	But this is 2017 std, and makefile is setting to 2011 (-std=c++11); would need to edit
    	Takes in string, can check "does this exist?" and if exists, "exec perms?"
    	*/
		
		//printf("Making command var...\n"); 
		std::string command; 
        std::string first_command;
		
		//printf("Prompting for input...\n"); 
		printf("osshell> "); 
		std::getline(std::cin, command); 
		
		//printf("Splitting string...\n"); 
		splitString(command, ' ', command_list); 
		//gives back a null-terminated list of strings in given char**
		vectorOfStringsToArrayOfCharArrays(command_list, &command_list_exec);
		//gives back an array of char*s
		//TODO create command_history based on the creation of command_list_exec
		
		//Echoing the (full and split) command...
		std::cout << "[" << command << "]" << std::endl; 
		int i = 0;
		while (command_list_exec[i] != NULL)
		{
			printf("CMD[%2d]: %s\n", i, command_list_exec[i]);
			i++;
		}
		
		//printf("Incrementing counter...\n");
		history_counter = (history_counter + 1) % 128;
		
		//printf("Recording command...\n");
		//command_history[history_counter] = command; 
		//TODO make sure this passes only the data
		
		//printf("Processing command...\n");
		
        first_command = command_list_exec[0];
		if(command == "") {
			//Do nothing, reprompt
			
		} else if (first_command == "exit") {
			exit(1);
			
		} else if (first_command == "history") { 
			//Print (or clear) history up to 128 (or X)
						
			// Setting limit (how many entries to print), and if applicable, clearing history
			
			int limit = 128; 
			if(command_list_exec[1] != NULL && strcmp(command_list_exec[1], "clear") == 0) {
				//TODO clear history (counter to 0, history var reset)
				//(still need to finish designing the history variable, command_history)
				for(int i = 0; i < 128; i++) {
					command_history[i] = NULL; 
				}
				int limit = 0; 
			} else {
				if (command_list_exec[1] != NULL) {
				//TODO check if number, then try to set that number as the limit
                }
			}
			
			// Limit obtained, printing command_list (limit is 0 if clearing)
			//loops in a circle, starting at counter, until either null or limit
			for(int i = 0; i < limit && command_history[(i+history_counter)%128] != NULL; i++) {
				printf("%s\n", command_history[(i+history_counter)%128]); 
			}
			
		} else {
			//continue; //WARNING doing "continue" skips everything left in this while loop. 
			//TODO search for executable; PATH, or ., or /
			//See main method for an example of searching PATH
			int i;
            int done = 0;
			for (i = 0; i < os_path_list.size(); i++)
			{
                struct stat buf;
                std::string pathloc = os_path_list[i];
                pathloc += ("/" + first_command);
                //std::cout << pathloc << std::endl;
				if(stat(pathloc.c_str(), &buf) == 0 && done == 0){
                    printf("Inside Path Loop!\n");
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
		
		printf("Freeing memory for `command_list_exec`...\n"); 
		freeArrayOfCharArrays(command_list_exec, command_list.size() + 1);
		printf("------\n");
		printf("Made it through one loop\n"); 
	}

    
    /************************************************************************************
     *   Example code - remove in actual program                                        *
     ************************************************************************************/
    /*
    // Shows how to split a command and prepare for the execv() function
    std::string example_command = "ls -lh";
    splitString(example_command, ' ', command_list);
    vectorOfStringsToArrayOfCharArrays(command_list, &command_list_exec);
    // use `command_list_exec` in the execv() function rather than looping and printing
    i = 0;
    while (command_list_exec[i] != NULL)
    {
        printf("CMD[%2d]: %s\n", i, command_list_exec[i]);
        i++;
    }
    // free memory for `command_list_exec`
    freeArrayOfCharArrays(command_list_exec, command_list.size() + 1);
    printf("------\n");

    // Second example command - reuse the `command_list` and `command_list_exec` variables
    example_command = "echo \"Hello world\" I am alive!";
    splitString(example_command, ' ', command_list);
    vectorOfStringsToArrayOfCharArrays(command_list, &command_list_exec);
    // use `command_list_exec` in the execv() function rather than looping and printing
    i = 0;
    while (command_list_exec[i] != NULL)
    {
        printf("CMD[%2d]: %s\n", i, command_list_exec[i]);
        i++;
    }
    // free memory for `command_list_exec`
    freeArrayOfCharArrays(command_list_exec, command_list.size() + 1);
    printf("------\n");
    */
    /************************************************************************************
     *   End example code                                                               *
     ************************************************************************************/

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






