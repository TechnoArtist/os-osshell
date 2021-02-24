#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <unistd.h>

void allocateArrayOfCharArrays(char ***array_ptr, size_t array_length, size_t item_size);
void freeArrayOfCharArrays(char **array, size_t array_length);
void splitString(std::string text, char d, char **result);
void recordCommand(std::string* command, char** command_list, int* counter); 

int main (int argc, char **argv)
{
    // Get list of paths to binary executables
    // `os_path_list` supports up to 16 directories in PATH, 
    //     each with a directory name length of up to 64 characters
    char **os_path_list;
    allocateArrayOfCharArrays(&os_path_list, 16, 64);
    char* os_path = getenv("PATH");
    splitString(os_path, ':', os_path_list);


    // Example code for how to loop over NULL terminated list of strings
    int i = 0;
    while (os_path_list[i] != NULL)
    {
        printf("PATH[%2d]: %s\n", i, os_path_list[i]);
        i++;
    }


    // Welcome message
    printf("Welcome to OSShell! Please enter your commands ('exit' to quit).\n");

    // Allocate space for input command lists
    // `command_list` supports up to 32 command line parameters, 
    //     each with a parameter string length of up to 128 characters
    char **command_list;
    allocateArrayOfCharArrays(&command_list, 32, 128);

    // Repeat:
    //  Print prompt for user input: "osshell> " (no newline)
    //  Get user input for next command
    //  If command is `exit` exit loop / quit program
    //  If command is `history` print previous N commands
    //  For all other commands, check if an executable by that name is in one of the PATH directories
    //   If yes, execute it
    //   If no, print error statement: "<command_name>: Error command not found" (do include newline)
    
    int currentCommand = 0; 
    bool running = true; 
    while (running) {
    	/*
    	Recommended using the cis/stat library when checking if a file is available
    	this will allow `struct stat;` to query file vs dir, rwx perms, etc (linux specific)
    	Or std::filesystem
    	But this is 2017 std, and makefile is setting to 2011 (-std=c++11); would need to edit
    	Takes in string, can check "does this exist?" and if exists, "exec perms?"
    	*/
    	
    	//Make command var
    	std::string command; 
    	char* command_parts; 
    	
    	printf("Made command var\n"); 
    	
    	//Prompt and retrieve user input
    	printf("osshell> "); 
		std::getline(std::cin, command); 
		
		
    	//TODO use the splitString() method given by teacher, it's a delimiter splitter
    	//gives back a null-terminated list of strings in given char**
    	
    	printf("\nPrompted, recieved input\n"); 
    	
    	//splitString(command, ' ', &command_parts); 
    	
    	//printf("Split the string\n"); 
    	
    	std::cout << "[" << command << "]" << std::endl; 
    	
    	recordCommand(&command, command_list, &currentCommand); //TODO make this method, or write it here
    	
    	if(command == "exit") {
    		printf("exiting..."); 
    		running = false; 
    	} else if (command == "") {
    		//Do nothing, reprompt
    	} else if (command == "history") { //TODO split stringing
    		//TODO print (or clear) history up to 128 (or X)
    		
    		//printing command_list
    		
    		
    	} else {
    		//TODO search for executable; PATH, or ., or /
    		//Remember to thread this, carefully
    	}
    	
    	printf("Made it through one loop\n"); 
    	
    }

    // Free allocated memory
    freeArrayOfCharArrays(os_path_list, 16);
    freeArrayOfCharArrays(command_list, 32);

    return 0;
}

/*
   array_ptr: pointer to list of strings to be allocated
   array_length: number of strings to allocate space for in the list
   item_size: length of each string to allocate space for
*/
void allocateArrayOfCharArrays(char ***array_ptr, size_t array_length, size_t item_size)
{
    int i;
    *array_ptr = new char*[array_length];
    for (i = 0; i < array_length; i++)
    {
        (*array_ptr)[i] = new char[item_size];
    }
}

/*
   array: list of strings to be freed
   array_length: number of strings in the list to free
*/
void freeArrayOfCharArrays(char **array, size_t array_length)
{
    int i;
    for (i = 0; i < array_length; i++)
    {
        delete[] array[i];
    }
    delete[] array;
}

/*
   text: string to split
   d: character delimiter to split `text` on
   result: NULL terminated list of strings (char **) - result will be stored here
*/
void splitString(std::string text, char d, char **result)
{
	enum states { NONE, IN_WORD, IN_STRING } state = NONE; 
	
    int i;
    std::vector<std::string> list;
    std::string token;
    
    for(i = 0; i < text.length(); i++) {
    	char c = text[i]; 
    	switch (state) {
    		case NONE: 
    			if (c != d) {
					if (c == '\"') {
						state = IN_STRING; 
						token = ""; 
					}
					else {
						state = IN_WORD; 
						token = c; 
					}
				}
				break; 
				
    		case IN_WORD: 
    			if (c == d) {
    				list.push_back(token); 
    				state = NONE; 
    			}
    			else {
    				token += c; 
    			}
    			break; 
    			
    		case IN_STRING: 
    			if (c == '\"') {
    				list.push_back(token); 
    				state = NONE; 
    			}
    			else {
    				token += c; 
    			}
    			break; 
    	}
    }
    if (state != NONE) {
    	list.push_back(token); 
    } 

    for (i = 0; i < list.size(); i++)
    {
        strcpy(result[i], list[i].c_str());
    }
    result[list.size()] = NULL;
}

void recordCommand(std::string* command, char** command_list, int* counter) {
	
	strcpy(command_list[*counter], command->c_str()); 
	
	//TODO in the printHistory, check if null, then loop until _
	
	*(counter) = (*(counter) + 1) % 32; 
}


