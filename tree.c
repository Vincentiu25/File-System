// Copyright 2022 Mazilu Flavius-Romeo & Tarsoaga Vincentiu-Ionut
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "LinkedList.h"
#include "tree.h"

#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."


FileTree createFileTree(char* rootFolderName) {
	FileTree file_tree;
	// primul NULL->parintele lui root, al doilea NULL->filecontent, ptc e folder
	file_tree.root = create_TN(NULL, rootFolderName, FOLDER_NODE, NULL);
	DIE(!file_tree.root, "malloc createFileTree\n");

	return file_tree;
}
void freeTree_wrapper(TreeNode *current_node)
{
	FolderContent *folder_content = current_node->content;
	linked_list_t *list = folder_content->children;
	ll_node_t *walk = list->head;

	while (walk) {
		TreeNode *curr_TN = walk->data;
		if (curr_TN->type == FILE_NODE) {
			FileContent *file_content = curr_TN->content;
			free(file_content->text);
		} else {
			freeTree_wrapper(curr_TN);
		}
		free(curr_TN->name);
		free(curr_TN->content);
		walk = walk->next;
	}
	ll_free(&list);
}

void freeTree(FileTree fileTree) {
	TreeNode *root = fileTree.root;
	freeTree_wrapper(root);
	free(root->name);
	free(root->content);
	free(root);
}


void ls(TreeNode* currentNode, char* arg) {
	FolderContent *folder_content = currentNode->content;
	linked_list_t *list = folder_content->children;
	if (*arg == 0) {  // afisez num tuturor folderelor/fisierelor din director
		ll_node_t *walk = list->head;
		while (walk) {
			TreeNode *curr_TN = walk->data;
			printf("%s\n", curr_TN->name);
			walk = walk->next;
		}
	} else {
		int ok = 0;
		ll_node_t *walk = list->head;
		while (walk) {
			TreeNode *curr_TN = walk->data;
			if (strcmp(curr_TN->name, arg) == 0) {
				ok = 1;
				if (curr_TN->type == FOLDER_NODE)
					return ls(curr_TN, "\0");
				// altfel daca e file_node
				FileContent *f_content = curr_TN->content;
				printf("%s: %s\n", curr_TN->name, f_content->text);
				return;
			}
			walk = walk->next;
		}
		if (!ok) {
			printf("ls: cannot access '%s': No such file or directory\n", arg);
			return;
		}
	}
}


void pwd(TreeNode* treeNode) {
    int num_path = 0;
    char v[100][100];
	int i = 0;
	strcpy(v[i], treeNode->name);
	i++;
	TreeNode *walk = treeNode->parent;
	while (walk) {
		strcpy(v[i], walk->name);
		i++;
		walk = walk->parent;
	}
    for (int aux = i - 1; aux >=0; aux--) {
		printf("%s", v[aux]);
		if (aux != 0) {
			printf("/");
		}
	}
}

TreeNode* cd_wrapper(TreeNode *currentNode, char *path)
{
    char *token = strtok(path, "/");
	while (token) {
		if (strcmp(token, PARENT_DIR) == 0)
			currentNode = currentNode->parent;
		else
			currentNode = find_name_in_folder(currentNode, token);

		// daca a vrut sa sara dincolo de root sau la un dir care nu exista
		if (!currentNode || currentNode->type == FILE_NODE) {
			return NULL;
		}

		token = strtok(NULL, "/");
	}
	return currentNode;
}
TreeNode* cd(TreeNode* currentNode, char* path)
{
	TreeNode *currentNode_cp = currentNode;
	char *path_cp = strdup(path);
	currentNode = cd_wrapper(currentNode, path);

	if (!currentNode || currentNode->type == FILE_NODE) {
		printf("cd: no such file or directory: %s\n", path_cp);
		free(path_cp);
		return currentNode_cp;
	}

	free(path_cp);
	return currentNode;
}
void tree_wrapper(TreeNode *currentNode, int *dr, int *fl, int tabs)
{
	FolderContent *folder_content = currentNode->content;
	linked_list_t *list = folder_content->children;
	ll_node_t *walk = list->head;

	while (walk) {
		TreeNode *curr_TN = walk->data;
		if (curr_TN->type == FILE_NODE)
			(*fl)++;
		else
			(*dr)++;

		for (int i = 0; i < tabs; i++)
			printf("\t");
		printf("%s\n", curr_TN->name);

		if (curr_TN->type == FOLDER_NODE)
			tree_wrapper(curr_TN, dr, fl, tabs + 1);
		walk = walk->next;
	}
}
void tree(TreeNode* currentNode, char* arg) {
	char *path_cp = strdup(arg);
	if (*arg != 0) {
		currentNode = cd_wrapper(currentNode, arg);
		if (!currentNode || currentNode->type == FILE_NODE) {
			printf("%s [error opening dir]\n\n0 directories, 0 files\n", path_cp);
			free(path_cp);
			return;
		}
	}
	int directories = 0, files = 0;
	tree_wrapper(currentNode, &directories, &files, 0);
	printf("%d directories, %d files\n", directories, files);
	free(path_cp);
}


