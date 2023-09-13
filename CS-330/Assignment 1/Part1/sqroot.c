#include <stdio.h>
#include<stdlib.h>
#include<math.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>

int main(int argc, char *argv[])
{
	float ans = atof(argv[argc-1]);
	ans = sqrt(ans);
	float lower = (int)ans;
	if(ans>=lower+0.5) ans = (int)lower+1;
	else ans = (int)lower;
	
	if(argc==2){
		// just return the double of the passed number
		printf("%d",(int)ans);
		return 0;
	}
	else{
		// perform the other function and then execute double on their output
		char* command[argc];

		for(int i=1; i<argc; i++){
			command[i-1] = argv[i];
		}
		command[argc-1] = NULL;
		sprintf(command[argc-2],"%d",(int)ans);

		char temp[] ="./";
		strcat(temp,command[0]);

		command[argc-1] = NULL;

		execvp(temp,command);

		printf("Exec call failed \n");

	}
	return 0;
}
