#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new = malloc(sizeof(struct list_head));

    if (!new)
        return NULL;

    INIT_LIST_HEAD(new);

    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    element_t *ei, *tmp;

    list_for_each_entry_safe (ei, tmp, l, list) {
        list_del(&ei->list);
        free(ei->value);
        free(ei);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    INIT_LIST_HEAD(&new->list);

    int len = strlen(s) + 1;
    new->value = malloc(sizeof(char) * len);
    if (!new->value) {
        free(new);
        return false;
    }
    strncpy(new->value, s, len);
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    INIT_LIST_HEAD(&new->list);

    int len = strlen(s) + 1;
    new->value = malloc(sizeof(char) * len);
    if (!new->value) {
        free(new);
        return false;
    }
    strncpy(new->value, s, len);
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head))
        return NULL;

    struct list_head *li = head->next;
    element_t *e = list_entry(li, element_t, list);
    strncpy(sp, e->value, bufsize);
    sp[bufsize - 1] = '\0';
    list_del_init(li);

    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head))
        return NULL;

    struct list_head *li = head->prev;
    element_t *e = list_entry(li, element_t, list);
    strncpy(sp, e->value, bufsize);
    sp[bufsize - 1] = '\0';
    list_del_init(li);

    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *slow = head->next;

    for (struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next)
        slow = slow->next;

    struct list_head *del = slow;

    list_del(del);
    free(list_entry(del, element_t, list)->value);
    free(list_entry(del, element_t, list));

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    if (!head)
        return false;

    struct list_head *li, *tmp;
    element_t *prev = NULL;
    int count = 0;

    list_for_each_safe (li, tmp, head) {
        element_t *e = list_entry(li, element_t, list);
        if (prev && !strcmp(prev->value, e->value)) {
            list_del_init(li);
            free(e->value);
            free(e);
            count++;
        } else {
            if (count > 0) {
                list_del_init(&prev->list);
                free(prev->value);
                free(prev);
            }
            prev = e;
            count = 0;
        }
    }

    if (count > 0) {
        list_del_init(&prev->list);
        free(prev->value);
        free(prev);
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head *li = head->next;

    while (li != head && li->next != head) {
        list_move(li, li->next);
        li = li->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *li;

    list_for_each (li, head->prev)
        list_move(head->prev, li);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/

    LIST_HEAD(tmp);
    LIST_HEAD(new_head);

    int times = q_size(head) / (k + !k);
    struct list_head *li;

    for (int i = 0; i < times; i++) {
        int j = 1;
        list_for_each (li, head) {
            if (j == k)
                break;
            j++;
        }

        list_cut_position(&tmp, head, li);

        q_reverse(&tmp);

        list_splice_tail_init(&tmp, &new_head);
    }

    list_splice_init(&new_head, head);
}

/* Merge two list */
void merge(struct list_head *head,
           struct list_head *left,
           struct list_head *right)
{
    while (!list_empty(left) && !list_empty(right)) {
        element_t *l = list_first_entry(left, element_t, list);
        element_t *r = list_first_entry(right, element_t, list);

        if (strcmp(l->value, r->value) < 0) {
            list_move_tail(left->next, head);
        } else {
            list_move_tail(right->next, head);
        }
    }

    if (!list_empty(left))
        list_splice_tail_init(left, head);
    if (!list_empty(right))
        list_splice_tail_init(right, head);
}

void merge_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    LIST_HEAD(left);
    LIST_HEAD(right);

    struct list_head *slow = head->next, *fast = head->next;


    while (fast->next != head && fast->next->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    list_cut_position(&left, head, slow);
    list_splice_tail_init(head, &right);

    merge_sort(&left);
    merge_sort(&right);
    merge(head, &left, &right);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    merge_sort(head);

    if (descend)
        q_reverse(head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    q_reverse(head);

    char *min = NULL;
    struct list_head *li, *tmp;

    list_for_each_safe (li, tmp, head) {
        element_t *e = list_entry(li, element_t, list);
        if (!min || strcmp(e->value, min) < 0)
            min = e->value;
        else {
            list_del(li);
            free(e->value);
            free(e);
        }
    }

    q_reverse(head);
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    q_reverse(head);

    char *max = NULL;
    struct list_head *li, *tmp;

    list_for_each_safe (li, tmp, head) {
        element_t *e = list_entry(li, element_t, list);
        if (!max || strcmp(e->value, max) > 0)
            max = e->value;
        else {
            list_del(li);
            free(e->value);
            free(e);
        }
    }

    q_reverse(head);
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/

    queue_contex_t *qc = list_first_entry(head, queue_contex_t, chain);
    queue_contex_t *tmp = NULL;

    list_for_each_entry (tmp, head, chain) {
        if (tmp == qc)
            continue;

        list_splice_init(tmp->q, qc->q);
        qc->size += tmp->size;
        tmp->size = 0;
    }

    q_sort(qc->q, descend);

    return qc->size;
}
