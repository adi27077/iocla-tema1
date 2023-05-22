#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

typedef struct Dir{
	char *name;
	struct Dir* parent;
	struct File* head_children_files;
	struct Dir* head_children_dirs;
	struct Dir* next;
} Dir;

typedef struct File {
	char *name;
	struct Dir* parent;
	struct File* next;
} File;

Dir *initDir (char *name)
{
	Dir *aux = (Dir *)malloc(sizeof(Dir));
	aux->name = (char *)malloc(sizeof(char) * MAX_INPUT_LINE_SIZE);
	strcpy(aux->name, name);
	aux->parent = NULL;
	aux->head_children_dirs = NULL;
	aux->head_children_files = NULL;
	aux->next = NULL;

	return aux;
}

File *initFile (char *name)
{
	File *aux = (File *)malloc(sizeof(File));
	aux->name = (char *)malloc(sizeof(char) * MAX_INPUT_LINE_SIZE);
	strcpy(aux->name, name);
	aux->parent = NULL;
	aux->next = NULL;

	return aux;
}

void freeFile (File **file)
{
	if(*file == NULL)
		return;

	if((*file)->next != NULL)
	{
		freeFile(&((*file)->next));
	}

	free((*file)->name);
	free(*file);
	*file = NULL;
}

void freeDir (Dir **parent)
{
	if(*parent == NULL)
		return;

	if((*parent)->head_children_dirs != NULL)
	{
		freeDir(&((*parent)->head_children_dirs));
	}

	if((*parent)->head_children_files != NULL)
	{
		freeFile(&((*parent)->head_children_files));
	}

	if((*parent)->next != NULL)
	{
		freeDir(&((*parent)->next));
	}

	free((*parent)->name);
	free(*parent);
	*parent = NULL;
}

void touch (Dir* parent, char* name) 
{
	if(parent->head_children_files == NULL)
	{
		parent->head_children_files = initFile(name);
		parent->head_children_files->parent = parent;
	}
	else
	{
		File *aux = parent->head_children_files;
		//adaugare fisier nou la sfarsitul listei
		while(aux != NULL)
		{
			if(strcmp(name, aux->name) == 0)
			{
				printf("File already exists\n");
				return;
			}
			if(aux->next == NULL)
			{
				aux->next = initFile(name);
				aux->next->parent = parent;
				return;
			}
			aux = aux->next;	
		}
	}
}

void mkdir (Dir* parent, char* name)
{
	if(parent->head_children_dirs == NULL)
	{
		parent->head_children_dirs = initDir(name);
		parent->head_children_dirs->parent = parent;
	}
	else
	{
		Dir *aux = parent->head_children_dirs;
		//adaugare director nou la sfarsitul listei
		while(aux != NULL)
		{
			if(strcmp(name, aux->name) == 0)
			{
				printf("Directory already exists\n");
				return;
			}
			if(aux->next == NULL)
			{
				aux->next = initDir(name);
				aux->next->parent = parent;
				return;
			}
			aux = aux->next;	
		}
	}
}

void ls (Dir* parent) 
{
	//afisare directoare
	if(parent->head_children_dirs != NULL)
	{
		Dir *aux = parent->head_children_dirs;
		while(aux != NULL)
		{
			printf("%s\n", aux->name);
			aux = aux->next;
		}
	}

	//afiare fisiere
	if(parent->head_children_files != NULL)
	{
		File *aux = parent->head_children_files;
		while(aux != NULL)
		{
			printf("%s\n", aux->name);
			aux = aux->next;
		}
	}
}

void rm (Dir* parent, char* name) 
{
	//cazul in care nu exista fisiere
	if(parent->head_children_files == NULL)
	{
		printf("Could not find the file\n");
		return;
	}
	
	//cazul in care fisierul de eliminat e primul din lista
	if(strcmp(parent->head_children_files->name, name) == 0)
	{
		File *aux = parent->head_children_files;
		parent->head_children_files = parent->head_children_files->next;
		free(aux->name);
		free(aux);
		return;
	}
	
	File *prev = parent->head_children_files;
	File *current = parent->head_children_files->next;

	//cazul in care fisierul de eliminat nu e primul
	while(current != NULL)
	{
		if(strcmp(current->name, name) == 0)
		{
			File *aux = current;
			prev->next = current->next;
			free(aux->name);
			free(aux);
			return;
		}
		prev = prev->next;
		current = current->next;
	}

	//cazul in care fisierul nu exista
	printf("Could not find the file\n");
}

