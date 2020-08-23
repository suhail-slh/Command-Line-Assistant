# include <stdio.h> 
# include <string.h> 
# include <windows.h>
# define STR_SIZE 10000

void read(FILE *fp)
{
	char read[STR_SIZE];
	while(fgets(read,STR_SIZE,fp)!=NULL)
		printf("%s",read);
	fclose(fp); 
}

void write(FILE *fp, char *path)
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

void delete(char *path)
{
	if(remove(path))
		printf("Path does not exist.");
}

int main( ) 
{ 
	FILE *sfp,*dfp; 
      
    char s_path[STR_SIZE], d_path[STR_SIZE], action[2];
	
	printf("Enter the path of the file along with filename(use '/' as separator): ");	
	scanf("%[^\n]%*c",s_path);
	printf("Enter the action to be performed 'r'->read, 'a'->append, 'w'->overwrite, 'c'->copy, 'm'->move, 'd'->delete: ");
	scanf("%[^\n]%*c",action);
	
	if(strcmp(action,"d"))
	{
		if(strcmp(action,"c")==0 || strcmp(action,"m")==0)
			sfp = fopen(s_path, "r");
		else
			sfp = fopen(s_path, action); 
      
		if (sfp == NULL) 
			printf("Path does not exist."); 
		else
		{
			if(strcmp(action,"a")==0 || strcmp(action,"w")==0)
				write(sfp,s_path);
			else if(strcmp(action,"r")==0)
				read(sfp);
			else if(strcmp(action,"c")==0 || strcmp(action,"m")==0)
			{
				printf("Enter the destination of the file along with filename(use '/' as separator): ");	
				scanf("%[^\n]%*c",d_path);
				dfp = fopen(d_path,"a");
				if (dfp == NULL) 
					printf("Path does not exist.");
				else
				{
					copy(sfp,dfp);
					if(strcmp(action,"m")==0)
						delete(s_path);
				}
			}
		} 
	}
	else
		delete(s_path);

    return 0;         
} 