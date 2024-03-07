// Copyright 2022 Mazilu Flavius-Romeo & Tarsoaga Vincentiu-Ionut
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "./LinkedList.h"

#define MAX_STRING_SIZE 64

linked_list_t* ll_create(unsigned int data_size)
{
    linked_list_t *lista = (linked_list_t *) malloc(sizeof(linked_list_t));
    if (!lista) {
        fprintf(stderr, "Eroare la malloc\n");
        return NULL;
    }
    lista->head = NULL;
    lista->data_size = data_size;
    lista->size = 0;
    return lista;
}

ll_node_t* get_nth_node(linked_list_t* list, unsigned int n)
{
	unsigned int len = list->size - 1;
	unsigned int i;
	ll_node_t* node = list->head;

	if (n > len)
		n = len;

	for (i = 0; i < n; ++i)
		node = node->next;

	return node;
}

void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t* cur_n = list->head;
    ll_node_t* new_n = (ll_node_t*)malloc(sizeof(ll_node_t));
    new_n->data = malloc(list->data_size);
    new_n->next = NULL;
    memcpy(new_n->data, new_data, list->data_size);
    if (cur_n == NULL) {
        list->head = new_n;
        list->size++;
        return;
    }
    if (n == 0) {
        new_n->next = list->head;
        list->head = new_n;
    } else {
        unsigned iterator = 0;
            while (cur_n->next != NULL && iterator < n - 1) {
                cur_n = cur_n->next;
                iterator++;
            }
        new_n->next = cur_n->next;
        cur_n->next = new_n;
    }
    list->size++;
}

ll_node_t* ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    if (n == 0) {
        ll_node_t* aux = list->head;
        list->head = list->head->next;
        list->size--;
        return aux;
    }

    ll_node_t *cur_n = list->head;
    for (unsigned i = 0; i < n - 1 && cur_n->next != NULL; i++)
        cur_n = cur_n->next;
    ll_node_t *to_remove_n = cur_n->next;
    cur_n->next = to_remove_n->next;
    list->size--;

    return to_remove_n;
}

unsigned int ll_get_size(linked_list_t* list)
{
	if (list)
		return list->size;
	else
		return 0;
}

void ll_free(linked_list_t** pp_list)
{
    int length = ll_get_size(*pp_list);
    ll_node_t *cur_n = (*pp_list)->head;
    for (int i = 0; i < length; i++) {
        ll_node_t *aux = cur_n->next;
        free(cur_n->data);
        free(cur_n);
        cur_n = aux;
    }
    free(*pp_list);
    *pp_list = NULL;
}

void ll_print_int(linked_list_t* list)
{
	ll_node_t* node = list->head;

	for (; node; node = node->next)
		printf("%d ", *(int*)node->data);
	printf("\n");
}

void ll_print_string(linked_list_t* list)
{
	ll_node_t* node = list->head;

	for (; node; node = node->next)
		printf("%s ", (char*)node->data);
	printf("\n");
}