void rmdir (Dir* parent, char* name) 
{
	//cazul in care nu exista directoare
	if(parent->head_children_dirs == NULL)
	{
		printf("Could not find the dir\n");
		return;
	}

	//cazul in care directorul de eliminat e primul din lista
	if(strcmp(parent->head_children_dirs->name, name) == 0)
	{
		Dir *aux = parent->head_children_dirs;
		parent->head_children_dirs = parent->head_children_dirs->next;

		if(aux->head_children_dirs != NULL)
			freeDir(&(aux->head_children_dirs));
		if(aux->head_children_files)
			freeFile(&(aux->head_children_files));
		
		free(aux->name);
		free(aux);
		return;
	}

	Dir *prev = parent->head_children_dirs;
	Dir *current = parent->head_children_dirs->next;

	//cazul in care directorul de eliminat nu e primul
	while(current != NULL)
	{
		if(strcmp(current->name, name) == 0)
		{
			Dir *aux = current;
			prev->next = current->next;

			if(aux->head_children_dirs != NULL)
				freeDir(&(aux->head_children_dirs));
			if(aux->head_children_files)
				freeFile(&(aux->head_children_files));

			free(aux->name);
			free(aux);
			return;
		}
		prev = prev->next;
		current = current->next;
	}

	//cazul in care directorul nu exista
	printf("Could not find the dir\n");
}

void cd(Dir** target, char *name) 
{
	//cazul ".."
	if(strcmp(name, "..") == 0)
	{
		//cazul /home
		if((*target)->parent == NULL)
			return;
		
		*target = (*target)->parent;
		return;
	}
	
	//cazul in care nu exista niciun director
	if((*target)->head_children_dirs == NULL)
	{
		printf("No directories found!\n");
		return;
	}

	Dir *aux = (*target)->head_children_dirs;

	while(aux != NULL)
	{
		if(strcmp(aux->name, name) == 0)
		{
			*target = aux;
			return;
		}
		aux = aux->next;
	}

	//cazul in care nu s-a gasit directorul
	printf("No directories found!\n");
}

char *pwd (Dir* target) 
{
	char *path = (char *)calloc(MAX_INPUT_LINE_SIZE * 10, sizeof(char));
	char *name = (char *)calloc(MAX_INPUT_LINE_SIZE * 10, sizeof(char));

	Dir* aux = target;

	//parcurgere de la calea curenta catre radacina si concatenare la inceput
	while(aux != NULL)
	{
		strcpy(name, aux->name);
		strcat(name, path);
		strcpy(path, name);

		strcpy(name, "/");
		strcat(name, path);
		strcpy(path, name);

		aux = aux->parent;
	}

	free(name);
	return path;
}

void stop (Dir* target) 
{
	freeDir(&target);
}

void tree (Dir* target, int level) 
{
	if(target->head_children_dirs != NULL)
	{
		Dir *aux = target->head_children_dirs;

		while(aux != NULL)
		{
			for(int i = 0; i < level; i++)
				printf("    ");

			printf("%s\n", aux->name);
			if(aux->head_children_dirs != NULL)
				//apel recursiv pt afisarea subdirectoarelor
				tree(aux, level + 1);

			aux = aux->next;
		}

	}

	if(target->head_children_files != NULL)
	{
		File *aux = target->head_children_files;

		while(aux != NULL)
		{
			for(int i = 0; i < level; i++)
				printf("    ");

			printf("%s\n", aux->name);

			aux = aux->next;
		}
	}
}

