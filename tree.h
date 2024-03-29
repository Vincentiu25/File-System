// Copyright 2022 Mazilu Flavius-Romeo & Tarsoaga Vincentiu-Ionut
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

typedef struct FileContent FileContent;
typedef struct FolderContent FolderContent;
typedef struct TreeNode TreeNode;
typedef struct FileTree FileTree;

enum TreeNodeType {
    FILE_NODE,
    FOLDER_NODE
};

struct FileContent {
    char* text;
};

struct FolderContent {
    linked_list_t* children;
};

struct TreeNode {
    TreeNode* parent;
    char* name;
    enum TreeNodeType type;
    void* content;
};

struct FileTree {
    TreeNode* root;
};

void ls(TreeNode* currentNode, char* arg);
void pwd(TreeNode* treeNode);
TreeNode* cd(TreeNode* currentNode, char* path);
void tree(TreeNode* currentNode, char* arg);
void mkdir(TreeNode* currentNode, char* folderName);
void rm(TreeNode* currentNode, char* fileName);
void rmdir(TreeNode* currentNode, char* folderName);
void rmrec(TreeNode* currentNode, char* resourceName);
void touch(TreeNode* currentNode, char* fileName, char* fileContent);
void cp(TreeNode* currentNode, char* source, char* destination);
void mv(TreeNode* currentNode, char* source, char* destination);
FileTree createFileTree();
void freeTree(FileTree fileTree);
TreeNode *create_TN(TreeNode *parent, char *name, enum TreeNodeType type, char *content);
TreeNode* find_name_in_folder(TreeNode *currentNode, char *name);

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ", __FILE__,		\
				__LINE__);				\
			perror(call_description);			\
			exit(errno);					\
		}							\
	} while (0)