void mkdir(TreeNode* currentNode, char* folderName) {
	if (find_name_in_folder(currentNode, folderName) != NULL) {
		printf("mkdir: cannot create directory '%s': File exists\n", folderName);
		return;
	}

	FolderContent *folder_content = currentNode->content;
	linked_list_t *list = folder_content->children;

	TreeNode *new_dir = create_TN(currentNode, folderName, FOLDER_NODE, NULL);
	ll_add_nth_node(list, 0, new_dir);
	free(new_dir);
}


void rmrec(TreeNode* currentNode, char* resourceName) {
	// cautam in lista dupa nume
	TreeNode  *de_sters = find_name_in_folder(currentNode, resourceName);
	if (de_sters == NULL) {
		// caz in care nu exista directorul
		printf("rmrec: failed to remove '%s': No such file or directory",
				resourceName);
		return;
	} else {
		// apelam functia de eliberare a unui arbore plecand de la u  nod
		freeTree_wrapper(de_sters);
		// cautam in lista de fisiere si directoare pozitia celui pe care
		// dorim sa il stergem
		FolderContent *folder_content = currentNode->content;
		linked_list_t *list = folder_content->children;
		ll_node_t *walk_poz = list->head;
		int count = 0;
		int n;
		while (walk_poz) {
			// parcurgem lista
			TreeNode *curr_TN = walk_poz->data;
			if (strcmp(curr_TN->name, resourceName) == 0)
				n = count;
			count++;
			walk_poz = walk_poz->next;
		}
		// stergem nodul din lista, aflat pe pozitia n
		ll_node_t *to_remove = ll_remove_nth_node(list, n);
		free(to_remove->data);
		free(to_remove);
	}
}


void touch(TreeNode* currentNode, char* fileName, char* fileContent) {
	FolderContent *folder_content = currentNode->content;
	linked_list_t *list = folder_content->children;
	ll_node_t *walk = list->head;
	// in cazul in care gaseste nod in lista cu num fileName,
	// nu mai face o creare noua
	while (walk) {
		TreeNode *curr_treeNode = walk->data;
		if (strcmp(curr_treeNode->name, fileName) == 0)
			return;  // e deja un folder/file cu numele fileName
		walk = walk->next;
	}
	// creaza nodul in arbore si initializeaza valorile, inclusiv filecontent
	TreeNode *new_TN = create_TN(currentNode, fileName, FILE_NODE, fileContent);

	ll_add_nth_node(list, 0, new_TN);
	free(new_TN);
}

TreeNode* cp_mv_wrapper(TreeNode *currentNode, char *path)
{
	char *token = strtok(path, "/");
	TreeNode *prev_TN = NULL;
	while (token) {
		if (strcmp(token, PARENT_DIR) == 0)
			currentNode = currentNode->parent;
		else
			currentNode = find_name_in_folder(currentNode, token);

		if (!currentNode || currentNode->type == FILE_NODE) {
			token = strtok(NULL, "/");
			// daca mai urmeaza alt lucru in path, path-ul nu e bun
			if (token != NULL)  {
				return NULL;
			} else {
				if (!currentNode)  // se poate crea un fisier
					currentNode = create_TN(prev_TN, token, FILE_NODE, NULL);
				return currentNode;  // ori s-a creat unul, ori era deja
			}
			continue;
		}
		prev_TN = currentNode;
		token = strtok(NULL, "/");
	}
	return currentNode;
}