void mv(Dir* parent, char *oldname, char *newname) 
{
	//cazul in care nu exista directorul/fisierul
	if(parent->head_children_dirs == NULL && parent->head_children_files == NULL)
	{
		printf("File/Director not found\n");
		return;
	}

	//verificare daca exista deja un director cu acel nume
	Dir *currentd = parent->head_children_dirs;
	while(currentd != NULL)
	{
		if(strcmp(currentd->name, newname) == 0)
		{
			printf("File/Director already exists\n");
			return;
		}
		currentd = currentd->next;
	}
	//verificare daca exista deja un fisier cu acel nume
	File *currentf = parent->head_children_files;
	while(currentf != NULL)
	{
		if(strcmp(currentf->name, newname) == 0)
		{
			printf("File/Director already exists\n");
			return;
		}
		currentf = currentf->next;
	}

	if(parent->head_children_dirs != NULL)
	{
		//cazul in care directorul este singurul
		if(strcmp(parent->head_children_dirs->name, oldname) == 0 && parent->head_children_dirs->next == NULL)
		{
			//doar schimbam numele
			strcpy(parent->head_children_dirs->name, newname);
			return;
		}

		//cazul in care directorul de redenumit e primul din lista
		if(strcmp(parent->head_children_dirs->name, oldname) == 0)
		{
			Dir *aux = parent->head_children_dirs;
			parent->head_children_dirs = parent->head_children_dirs->next;

			Dir *p = parent->head_children_dirs;
			//parcurgere lista pana la sfarsit pt adaugare director redenumit
			while(p->next != NULL)
			{
				p = p->next;
			}

			strcpy(aux->name, newname);
			p->next = aux;
			aux->next = NULL;

			return;
		}

		Dir *prevd = parent->head_children_dirs;
		currentd = parent->head_children_dirs->next;
		Dir *auxd;

		//cazul in care directorul de redenumit nu e primul
		while(currentd != NULL)
		{
			if(strcmp(currentd->name, oldname) == 0)
			{
				auxd = currentd;
				prevd->next = currentd->next;

				Dir *p = parent->head_children_dirs;
				//parcurgere lista pana la sfarsit pt adaugare director redenumit
				while(p->next != NULL)
				{
					p = p->next;
				}

				strcpy(auxd->name, newname);
				p->next = auxd;
				auxd->next = NULL;
				return;
			}
			prevd = prevd->next;
			currentd = currentd->next;
		}
	}

	if(parent->head_children_files != NULL)
	{
		//cazul in care fisierul este singurul
		if(strcmp(parent->head_children_files->name, oldname) == 0 && parent->head_children_files->next == NULL)
		{
			//doar schimbam numele
			strcpy(parent->head_children_files->name, newname);
			return;
		}

		//cazul in care fisierul de redenumit e primul
		if(strcmp(parent->head_children_files->name, oldname) == 0)
		{
			File *aux = parent->head_children_files;
			parent->head_children_files = parent->head_children_files->next;

			File *p = parent->head_children_files;
			//parcurgere lista pana la sfarsit pt adaugare fisier redenumit
			while(p->next != NULL)
			{
				p = p->next;
			}

			strcpy(aux->name, newname);
			p->next = aux;
			aux->next = NULL;

			return;
		}

		File *prevf = parent->head_children_files;
		currentf = parent->head_children_files->next;
		File *auxf;

		//cazul in care fisierul de redenumit nu e primul
		while(currentf != NULL)
		{
			if(strcmp(currentf->name, oldname) == 0)
			{
				auxf = currentf;
				prevf->next = currentf->next;

				File *p = parent->head_children_files;
				//parcurgere lista pana la sfarsit pt adaugare fisier redenumit
				while(p->next != NULL)
				{
					p = p->next;
				}

				strcpy(auxf->name, newname);
				p->next = auxf;
				auxf->next = NULL;

				return;
			}
			prevf = prevf->next;
			currentf = currentf->next;
		}
	}
	
	//daca se ajunge aici inseamna ca nu exista director/fisier cu numele dat
	printf("File/Director not found\n");
}

int main () 
{
	char *line = (char *)calloc(MAX_INPUT_LINE_SIZE, sizeof(char));
	char *command = (char *)calloc(MAX_INPUT_LINE_SIZE, sizeof(char));
	char *arg = (char *)calloc(MAX_INPUT_LINE_SIZE, sizeof(char));
	char *arg2 = (char *)calloc(MAX_INPUT_LINE_SIZE, sizeof(char));
	size_t n = MAX_INPUT_LINE_SIZE;

	Dir *current = initDir("home");
	Dir *home = current;

	do
	{
		sscanf(line, "%s %s %s", command, arg, arg2);

		if(strcmp(command, "touch") == 0)
		{
			touch(current, arg);
		}

		if(strcmp(command, "mkdir") == 0)
		{
			mkdir(current, arg);
		}

		if(strcmp(command, "ls") == 0)
		{
			ls(current);
		}

		if(strcmp(command, "rm") == 0)
		{
			rm(current, arg);
		}

		if(strcmp(command, "rmdir") == 0)
		{
			rmdir(current, arg);
		}

		if(strcmp(command, "cd") == 0)
		{
			cd(&current, arg);
		}

		if(strcmp(command, "tree") == 0)
		{
			tree(current, 0);
		}

		if(strcmp(command, "pwd") == 0)
		{
			char *path = pwd(current);
			printf("%s\n", path);
			free(path);
		}

		if(strcmp(command, "mv") == 0)
		{
			mv(current, arg, arg2);
		}

		if(strcmp(command, "stop") == 0)
		{
			stop(home);
			free(command);
			free(arg);
			free(arg2);
			free(line);
			break;
		}

	} while (getline(&line, &n, stdin) != -1);
	
	return 0;
}
