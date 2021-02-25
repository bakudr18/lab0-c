#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q)
        return;
    while (q->head)
        q_remove_head(q, NULL, 0);
    /* Free queue structure */
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;
    size_t length;
    if (!q)
        return false;

    length = strlen(s) + 1;
    newh = malloc(sizeof(list_ele_t) + sizeof(char) * length);
    if (!newh)
        return false;

    strncpy(newh->value, s, length);
    newh->next = q->head;
    q->head = newh;
    if (!q->tail)
        q->tail = q->head;
    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    list_ele_t *newh;
    size_t length;
    if (!q)
        return false;

    length = strlen(s) + 1;
    newh = malloc(sizeof(list_ele_t) + sizeof(char) * length);
    if (!newh)
        return false;

    strncpy(newh->value, s, length);
    newh->next = NULL;
    if (!q->tail)
        q->head = newh;
    else
        q->tail->next = newh;
    q->tail = newh;
    q->size++;

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    list_ele_t *tmp;
    if (!q || !q->head)
        return false;
    if (sp) {
        size_t length = strlen(q->head->value) + 1;
        length = bufsize < length ? bufsize : length;
        strncpy(sp, q->head->value, length - 1);
        sp[length - 1] = '\0';
    }

    tmp = q->head;
    q->head = q->head->next;
    free(tmp);
    if (!q->head)
        q->tail = NULL;
    q->size--;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return q ? q->size : 0;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q)
        return;
    list_ele_t *curr, *next;
    q->tail = q->head;
    curr = q->head;
    q->head = NULL;

    while (curr) {
        next = curr->next;
        curr->next = q->head;
        q->head = curr;
        curr = next;
    }
}

static list_ele_t *q_split(list_ele_t *walk, size_t step)
{
    list_ele_t *remain;
    while (--step)
        walk = walk->next;
    remain = walk->next;
    walk->next = NULL;
    return remain;
}

static list_ele_t *q_merge_sorted_list(list_ele_t **head,
                                       list_ele_t *first,
                                       list_ele_t *second)
{
    while (first && second) {
        if (strcasecmp(first->value, second->value) < 0) {
            *head = first;
            first = first->next;
        } else {
            *head = second;
            second = second->next;
        }
        head = &(*head)->next;
    }
    if (first)
        *head = first;
    else
        *head = second;

    while ((*head)->next)
        head = &(*head)->next;
    return *head;
}

static list_ele_t *merge_sort(list_ele_t **head, size_t size)
{
    list_ele_t *mid;
    size_t ll, rl;
    if (size < 2)
        return *head;
    rl = size / 2;
    ll = size - rl;

    mid = q_split(*head, ll);
    merge_sort(head, ll);
    merge_sort(&mid, rl);
    return q_merge_sorted_list(head, *head, mid);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q)
        return;
    q->tail = merge_sort(&q->head, q->size);
}