void cp(TreeNode* currentNode, char* source, char* destination) {
	char *dst_cp = strdup(destination);
	char *src_cp = strdup(source);

	TreeNode *source_TN = cp_mv_wrapper(currentNode, src_cp);
	TreeNode *dest_TN = cp_mv_wrapper(currentNode, dst_cp);
	free(dst_cp);
	free(src_cp);

	if (source_TN->type == FOLDER_NODE) {
		printf("cp: -r not specified; omitting directory '%s'", source);
		return;
	}

	if (!dest_TN) {
		printf("cp: failed to access '%s': Not a directory\n", destination);
		return;
	}

	if (dest_TN->type == FOLDER_NODE) {
		// destinatie = folder -> creez un nod nou unde fac un deep copy
		// pe fisierul sursa, dupa il inserez in lista de copii ai lui dest
		linked_list_t *dest_list = ((FolderContent*)(dest_TN->content))->children;
		char *content = ((FileContent*)(source_TN->content))->text;
		TreeNode *new_entry = create_TN(dest_TN, strdup(source_TN->name),
									 FILE_NODE, strdup(content));

		ll_add_nth_node(dest_list, 0, new_entry);

		free(new_entry);
		return;
	}

	if (dest_TN->type == FILE_NODE) {
		// daca destinatia e fisier, ii sterg text-ul de la content
		// si il fac sa pointeze spre un nou string, o copie al sursei
		FileContent *file_content_dest = dest_TN->content;
		FileContent *file_content_src = source_TN->content;

		free(file_content_dest->text);
		file_content_dest->text = strdup(file_content_src->text);

		return;
	}
}

int find_pos_list(char *key, ll_node_t *head)
{
	// cauta pozitia fisierului/folderului <key> in lista de copii ai unui folder
	int contor = 0;
	while (head) {
		TreeNode *curr_TN = head->data;
		if (strcmp(curr_TN->name, key) == 0)
			break;
		contor++;
		head = head->next;
	}
	return contor;
}

void mv(TreeNode* currentNode, char* source, char* destination) {
	char *dst_cp = strdup(destination);
	char *src_cp = strdup(source);

	TreeNode *source_TN = cp_mv_wrapper(currentNode, src_cp);
	TreeNode *dest_TN = cp_mv_wrapper(currentNode, dst_cp);
	free(dst_cp);
	free(src_cp);

	if (!dest_TN) {
		printf("mv: failed to access '%s': Not a directory\n", destination);
		return;
	}
	if (!source_TN) {
		printf("cp: failed to access '%s': Not a directory\n", source);
		return;
	}

	// tratat cazul in care si destinatia si sursa sunt foldere
	if (dest_TN->type == FOLDER_NODE && source_TN->type == FOLDER_NODE) {
		// partea de stergere din lista de parinti a sursei, directorul source
		FolderContent *aux_content = source_TN->parent->content;
		linked_list_t *list_source_parent = aux_content->children;

		ll_node_t *walk = list_source_parent->head;
		int contor = find_pos_list(source_TN->name, walk);

		ll_node_t *aux_node = ll_remove_nth_node(list_source_parent, contor);
		TreeNode *node_to_move = aux_node->data;
		free(aux_node);

		// partea de adaugare a sursei in lista folderului destinatie
		aux_content = dest_TN->content;
		linked_list_t *list_dest = aux_content->children;

		ll_add_nth_node(list_dest, 0, node_to_move);
		// nu e nevoie sa fie sters decat nodul din lista, nu si campurile lui
		free(node_to_move);
		return;
	}

	// ramane cazul in care sursa e fisier
	TreeNode *source_parent = source_TN->parent;
	linked_list_t *list_parent;
	list_parent = ((FolderContent*)(source_parent)->content)->children;
	int pos = find_pos_list(source_TN->name, list_parent->head);

	ll_node_t *aux_node = ll_remove_nth_node(list_parent, pos);
	TreeNode *TN_remove = aux_node->data;
	free(aux_node);

	if (dest_TN->type == FILE_NODE && source_TN->type == FILE_NODE) {
		char *content_to_move = ((FileContent*)(TN_remove->content))->text;
		// ramane doar content care trebuie mutat in destinatie

		FileContent *f_content = dest_TN->content;
		free(f_content->text);
		f_content->text = content_to_move;
		return;
	}

	if (dest_TN->type == FOLDER_NODE && source_TN->type == FILE_NODE) {
		// cazul de mutat al unui fisier(TM_remove) in folderul dest_TN
		linked_list_t *list = ((FolderContent*)(dest_TN->content))->children;
		ll_add_nth_node(list, 0, TN_remove);
		free(TN_remove);
		return;
	}
}

