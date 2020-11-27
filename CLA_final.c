# include <stdio.h> 
# include <stdlib.h>
# include <string.h>
# include <dirent.h>
# include <time.h>
# include <windows.h>
# include <sys/stat.h>
# define STR_SIZE 10000

typedef struct mp_node
{
	struct mp_node *next,*prev;
	char filename[STR_SIZE];
}mp_node;

mp_node *mp_head,*mp_tail;

struct element {
    char note[STR_SIZE];
    struct element *link;
}*newnode,*nm_head,*nm_tail,*temp1;

struct element2{
    char note[STR_SIZE];
    struct element2 *link;
}*newnode2,*nm_head2,*nm_tail2,*temp2;

struct node
{   
    int isfile;
    char data[100];
    struct node * firstChild;
    struct node * nextSibling;
    struct node * next;
    int  hasChild,exists;
};
typedef struct node node;

/*--------file manager & vdrive--------*/
node *root;
node *currentnode;
node* add_node(char *,int);
node* add_child(node *,char *,int);
node* add_sibling(node *,char *,int);
void displayTree(node *,int);
void removeNode(node *,char *);
void delete(node *);
char* addBefore(char*,char*);
void showdir(node *);
char element[100];
/*---------build hierarchy-----------*/
void build_hierarchy();
node* create_node();
void insert_node(node*, node**, node**);
node* go_next(node*);
void display_tree(node *);
/*-----------save hierarchy-----------*/
node* findNextParent_havingChild(node *);
void assignNext(node *);
void saveHierarchy();
/*------------------------------------*/

/*-----------Music Player----------*/
int is_type(char*, char*);
mp_node* create_playlist(DIR*, int);
void play_music();
/*----------------------------------*/

/*-----------Note Maker----------*/
void enqueue(char*);
void dequeue(int);
void print(void);
void modi();
void pining();
void unpin();
void dispin();
void dequeuepin(int);
void enqueuepin(char*);
void modipin();
void note_maker();
void file_manager();
/*----------------------------------*/

int main() 
{ 	
	int choice;
	
	while(1)
	{
		printf("\n+------------------------------------------------------------------+\n");
		printf("|                         W E L C O M E                            |\n");
		printf("+------------------------------------------------------------------+\n");

		printf("1 : Note Maker \n");
		printf("2 : Play Music \n");
		printf("3 : File Manager \n");
		printf("0 : Quit \n");
		printf("\nEnter your choice: ");
		scanf("%d",&choice);
		
		switch(choice)
		{
			case 0:
					exit(0);
			case 1:
					note_maker();
					break;
			case 2:
					play_music();
					break;
			case 3:
					file_manager();
					break;
			default:
					printf("Invalid Choice\n");
		}
	}
	
    return 0;         
}

int is_type(char *fn, char *ext)
{
	/*
	checks if a file 'fn' is of type 'ext'
	*/
	
	int fn_len = strlen(fn), ext_len = strlen(ext), i, j;
	char check[STR_SIZE];
	
	j=0;
	if(fn_len >= ext_len)
		for(i=fn_len-ext_len;i<fn_len;i++)
			check[j++] = fn[i];
	check[j] = '\0';
		
	//printf("%s %s %d\n",ext,check,strcmp(ext,check));
		
	if( strcmp(ext,check) )
		return 0;
	return 1;
}

