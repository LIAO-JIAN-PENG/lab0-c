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
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head *prev = head;
    struct list_head *curr = head->next;

    while (curr != head) {
        prev->next = curr->next;

        curr->prev = prev->next;
        curr->next = prev->next->next;

        prev->next->next = curr;
        prev->next->prev = prev;

        prev = curr;
        curr = curr->next;
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

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    struct list_head list_less, list_greater;
    element_t *pivot;
    element_t *item = NULL, *is = NULL;

    if (list_empty(head) || list_is_singular(head))
        return;

    INIT_LIST_HEAD(&list_less);
    INIT_LIST_HEAD(&list_greater);

    pivot = list_first_entry(head, element_t, list);
    list_del(&pivot->list);

    list_for_each_entry_safe (item, is, head, list) {
        if (strcmp(item->value, pivot->value) < 0)
            list_move_tail(&item->list, &list_less);
        else
            list_move_tail(&item->list, &list_greater);
    }

    q_sort(&list_less, descend);
    q_sort(&list_greater, descend);


    list_add(&pivot->list, head);

    if (descend) {
        list_splice(&list_greater, head);
        list_splice_tail(&list_less, head);
    } else {
        list_splice(&list_less, head);
        list_splice_tail(&list_greater, head);
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
