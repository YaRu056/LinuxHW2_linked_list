#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <time.h>
/* Verify if list is order */
typedef struct __node {

    long value;
    struct list_head list;
} node_t;

struct list_head *list_new()
{
    struct list_head *head = (struct list_head *)malloc(sizeof(struct list_head));
    if (!head) {
        //printf("Fail to allocate space to an empty queue!\n");
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}
/* Free all storage used by list */
void list_free(struct list_head *head)
{
    if (!head)
        return;
    node_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        free(entry);
    }
    free(head);
    head = NULL;
    return;
}
/* Insert an element at head of list */
bool list_insert_head(struct list_head *head, int n)
{
    if (!head )
        return false;
    node_t *node = (node_t*) malloc(sizeof(node_t));
    if (!node) {
        return false;
    }
    INIT_LIST_HEAD(&node->list);
    node->value = n;
    if (!node->value) {
        // printf("Fail to allocate space to string!\n");
        free(node);
        return false;
    }
    //printf("Insert\n");
    list_add(&node->list, head);
    return true;
}

int list_length(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    int len = 0;
    struct list_head *tmp;
    list_for_each (tmp, head) {
        len++;
    }
    return len;
}

static bool list_is_ordered(struct list_head *head)
{       
    if (!head || list_empty(head)) {
        return true;
    }
    struct list_head *cur;
    bool first = true;
    long value;
    list_for_each(cur,head){
        if (cur->next == head){
            break;
        }
        node_t *p = list_entry(cur, node_t, list);
        node_t *n = list_entry(cur->next, node_t, list);
        if (p ->value > n->value) {
            return false;
        } 
    }
    return true;
}
void n_swap(struct list_head *node1, struct list_head *node2)
{
    struct list_head *tmp = node1->next;
    node1->next = node2->next;
    node2->next = tmp;
    if (node1->next != NULL) {
        node1->next->prev = node1;
    }
    if (node2->next != NULL) {
        node2->next->prev = node2;
    }
    tmp = node1->prev;
    node1->prev = node2->prev;
    node2->prev = tmp;
    if (node1->prev != NULL) {
        node1->prev->next = node1;
    }
    if (node2->prev != NULL) {
        node2->prev->next = node2;
    }
}

void rand_pivot(struct list_head *head)
{
    if (!head || list_empty(head) ||  list_is_singular(head)) {
        return;
    }
    int n = rand() % list_length(head);
    struct list_head *cur;
    list_for_each (cur, head) {
        if (n == 0)
            break;
        n--;
    }
    n_swap(head->next, cur);

}
/* shuffle array, only work if n < RAND_MAX */
void shuffle(int *array, size_t n)
{
    if (n <= 0)
        return;

    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        int t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}
node_t *list_remove_head(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    node_t *node;
    node = list_first_entry(head, node_t, list);
    if (node == NULL) {
        free(node);
        return NULL;
    }
    list_del(&node->list);
    return node;
}
void quick_sort(struct list_head **head)
{
    if (!(*head) || list_empty(*head) ||  list_is_singular(*head)) {
        return;
    }
    int n = list_length(*head);
    int value;
    int i = 0;
    int max_level = 2 * n;
    struct list_head *begin[max_level];
    struct list_head *result = list_new(), *left = list_new(), *right = list_new();
    
    begin[0] = *head;
    for(int j=1;j<max_level;j++){
        begin[j] = list_new();
    }           
    while (i >= 0) {
        struct list_head *L = begin[i]->next, *R = begin[i]->prev;
        if (L != R) {
            rand_pivot(begin[i]);
            node_t *pivot = list_remove_head(begin[i]);
            node_t *entry, *safe;
            list_for_each_entry_safe (entry, safe, begin[i], list) {
                list_del(&entry->list);//delete entry->list relation
                if (entry->value > pivot->value) {
                    //printf("add right\n");
                    list_add(&entry->list, right);
                } else {
                    //printf("add left\n");
                    list_add(&entry->list, left);
                }
            }
            list_splice_init(left, begin[i]);
            //printf("add pivot\n");
            list_add(&pivot->list, begin[i + 1]);
            list_splice_init(right, begin[i + 2]);
            i += 2;
        } else {
            if (list_is_singular(begin[i]))
                list_splice_init(begin[i], result);
            i--;
        }
    }
    *head = result;
    for (int i = 0; i < max_level; i++)
        list_free(begin[i]);
    list_free(left);
    list_free(right);

}

int main(int argc, char **argv)
{
    struct list_head *list = list_new();

    size_t count = 10000;

    int *test_arr = (int *)malloc(sizeof(int) * count);

    for (int i = 0; i < count; ++i)
        test_arr[i] = i;
    shuffle(test_arr, count);

    while (count--)
        list_insert_head(list, test_arr[count]);

    quick_sort(&list);
    assert(list_is_ordered(list));
    /*
    //print sorted list
    node_t *cur;
    list_for_each_entry (cur, list, list){
        printf("%ld ", cur->value);
    }
    printf("\n");*/
    list_free(list);

    free(test_arr);

    return 0;
}