mp_node* create_playlist(DIR *directory, int shuffle)
{
	/*
	creates a playlist in lexicographic order or shuffle according to the user's input
	*/
	
	struct dirent *dir_entry;
	mp_node *x,*y;
	int ct=0;
	
	if( (mp_tail = (mp_node*)malloc(sizeof(mp_node))) == NULL)
	{
		printf("Memory couldn't be allocated\n"); 
		return NULL;
	}
	mp_head = mp_tail;
		
	while( (dir_entry = readdir(directory)) )
	{
		//the readdir() function returns the directory contents in lexicographic order
		
		if( is_type(dir_entry->d_name, ".mp3") )
		{
			//printf("%s\n",dir_entry->d_name);
			
			strcpy(mp_tail->filename, dir_entry->d_name);
			
			if( (mp_tail->next = (mp_node*)malloc(sizeof(mp_node))) == NULL )
			{
				printf("Memory couldn't be allocated\n"); 
				break;
			}
			mp_tail->next->prev = mp_tail;
			mp_tail = mp_tail->next;
			ct++;
		}
	}
	
	if(ct==0)
	{
		free(mp_head);
		return NULL;
	}
	
	mp_tail = mp_tail->prev;
	free(mp_tail->next);
	mp_tail->next = mp_head;
	mp_head->prev = mp_tail;
	
	if(shuffle == 1)
	{
		x=mp_head;
		y=mp_tail;
		for(int i=0;i<ct;i++)
		{
			//swap x with next mp_node
			x->next->prev = x->prev;
			x->prev = x->next;
			x->next = x->next->next;
			x->prev->next = x;
			x->next->prev = x;
			x->prev->prev->next = x->prev;
			
			//swap y with previous mp_node
			y->prev->next = y->next;
			y->next = y->prev;
			y->prev = y->prev->prev;
			y->next->prev = y;
			y->prev->next = y;
			y->next->next->prev = y->next;
			
			if( abs(time(0)+rand())%2 )//mimics randomness
				x = x->next;
			else
				y = y->prev;						
		}
		return x;
	}
	
	return mp_head;
}