TreeNode *create_TN(TreeNode *parent, char *name,
					enum TreeNodeType type, char *text_content)
{
	// alocarea memoriei pentru un TreeNode
	TreeNode *new_TN = malloc(sizeof(TreeNode));
	DIE(!new_TN, "malloc new_TN\n");

	// setarea campurilor date ca parametru
	new_TN->name = name;
	new_TN->parent = parent;
	new_TN->type = type;

	if (type == FOLDER_NODE) {
		// nod = folder -> campul de content il aloc ca FolderContent
		// si creez o lista de copii pt folderul nou creat
		new_TN->content = malloc(sizeof(FolderContent));
		DIE(!new_TN->content, "malloc new_content folder_node\n");

		((FolderContent*)new_TN->content)->children = ll_create(sizeof(TreeNode));
		DIE(!((FolderContent*)new_TN->content)->children, "malloc list folder\n");
	} else {
		if (type == FILE_NODE) {
			// daca e file, campul content(void*) il aloc ca FileContent
			// si daca are si content dat ca parametru la citire il setez
			new_TN->content = malloc(sizeof(FileContent));
			DIE(!new_TN->content, "malloc content file_node\n");

			FileContent *file_content = new_TN->content;
			file_content->text = NULL;  // at cand nu avem filecontent

			if (text_content)
				file_content->text = text_content;
		}
	}
	return new_TN;
}

TreeNode* find_name_in_folder(TreeNode *currentNode, char *name) {
	// functie care testeaza daca in lista unui folder se afla
	// un fisier/folder cu numele <name>
	FolderContent *folder_content = currentNode->content;
	linked_list_t *list = folder_content->children;

	ll_node_t *walk = list->head;
	while (walk) {
		TreeNode *curr_TN = walk->data;
		if (strcmp(curr_TN->name, name) == 0)
			return curr_TN;
		walk = walk->next;
	}
	return NULL;
}

void rm(TreeNode* currentNode, char* fileName)
{
	TreeNode  *de_sters = find_name_in_folder(currentNode, fileName);
	if (de_sters == NULL) {
		printf("rm: failed to remove '%s': No such file or directory", fileName);
		return;
	} else if (de_sters->type == FOLDER_NODE) {
		printf("rm: cannot remove '%s': Is a directory", fileName);
		return;
	} else {
		FolderContent *folder_content = currentNode->content;
		linked_list_t *list = folder_content->children;
		ll_node_t *walk = list->head;
		int count = 0;
		int n;
		while (walk) {
			TreeNode *curr_TN = walk->data;
			if (strcmp(curr_TN->name, fileName) == 0)
				n = count;
			count++;
			walk = walk->next;
		}
		ll_node_t *to_remove = ll_remove_nth_node(list, n);
		free(to_remove->data);
		free(to_remove);
	}
}

void rmdir(TreeNode* currentNode, char* folderName)
{
	TreeNode  *de_sters = find_name_in_folder(currentNode, folderName);
	if (de_sters == NULL) {
		printf("rmdir: failed to remove '%s': No such file or directory", folderName);
		return;
	} else if (de_sters->type == FILE_NODE) {
		printf("rmdir: failed to remove '%s': Not a directory", folderName);
		return;
	}
	FolderContent *folder_content = de_sters->content;
	linked_list_t *list = folder_content->children;
	if(list->size != 0) {
		printf("rmdir: failed to remove '%s': Directory not empty", folderName);
		return;
	} else {
		folder_content = currentNode->content;
		list = folder_content->children;
		ll_node_t *walk = list->head;
		int count = 0;
		int n;
		while (walk) {
			TreeNode *curr_TN = walk->data;
			if (strcmp(curr_TN->name, folderName) == 0)
				n = count;
			count++;
			walk = walk->next;
		}
		ll_node_t *to_remove = ll_remove_nth_node(list, n);
		free(to_remove->data);
		free(to_remove);
	}
}
