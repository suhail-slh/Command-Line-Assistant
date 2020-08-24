# include <stdio.h> 
# include <string.h> 
# include <windows.h>
# include <dirent.h>
# include <sys/stat.h>
# include <unistd.h>
# define STR_SIZE 10000

void read_file(FILE *fp)
{
	char read[STR_SIZE];
	while(fgets(read,STR_SIZE,fp)!=NULL)
		printf("%s",read);
	fclose(fp); 
}

void write_file(FILE *fp, char *path)
{
	char cmd[STR_SIZE]="notepad ", timestamp[STR_SIZE];
	//replace "notepad " with "python editor_path " when editor is ready
	
	SYSTEMTIME t;
				
	GetLocalTime(&t);
	
	sprintf(timestamp,"\n%d:%d:%d %d/%d/%d\n", t.wHour, t.wMinute, t.wSecond, t.wDay, t.wMonth, t.wYear);
	
	fputs(timestamp, fp) ; 
	
	fclose(fp);
	
	strcat(cmd, path);//the file path will be taken in as an argument(argv) in the python text editor

	system(cmd);
}

void copy(FILE *sfp,FILE *dfp)
{
	char read[STR_SIZE];
	while(fgets(read,STR_SIZE,sfp)!=NULL)
		fputs(read,dfp);
	fclose(sfp);
	fclose(dfp);
}

void play_music()
{
	DIR *d;
	struct dirent *dir;
	char ch, path[STR_SIZE], fnm[STR_SIZE] = "", cmd[STR_SIZE] = "\"";
	int choice;
	
	printf("Enter the path to the music directory(use '/' as separator): ");
	scanf("%[^\n]%*c",path);
	
	d = opendir(path);
	
	if(d==NULL)
	{
		printf("Path does not exist."); 
		return;
	}
	
	printf("Enter your choice: \n'1' -> Play All \n'2' -> Play File \n\n");
	scanf("%d",&choice);
	
	switch(choice)
	{
		case 1:
			printf("Options: \n'N' -> Play Next \n'Q' -> quit \n");
			while( (dir = readdir(d)) != NULL )
			{
				if( strcmp(dir->d_name,".") && strcmp(dir->d_name,"..") && strcmp(dir->d_name,"desktop.ini") )
				{
					strcpy(cmd,"\"");
					strcat(cmd, path);
					if(path[strlen(path)-1] != '/')	strcat(cmd, "/");
					strcat(cmd,dir->d_name);
					strcat(cmd,"\"");
					printf("Currently Playing: %s\n",d_name);
					system(cmd);
					scanf("\n");
					scanf("%c",&ch);
					if(ch == 'N')
						continue;
					else if(ch == 'Q')
						break;
				}
			}
			break;
		case 2:
			printf("Enter the filename with extension: ");
			scanf("\n");
			scanf("%[^\n]%*c",fnm);
			strcat(cmd, path);
			if(path[strlen(path)-1] != '/')	strcat(cmd, "/");
			strcat(cmd,fnm);
			strcat(cmd,"\"");
			printf("Currently Playing: %s\n",fnm);
			system(cmd);
			break;
		default:
			printf("Invalid Input\n");
	}		
}

int main( ) 
{ 
	FILE *sfp,*dfp; 
      
    char s_path[STR_SIZE], d_path[STR_SIZE], action[4];
	
	printf("Enter the action to be performed: \n'r' -> read \n'a' -> append \n'w' -> overwrite \n'c' -> copy \n'm' -> move \n'd' -> delete \n'msc' -> play music \n\n");
	scanf("%[^\n]%*c",action);
	
	if( strcmp(action, "msc") )
	{
		printf("Enter the path of the file along with filename and extension(use '/' as separator): \n");	
		scanf("%[^\n]%*c",s_path);
		
		if( strcmp(action,"d")==0 )
		{
			if(remove(s_path))
				printf("Path does not exist.\n");
		}
		else
		{
			if(strcmp(action,"c")==0 || strcmp(action,"m")==0)
				sfp = fopen(s_path, "r");
			else
				sfp = fopen(s_path, action); 
		  
			if (sfp == NULL) 
				printf("Path does not exist.\n"); 
			else
			{
				if(strcmp(action,"a")==0 || strcmp(action,"w")==0)
					write_file(sfp,s_path);
				else if(strcmp(action,"r")==0)
					read_file(sfp);
				else if(strcmp(action,"c")==0 || strcmp(action,"m")==0)
				{
					printf("Enter the destination of the file along with filename and extension(use '/' as separator): ");	
					scanf("%[^\n]%*c",d_path);
					dfp = fopen(d_path,"a");
					if (dfp == NULL) 
						printf("Path does not exist.\n");
					else
					{
						copy(sfp,dfp);
						if(strcmp(action,"m")==0)
							remove(s_path);
					}
				}
			}
		}
	}
	else
		play_music();

    return 0;         
} 
