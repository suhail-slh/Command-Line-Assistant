# include <stdio.h> 
# include <string.h> 
# include <windows.h>
# include <dirent.h>
# include <sys/stat.h>
# include <unistd.h>
# include <time.h>
# define STR_SIZE 10000

typedef struct node
{
	struct node *next,*prev;
	char filename[STR_SIZE];
}node;

node *head,*tail;

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

node* create_playlist(DIR *directory, int shuffle)
{
	struct dirent *dir_entry;
	node *x,*y;
	int ct=0;
	
	tail = (node*)malloc(sizeof(node)); 
	head = tail;
		
	while( (dir_entry = readdir(directory)) != NULL )
	{
		if( strcmp(dir_entry->d_name,".") && strcmp(dir_entry->d_name,"..") && strcmp(dir_entry->d_name,"desktop.ini") )
		{
			strcpy(tail->filename, dir_entry->d_name);
			
			tail->next = (node*)malloc(sizeof(node));
			tail->next->prev = tail;
			tail = tail->next;
			ct++;
		}
	}
	
	tail = tail->prev;
	tail->next = head;
	head->prev = tail; 
	
	if(shuffle == 1)
	{
		x=head;
		y=tail;
		for(int i=0;i<ct;i++)
		{
			x->next->prev = x->prev;
			x->prev = x->next;
			x->next = x->next->next;
			x->prev->next = x;
			x->next->prev = x;
			x->prev->prev->next = x->prev;
			
			y->prev->next = y->next;
			y->next = y->prev;
			y->prev = y->prev->prev;
			y->next->prev = y;
			y->prev->next = y;
			y->next->next->prev = y->next;
			
			if( abs(time(0)+rand())%2 )
				x = x->next;
			else
				y = y->prev;						
		}
		return x;
	}
	
	return head;
}

void play_music()
{
	DIR *directory;
	char ch, path[STR_SIZE], filename[STR_SIZE] = "", cmd[STR_SIZE] = "\"";
	int choice, shuffle;
	node *x,*y;
	
	printf("Enter the path to the music directory: \n");
	scanf("\n");
	scanf("%[^\n]%*c",path);
	printf("\n");
	
	for(int i=0;path[i]!='\0';i++)
		if(path[i] == '\\')
			path[i] = '/';
	
	directory = opendir(path);
	
	if(directory==NULL)
	{
		printf("Path does not exist.\n"); 
		return;
	}
	
	printf("Press: \n1 to Play All \n2 to Play File \n\n");
	scanf("%d",&choice);
	printf("\n");
	
	switch(choice)
	{
		case 1:
			printf("Press: \n1 to Shuffle Play \n0 to Play In Order\n\n");
			scanf("%d",&shuffle);
			printf("\n");
			
			if(shuffle!=1 && shuffle!=0)
			{
				printf("Invalid Input\n");
				break;
			}
			
			x = create_playlist(directory, shuffle);
			printf("Press: \n'n' to Play Next \n'p' to Play Previous \n'q' to Quit \n\n");
			while( x != NULL )
			{
				strcpy(cmd,"\"");
				strcat(cmd, path);
				if(path[strlen(path)-1] != '/')	strcat(cmd, "/");
				strcat(cmd,x->filename);
				strcat(cmd,"\"");
				printf("Previous: %s\n",x->prev->filename);
				printf("Currently Playing: %s\n",x->filename);
				printf("Next: %s\n\n",x->next->filename);
				system(cmd);
				ch = ' ';
				while(ch != 'n' && ch != 'p' && ch != 'q')
				{
					scanf("\n%c",&ch);
				}
				if(ch == 'q')
					break;
				else if(ch == 'n')
					x = x->next;
				else if(ch == 'p')
					x = x->prev;
			}
			for(x = head;x != tail;)
			{
				y = x->next;
				free(x);
				x = y;
			}
			free(x);
			break;
		case 2:
			printf("Enter the filename with extension: \n");
			scanf("\n");
			scanf("%[^\n]%*c",filename);
			printf("\n");
			strcat(cmd, path);
			if(path[strlen(path)-1] != '/')	strcat(cmd, "/");
			strcat(cmd,filename);
			strcat(cmd,"\"");
			printf("Currently Playing: %s\n",filename);
			system(cmd);
			break;
		default:
			printf("Invalid Input\n");
	}		
	closedir(directory);
}

int main( ) 
{ 
	FILE *sfp,*dfp; 
      
    char s_path[STR_SIZE], d_path[STR_SIZE];
	
	int action,action2;
	
	srand(time(0));
	
	while(1)
	{
		printf("\nPress: \n1 to Read \n2 to Append \n3 to Overwrite \n4 to Copy \n5 to Move \n6 to Delete \n7 to Play Music \n0 to Quit \n\n");
		scanf("%d",&action);
		
		if(!action)
			break;
		
		switch(action)
		{
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
					printf("Enter the path of the file along with filename and extension: \n");	
					scanf("\n");
					scanf("%[^\n]%*c",s_path);
					printf("\n");
					
					for(int i=0;s_path[i]!='\0';i++)
						if(s_path[i] == '\\')
							s_path[i] = '/';
						
					switch(action)
					{
						case 1:
						case 4:
						case 5:
								sfp = fopen(s_path, "r");
								break;
						case 2:
								sfp = fopen(s_path, "a");
								break;
						case 3:
								sfp = fopen(s_path, "w");
								break;
						case 6:
								if(remove(s_path))
									printf("Path does not exist.\n");
								break;
					}
					if (sfp == NULL) 
						printf("Path does not exist.\n"); 
					else
					{
						switch(action)
						{
							case 1:
									read_file(sfp);
									break;
							case 2:
							case 3:
									write_file(sfp,s_path);
									break;
							case 4:
							case 5:
									printf("Enter the destination of the file along with filename and extension: \n");	
									scanf("\n");
									scanf("%[^\n]%*c",d_path);
									printf("\n");
									
									for(int i=0;d_path[i]!='\0';i++)
										if(d_path[i] == '\\')
											d_path[i] = '/';
									
									dfp = fopen(d_path,"r");//to check if file exists already
									
									if (dfp == NULL) 
									{
										fclose(dfp);
										dfp = fopen(d_path,"w");
									}
									else
									{
										fclose(dfp);
										printf("File already exists at the destination. Press: \n1 to Overwrite \n2 to Append \n\n");
										scanf("%d",&action2);
										
										if(action2 == 1)
											dfp = fopen(d_path,"w");
										else if(action2 == 2)
											dfp = fopen(d_path,"a");
										else
										{
											printf("Invalid Input\n");
											continue;
										}
									}
									copy(sfp,dfp);
									if(action==5)
										remove(s_path);
									break;
						}
					}
					break;
			case 7:
					play_music();
					break;
			default:
					printf("Invalid Input\n");
		}
	}
    return 0;         
} 