void play_music()
{
	/*
	Plays the a single file or a playlist according to the user's input
	*/	
	
	srand(time(0));
	
	DIR *directory;
	char ch, path[STR_SIZE], filename[STR_SIZE], cmd[STR_SIZE];
	int choice, shuffle,i;
	mp_node *x,*y;
	
	printf("\nEnter the path to the music directory: \n");
	scanf("\n");
	scanf("%[^\n]%*c",path);
	printf("\n");
	
	for(i=0;path[i]!='\0';i++)//to make sure the path has '/' as separator
		if(path[i] == '\\')
			path[i] = '/';
	if(path[i-1] != '/');
		strcat(path, "/");
	
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
			
			if(x == NULL)
			{
				printf("Directory does not contain any mp3 files\n");
				break;
			}
			
			printf("Press: \n'n' to Play Next \n'p' to Play Previous \n'q' to Quit \n\n");
			while(1)
			{
				strcpy(cmd,"\"");
				strcat(cmd, path);
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
			for(x = mp_head;x != mp_tail;)
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
			strcpy(cmd,"\"");
			strcat(cmd, path);
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

void enqueue(char *note)
{
    struct element *ptr;

    ptr = (struct element *) malloc (sizeof (struct element));
    strcpy(ptr -> note, note);
    ptr -> link = NULL;

    if (nm_head == NULL) {
        /*printf("List is empty");*/
        nm_head = nm_tail = ptr;
    }
    else
    {
        ptr->link=nm_head;
        nm_head=ptr;
    }
}

void dequeue(int pos){

    struct element *ptr;
    int i;
    if (nm_head == NULL) {
        printf("NOTEPAD is empty\n");
        return;
    }
    
    if(pos<1)
        return;
        
    if(pos == 1)
    {
        nm_head = nm_head->link;
        return;
    }
        
    ptr=nm_head;
        
    for(i=1;i<pos-1 && ptr->link!=NULL;i++,ptr=ptr->link);
    
    if(ptr->link==NULL)
        return;
    
    if(ptr->link == nm_tail)
        nm_tail = ptr;
        
    ptr->link = ptr->link->link;
}

void print(void)
{
    int i;
    struct element *ptr;
    
    if (!nm_head) {
        fprintf (stderr, "NOTEPAD is empty\n");
        return;
    }
    printf ("Queue: \n");
    ptr=nm_head;
     i = 1;

    while (ptr != NULL){
        
        printf ("%d. %s\n", i, ptr -> note);
        ptr = ptr -> link;
        i++;
    
    }
}

void modi() /*modification of simple note*/
{
     int n,i=0;
     char arr1[STR_SIZE];
     char arr2[STR_SIZE];
     print();
     if(!nm_head)
      return;
     printf("enter the note you want to modify:\n");
     scanf("%d",&n);
     temp1=nm_head;
     while(i<n-1)
      {
          temp1=temp1->link;
          i++;
      }
      strcpy(arr1,temp1->note);
      printf("%s\n",arr1);
      printf("add to note:");
      scanf("\n");
      scanf("%[^\n]%*c",arr2);
      strcat(arr1,arr2);
      dequeue(n);
      enqueue(arr1);
}

void pining() /* pining of note*/
{   
    int n,i=0;
    print();
    if(!nm_head)
    {
        return;
    }
    printf("enter the note you want to pin:");
    scanf("%d",&n);
    temp1=nm_head;
     while(i<n-1)
      {
          temp1=temp1->link;
          i++;
      }
      
    newnode2=(struct element2 *) malloc (sizeof (struct element2));
    strcpy(newnode2->note,temp1->note);
    newnode2->link=NULL;

    if (nm_head2 == NULL) {
        nm_head2 = nm_tail2 = newnode2;
    }
    else
    {
        newnode2->link=nm_head2;
        nm_head2=newnode2;
    }
    dequeue(n);    
}

void unpin() /*to unpin a note.*/
{
    int n,i=0;
    dispin();
    if(!nm_head2)
    {
        return;
    }
    printf("enter the note you want to unpin:");
    scanf("%d",&n);
    temp2=nm_head2;
     while(i<n-1)
      {
          temp2=temp2->link;
          i++;
      }
      newnode=(struct element *) malloc (sizeof (struct element));
      strcpy(newnode->note,temp2->note);
      newnode->link=NULL;
      if(nm_head == NULL)
      {
          nm_head=nm_tail=newnode;
      }
      else
      {
      nm_tail->link=newnode;
      nm_tail=newnode;
      }
      dequeuepin(n);
}

void dispin() /*to Display pinned notes*/
{
    
     if (!nm_head2) {
        printf ( "OOPS! no pinned note\n");
        return;
    }
    if(nm_head2!=NULL)
    {
        int i;
        printf ("Pinned: \n");
        temp2=nm_head2;
        i = 1;
        
        while (temp2 != NULL)
        {
        
          printf ("%d. %s\n", i, temp2 -> note);
          temp2 = temp2 -> link;
          i++;
        }
        
    }

}

void dequeuepin(int pos) /*to delete pinned note*/
{
    int i;
    /*struct element *temp2;*/
    if (nm_head2 == NULL) {
        printf("OOPS ! No pinned notes.\n");
        return;
    }
    
    if(pos<1)
        return;
        
    if(pos == 1)
    {
        nm_head2 = nm_head2->link;
        return;
    }
        
    temp2=nm_head2;
     
    for(i=1;i<pos-1 && temp2->link!=NULL;i++,temp2=temp2->link);
    
    if(temp2->link==NULL)
        return;
    
    if(temp2->link == nm_tail2)
        nm_tail2 = temp2;
        
    temp2->link = temp2->link->link;
}

void enqueuepin(char *note) /*add note(after modification) to pinned note*/
{
    
    temp2=(struct element2 *) malloc (sizeof (struct element2));
    strcpy(temp2 -> note, note);
    temp2 -> link = NULL;

    if (nm_head2 == NULL) {
        /*printf("List is empty");*/
        nm_head2 = nm_tail2 = temp2;
    }
    else
    {
        temp2->link=nm_head2;
        nm_head2=temp2;
    }
}

void modipin()
{
    int n,i=0;
     char arr1[STR_SIZE];
     char arr2[STR_SIZE];
     dispin();
     if(!nm_head2)
     {
         return;
     }
     printf("enter the note you want to modify\n");
     scanf("%d",&n);
     temp2=nm_head2;
     while(i<n-1)
      {
          temp2=temp2->link;
          i++;
      }
      strcpy(arr1,temp2->note);
      printf("%s\n",arr1);
      printf("Add to note:");
      scanf("\n");
      scanf("%[^\n]%*c",arr2);
      strcat(arr1,arr2);
      dequeuepin(n);
      enqueuepin(arr1);
      printf("After modification your list is:--\n");
      print();
}

void note_maker()
{
	char arr[STR_SIZE];
    int num,n;
    nm_head = NULL;
    nm_tail = nm_head;
    while(1)
	{
		printf("\nWelcome to Note Maker.\n\n");
		printf("1:  Add a new Note\n");
		printf("2:  Delete a Note\n");
		printf("3:  Display Note\n");
		printf("4:  Modify a note.\n");
		printf("5:  Pining\n");
		printf("6:  How to use this NOTE MAKER!\n");
		printf("0:  Quit\n\nEnter your choice:");
		scanf("%d",&num);
		switch(num)
		{
			case 1: 
					printf("Note:");
					scanf("\n");
					scanf("%[^\n]%*c",arr);
					/*len=strlen(arr);
					if(arr[len-1]=='\n'){
					arr[len-1]='\0';
					}*/
					enqueue(arr);
					break;
			case 2: 
					print();
					if(!nm_head)
					  break;
					printf("\nEnter the note number to be deleted ");
					scanf("%d",&num);
					dequeue(num);
					break;
			case 3: 
					if(nm_head2!=NULL)
					{
						dispin();
						printf("\n***********************\n\n");
						print();
					}
					else
					{
						print();
					}
					break;
					
			case 4:
					modi();
					break;
					
			case 5:
					printf("\nEnter what you want to:\n");
					printf("1:  Pin a note\n");
					printf("2:  Unpin a note\n");
					printf("3:  Modify pinned notes\n");
					scanf("%d",&n);
					if(n==1)
					{
						pining();
					}
					else if(n==2)
					{
						unpin();
					}
					else if(n==3)
					{
						modipin();
					}
					else
					{
					 printf("Choose a valid option\n");
						
					}
					break;
			case 0: 
					printf("Thanks for using Note Maker.\n");
					return;
			default: printf("Choose a Correct Choice\n");
					break;
		}
    }
}

node * add_node(char * data,int markfile)
{
    node *temp = (node*)malloc(sizeof(node));

    if ( temp ) 
	{
        temp->nextSibling = NULL;
        temp->firstChild = NULL;
        temp->next= NULL;
        strcpy(temp->data,data);
        temp->isfile=markfile;
        temp->hasChild=0;
        temp->exists = 0;
    }

    return temp;
}

node * add_child(node *n, char * data,int markfile)
{
    node *temp;
    if(n==NULL)
        return NULL;
    temp = add_node(data,markfile);
    n->firstChild=temp;
    return (n=n->firstChild);

}

node * add_sibling(node *n, char * data,int markfile)
{
    /*refer using first child*/
    node *temp;
    if(n==NULL)
        return NULL;
    temp = add_node(data,markfile);
    n->nextSibling=temp;
    return (n=n->nextSibling);
}

void displayTree(node * current,int depth)
{
    int i;
    node *j;
    if(current!=NULL)
    {
        current=current->firstChild;
        if(current!=NULL)
        {
            j=current;
            while(j!=NULL)
            {
                for(i=0;i<depth;i++)
                    printf("\t");
                printf(" %s\n",j->data);
                displayTree(j,depth+1);
                j=j->nextSibling;
            }
        }
    }
    return;
}

void showDir(node * current)
{
    if(current->firstChild)
    {
        current =current->firstChild;
        printf("%s\n",current->data);
        while(current->nextSibling)
        {
            current=current->nextSibling;
            printf("%s\n",current->data);
        }
    }
    else
        printf("This directory is empty..\n");
    return;

}

int isChildPresent(node * current,char *childname)
{
    if(current->firstChild)
    {
        if(strcmp(current->firstChild->data,childname)==0)
            {
                currentnode=current->firstChild;
                return (currentnode->isfile);
            }
        else
        {
            current =current->firstChild;
            while(current->nextSibling)
            {
                if(strcmp(current->nextSibling->data,childname)==0)
                {
                    currentnode=current->nextSibling;
                    return (currentnode->isfile);
                    /*1: isfile; 0: not isfile; -1:child not present */
                }
                else
                    current=current->nextSibling;
            }
        }
    }
    return -1;
}

struct StackNode
{
    struct node * nodes;
    struct StackNode *snodenext;
};
typedef struct StackNode StackNode;

StackNode* pushNode(StackNode* head, node * n)
{
    StackNode* temp =(struct StackNode*)malloc(sizeof(StackNode));
    if(temp==NULL)
    {
        exit(0);
    }
    temp->nodes=n;
    temp->snodenext = head;
    head = temp;
    return head;
}

StackNode* popNode(StackNode* head)
{
    StackNode* temp =head;
    head = head-> snodenext;
    free(temp);
    return head;
}

struct Stack
{
    char sdata[100];
    struct Stack *snext;
};
typedef struct Stack Stack;

Stack* push(Stack* head, char x[])
{
    Stack* temp =(struct Stack*)malloc(sizeof(Stack));
    if(temp==NULL)
    {
        exit(0);
    }
    strcpy(temp->sdata,x);
    temp->snext = head;
    head = temp;
    return head;
}

Stack* pop(Stack* head, char element[])
{
    Stack* temp =head;
    strcpy(element,head->sdata);
    head = head-> snext;
    free(temp);
    return head;
}
int empty(Stack* head)
{
    return head == NULL ? 1 : 0;
}

void sdisplay(Stack* head)
{
    char str[STR_SIZE]="";
    Stack *current;
    current = head;
    if(current!= NULL)
    {
        while (current!= NULL)
        {
            strcpy(str,addBefore(str,current->sdata));
            current = current->snext;
        }
        
        printf("%s",str);
    }
    else
    {
        printf("The Stack is empty\n");
    }
 
}

 char* addBefore(char* s1,char* s2)
{
    char *s3=malloc(STR_SIZE);
    int i,j;
    j=0;
    for(i=0;s2[i]!='\0';i++)
    {
        s3[i]=s2[i];
    }
    while(s1[j]!='\0')
    {
        s3[i]=s1[j];
        j=j+1;
        i=i+1;
    }
    s3[i]='\0';
    return s3;

}

void removeNode(node *current,char *nodeName)
{
    node * temp;
    if(isChildPresent(current,nodeName)!=-1)
    {
        if (strcmp(current->firstChild->data,nodeName)==0)
        {

            if(current->firstChild->firstChild!=NULL)
                printf("Directory is not empty..\n");
            else{
            temp = current->firstChild;
            current->firstChild = temp->nextSibling;
            free(temp);
            printf("Deleted successfully..\n");
            }
        }
        else
        {
            current = current->firstChild;
            while(current!=NULL)
            {
                if(strcmp(current->nextSibling->data,nodeName)==0)
                {
                   if(current->nextSibling->firstChild!=NULL)
                    {printf("Directory is not empty..\n");break;}
                   else
                    {delete(current);printf("Deleted successfully..\n");break;}
                }
                current = current->nextSibling;
            }
        }
    }
    else
        printf("%s does not exist..\n", nodeName);
}

void delete(node *before_del)
{
    node *temp;
    temp = before_del->nextSibling;
    before_del->nextSibling = temp->nextSibling;
    free(temp);
}


/*-------------------Restore Hierarchy---------------------*/

void build_hierarchy()
{	
	FILE *fp = fopen("hierarchy.txt", "r");
	char c;
	node *first_parent, *current_parent, *current, *nd, *temp, *prev;
	int i;
	
	if((nd=create_node()) == NULL)
		return;
	
	root = NULL;
	first_parent = NULL;
	current_parent = NULL;
	current = NULL;
	prev = NULL;
	i=0;
	while((c=fgetc(fp))!=EOF)
	{						
		switch(c)
		{			
			case ',':
			case ';':
					nd->data[i] = '\0';		
					insert_node(nd, &current_parent, &current);
					
					if(i)
					{
					    current->exists = 1;
						if(current!=root)
					    	current_parent->hasChild = 1;
					    if(prev)
					    {
					        prev->next = current;
					        prev = NULL;
					    }
					}
										
					if(c==';' && current_parent)
					{
					    temp = current_parent;
						while((temp=go_next(temp)) && temp->exists == 0);
						
						if(temp!=current_parent)
						{
						    current_parent = temp;
    						prev = current;
        					current = current_parent;
						}
					}
					
					i=0;
					if((nd=create_node()) == NULL)
						return;
					break;
			case '\n':
					while(first_parent && first_parent->hasChild == 0)
					{
					    first_parent = go_next(first_parent);
					}
					
					if(first_parent == NULL)
						first_parent = root;
					else
					{
					    first_parent = first_parent->firstChild;
						current_parent =  first_parent;
						current = current_parent;
					}
					i=0;
					break;
			default:
					nd->data[i++] = c;
		}
	}
	if(root == NULL)
	{
		strcpy(nd->data, "root");
		root = nd;
	}
	else
		free(nd);
	fclose(fp);
}

node* create_node()
{
	node *nd = (node*)malloc(sizeof(node));
	
	if(nd == NULL)
		printf("Memory could not be allocated for node\n");
	else
	{
		nd->firstChild = NULL;
		nd->nextSibling = NULL;	
		nd->next = NULL;
		nd->exists = 0;
		nd->hasChild = 0;
        nd->isfile = 0;
	}
	
	return nd;
}

void insert_node(node *nd, node **current_parent, node **current)
{
	if(root == NULL)
	{
		root = nd;
		(*current_parent) = root;
		(*current) = root;
	}
	else
	{
		if((*current) == (*current_parent))
		{
			(*current_parent)->firstChild = nd;
			(*current) = (*current_parent)->firstChild;
		}
		else
		{
			(*current)->nextSibling = nd;
			(*current) = (*current)->nextSibling;
		}
	}
}

node* go_next(node *x)
{
    if(x->nextSibling)
		x = x->nextSibling;						
	else if(x->next)
		x = x->next->firstChild;
		
	return x;
}

void display_tree(node *current)
{
	node *x;
    if(current == NULL)
		return;
	
	printf("%s\n",current->data);
	
	x = current->firstChild;
	while(x)
	{
		printf("%s ",x->data);
		x = x->nextSibling;
	}
	printf("\n\n");
	
	display_tree(current->firstChild);
	display_tree(current->nextSibling);
}
/*--------------------Save hierarchy------------------ */

void assignNext(node *first_parent)
{
	node *current_parent, *current, *temp;
	int next_first_parent_chosen = 0;
	
	if(!first_parent || !first_parent->firstChild)
		return;
	
	current_parent = first_parent;
	current = current_parent->firstChild;
	
	while(current->next)
	{
		while(current->nextSibling)
		{
			current = current->nextSibling;
			if( !next_first_parent_chosen && current->hasChild )
			{
				first_parent = current;
				next_first_parent_chosen  = 1;
			}
		}
		
		temp = findNextParent_havingChild(current_parent);
		
		if( temp == current_parent || !temp->hasChild )
		    break;
		    
		current->next = temp->firstChild;
	}
	
	if( next_first_parent_chosen )
		assignNext(first_parent);
}

node* findNextParent_havingChild(node *current)
{
    if( current == NULL )
        return NULL;
    
	if( current->nextSibling )
		current = current->nextSibling;
	else if( current->next )
	    current = current->next;
	else
	    return current;
	
	while( !current->hasChild )
	{
		if( current->nextSibling )
			current = current->nextSibling;
    	else if( current->next )
    	     current = current->next;
		else
		    break;
	}
	
    return current;
}

void saveHierarchy()
{
    FILE *fp = fopen("hierarchy.txt","w");
    char next_level[STR_SIZE] = "";
    int next_first_parent_chosen = 0;
    node *first_parent=root, *current_parent=root, *current=root;
    
	fputs("root;\n",fp);
	
    while(current_parent)
    {
		if(!next_first_parent_chosen && current_parent->hasChild)
		{
			first_parent = current_parent;
			next_first_parent_chosen = 1;
		}
		
		current = current_parent->firstChild;
		
		while(current)
		{
			strcat(next_level, current->data);			
			if(current->nextSibling)
				strcat(next_level,",");
			current = current->nextSibling;
		}
		
		strcat(next_level,";");
		
		if(current_parent->nextSibling)
			current_parent = current_parent->nextSibling;
		else if(current_parent->next)
			current_parent = current_parent->next;
		else
		{
			if(!next_first_parent_chosen)
				break;
			
			if ( (current_parent = first_parent->firstChild) )
			{
			    strcat(next_level,"\n");
    			fputs(next_level,fp);
    			strcpy(next_level,"");
			}
			next_first_parent_chosen = 0;
		}
	}	
	fclose(fp);
}
void createDumpFolder()
{
    struct stat st;
    char *dirName = "that.dumps";
    if(stat(dirName,&st)==-1)
    {
        if(mkdir(dirName) != -1)
        {
            printf("Created %s successfully.\n",dirName);

        }
        else
        {   
            perror("mkdir: ");
            printf("Please check whether '%s' file exists.\n", dirName);
        }
    }
}

/*---------------------Virtal Drive-------------------------*/

void vDrive(struct node *n)
{
    char ch[100];
    char ch1[100];
    struct Stack* head = NULL;
    struct StackNode* headNode = NULL;

    printf("\n+----------------------THE VIRTUAL DRIVE--------------------------+\n");
    printf("|   Commands for virtual drive:-                                  |\n");
    printf("|   cd      : Change directory                                    |\n");
    printf("|   cd ..   : Previous directory                                  |\n");
    printf("|   mkdir   : Make directory                                      |\n");
    printf("|   mkfile  : Make new file                                       |\n");
    printf("|   open    : Open a file in a directory                          |\n");
    printf("|   remove  : Remove a file or a folder                           |\n");
    printf("|   ls -a   : Display a directory's files and subdirectories      |\n");
    printf("|   ls -all : Display tree of folders of virtual drive            |\n");
    printf("|   quit -v : quit                                                |\n");
    printf("+-----------------------------------------------------------------+\n\n");
    strcpy(element,"V:\\");
    head = push(head,element);
    strcpy(element,">");
    head = push(head,element);
    sdisplay(head);
    currentnode =n;/*n = root*/
    headNode = pushNode(headNode,currentnode);
    while(1)
    {
        
        scanf("%s %s",ch,ch1);
        if(strcmp(ch,"cd")==0 && strcmp(ch1,"..")!=0)
         {   
           currentnode = headNode->nodes;
           if(isChildPresent(currentnode,ch1)==0)
           {
            head = pop(head,element);

            strcpy(element,ch1);
            head = push(head,element);

            strcpy(element,"\\");
            head = push(head,element);

            strcpy(element,">");
            head = push(head,element);
            sdisplay(head);
            headNode = pushNode(headNode,currentnode);
            currentnode = headNode->nodes;
            }
            else
             {  printf("No such directory present\n");
                sdisplay(head);
                currentnode = headNode->nodes;
             }
         }
        else if(strcmp(ch,"cd")==0 && strcmp(ch1,"..")==0)
        {
            head = pop(head,element);
            head = pop(head,element);
            head = pop(head,element);


            strcpy(element,">");
            head = push(head,element);
            sdisplay(head);
            headNode =popNode(headNode);
            currentnode = headNode->nodes;

        }
        else if(strcmp(ch,"ls")==0 && strcmp(ch1,"-a")==0)
        {
             currentnode = headNode->nodes; 
             printf("\nDirectory of ");
             sdisplay(head);
             printf("\n");
             showDir(currentnode);
             printf("\n");
             sdisplay(head);
        }
        else if(strcmp(ch,"ls")==0 && strcmp(ch1,"-all")==0)
        {
             printf("\nFolder listing for drive V:\n");
             displayTree(root,0);
             printf("\n");
             sdisplay(head);
        }
        else if(strcmp(ch,"mkdir")==0 && strcmp(ch1,"`")!=0)
        {

                if(currentnode->firstChild==NULL)
                {
                    currentnode = add_child(currentnode,ch1,0);
                    /*0 represents not a file*/
                    /*curent node changed so need to
                    point the currentnode to parent*/
                    currentnode = headNode->nodes;
                    currentnode->hasChild=1;
                    printf("Folder successfully created..\n");

                }
                else if(isChildPresent(currentnode,ch1)==-1)
                {
                    currentnode=currentnode->firstChild;
                    while(currentnode->nextSibling)
                    {
                        currentnode=currentnode->nextSibling;
                    }
                    currentnode = add_sibling(currentnode,ch1,0);
                    currentnode = headNode->nodes;
                    printf("Folder successfully created..\n");
                }

                else{
                    currentnode = headNode->nodes;
                    printf("Folder name with %s already exists\n",ch1);
                }
            sdisplay(head);
        }
        
        else if(strcmp(ch,"mkfile")==0 && strcmp(ch1,"`")!=0)
        {
            FILE *fp;
            char *prefix="that.dumps";
            char cmd[STR_SIZE] ="notepad ";
            char filePath[STR_SIZE];
            if(currentnode->firstChild==NULL)
                {
                    currentnode = add_child(currentnode,ch1,1);
                    currentnode = headNode->nodes;
                   
                    snprintf(filePath,STR_SIZE+1,"%s/%s",prefix,ch1);
                    fp=fopen(filePath,"w+");
                    fclose(fp);
                    strcat(cmd,filePath);
                    system(cmd);
                    printf("File successfully created..\n");

                }
                else if(isChildPresent(currentnode,ch1)==-1)
                {
                    /*isChildPresent(currentnode,ch1)==-1 represents if no such
                    named file exists*/
                    currentnode=currentnode->firstChild;
                    while(currentnode->nextSibling)
                    {
                        currentnode=currentnode->nextSibling;
                    }
                    currentnode = add_sibling(currentnode,ch1,1);
                    currentnode = headNode->nodes;

                    snprintf(filePath,STR_SIZE+1,"%s/%s",prefix,ch1);
                    fp=fopen(filePath,"w+");
                    fclose(fp);
                    strcat(cmd,filePath);
                    system(cmd);
                    printf("File successfully created..\n");
                }

                else{
                     currentnode = headNode->nodes;
                    printf("File name with %s already exists\n",ch1);
                }
            sdisplay(head);
        }
        else if(strcmp(ch,"open")==0 && strcmp(ch1,"`")!=0)
        {
            char *prefix="that.dumps";
            char cmd[STR_SIZE] ="notepad ";
            char filePath[STR_SIZE];
            if(strcmp(ch1,currentnode->firstChild->data)==0)
            {
                snprintf(filePath,STR_SIZE,"%s/%s",prefix,ch1);
                strcat(cmd,filePath);
                system(cmd);
            }
            else if(isChildPresent(currentnode,ch1)==1)
            {
                /*currentnode = headNode->nodes; 
                currentnode = currentnode->firstChild->nextSibling;
                while(currentnode!=NULL)
                {
                    if(strcmp(ch1,currentnode->data)==0)
                    {
                        snprintf(filePath,STR_SIZE,"%s/%s",prefix,ch1);
                        strcat(cmd,filePath);
                        system(cmd);
                    }
                    currentnode=currentnode->nextSibling;
                }*/
                snprintf(filePath,STR_SIZE,"%s/%s",prefix,ch1);
                strcat(cmd,filePath);
                system(cmd);
            }
            else
            {
                printf("Filename with %s does not exist\n",ch1);
            }
            currentnode = headNode->nodes;
            sdisplay(head);   
        }
        else if(strcmp(ch,"remove")==0 && strcmp(ch1,"`")!=0)
        {
            removeNode(currentnode,ch1);
            currentnode = headNode->nodes;
            sdisplay(head);  
        }
        else if(strcmp(ch,"quit")==0 && strcmp(ch1,"-v")==0)
        {
            break;
        }
        else 
        {
            printf("Not regocnized as a command\n");
            sdisplay(head);
        }        
    }
}

void file_manager()
{
	int ch;
    node *current;
    createDumpFolder();

    /* The Generic Tree's root */
    root=add_node("root",0);

    current=root;
    current->hasChild=1;
    currentnode=root;

    printf("\n+------------------------------------------------------------------+\n");
    printf("|                         FILE MANAGER                             |\n");
    printf("+------------------------------------------------------------------+\n");

    while(1)
	{    
		printf("\n1       : Enter the Virtual Drive\n");
	    printf("2       : Display Tree\n");
		printf("3       : Restore Hierarchy\n");
        printf("4       : Save Hierarchy\n");
		printf("5       : Exit\n");
		printf("\nEnter your choice : ");
		scanf("%d",&ch);

		switch(ch)
		{
			case 1: 
					vDrive(root);
					break;
			case 2: 
					displayTree(root,0);
					break;
			case 3:                 
                    build_hierarchy();
                    /*display_tree(root);*/
                    displayTree(root,0);
                    printf("Build successfull!\n");
					break;
			case 4: 
					assignNext(root);
					saveHierarchy();
                    printf("Saved successfully!\n");
					break;
			case 5: 
					return;
			default: 
					printf("\nNot regocnized as a command.\n");
		}
    }
}