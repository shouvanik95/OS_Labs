#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}


int cd(char **args)
{
	if ( args[1] == NULL )
	{
		fprintf(stderr, "Argument should be of form \"cd <dir>\"\n");
	}
	else if( args[2] !=NULL)
	{
		fprintf(stderr, "Argument should be of form \"cd <dir>\"\n");
	}
	else
	{
		if (chdir(args[1]) != 0) {
      perror("Directory does not exist");
    }
  }
  return 1;
}


int server(char **args, char ** serverinfo)
{
	if(args[1]==NULL || args[2]==NULL)
	{
		fprintf(stderr, "Too less arguments. Argument should be of form \"server <server-IP> <server-port>\"\n");
		return -1;
	}
	else if(args[3]!=NULL)
	{
		fprintf(stderr, "Too many arguments. Argument should be of form \"server <server-IP> <server-port>\"\n");
		return -2;
	}
	else
	{
		serverinfo[0]=args[1];
		serverinfo[1]=args[2];
	}
	return 1;
}

void  main(void)
{
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;
	
	char * serverinfo[2];
	bool serverInitialized=0;
	
	
	while (1)
 {           
	 
		printf("Hello>");     
		bzero(line, MAX_INPUT_SIZE);
		gets(line);           
		//~ printf("Got command %s\n", line);
		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);

		//do whatever you want with the commands, here we just print them
		if(strcmp(tokens[0],"cd") == 0)
		{
			cd(tokens);
		}
		else if(strcmp(tokens[0],"server")==0)
		{
			int a=server(tokens,serverinfo);
			if(a>=0)
			{
				serverInitialized=1;
				printf("Server details changed succesfully.\nIP: %s \nPort: %s \n",serverinfo[0],serverinfo[1]);
			}
		}
		else
		{
			fprintf(stderr, "Invalid command\n");
		}


		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++)
		{
			free(tokens[i]);
		}
		free(tokens);
	}


}

                
