/**
 * @file list.h
 * @brief 
 * @author gerald (hongyi.zhu@aurefluidics.com)
 * @version 1.0
 * @date 2021-09-27
 * 
 * @copyright Copyright (c) 2021  Shanghai AureFluidics Technology Co.,Ltd
 * 
 * @par ChangeLog:
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2021-09-27     <td>1.0         <td>gerald      <td>the first version
 * </table>
 */

#ifndef __LIST_H
#define __LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct slist_node slist_t;

struct slist_node
{
    slist_t *next;          /**< point to next node. */
};

typedef struct list_node list_t;

struct list_node
{
    list_t *next;        /**< point to next node. */
    list_t *prev;        /**< point to prev node. */
};

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

/**
 * @brief initialize a list object
 */
#define LIST_OBJECT_INIT(object) { &(object), &(object) }

/**
 * @brief initialize a list
 *
 * @param l list to be initialized
 */
static inline void list_init(list_t *l)
{
    l->next = l->prev = l;
}

/**
 * @brief insert a node after a list
 *
 * @param l list to insert it
 * @param n new node to be inserted
 */
static inline void list_insert_after(list_t *l, list_t *n)
{
    l->next->prev = n;
    n->next = l->next;

    l->next = n;
    n->prev = l;
}

/**
 * @brief insert a node before a list
 *
 * @param n new node to be inserted
 * @param l list to insert it
 */
static inline void list_insert_before(list_t *l, list_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

/**
 * @brief remove node from list.
 * @param n the node to remove from the list.
 */
static inline void list_remove(list_t *n)
{
    n->next->prev = n->prev;
    n->prev->next = n->next;

    n->next = n->prev = n;
}

/**
 * @brief tests whether a list is empty
 * @param l the list to test.
 */
static inline int list_isempty(const list_t *l)
{
    return l->next == l;
}

/**
 * @brief get the list length
 * @param l the list to get.
 */
static inline unsigned int list_len(const list_t *l)
{
    unsigned int len = 0;
    const list_t *p = l;
    while (p->next != l)
    {
        p = p->next;
        len ++;
    }

    return len;
}

/**
 * @brief get the struct for this entry
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define list_entry(node, type, member) \
    container_of(node, type, member)

/**
 * list_for_each - iterate over a list
 * @pos:    the list_t * to use as a loop cursor.
 * @head:   the head for your list.
 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_entry  -   iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member) \
    for (pos = list_entry((head)->next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_first_entry - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

#define SLIST_OBJECT_INIT(object) { NULL }

/**
 * @brief initialize a single list
 *
 * @param l the single list to be initialized
 */
static inline void slist_init(slist_t *l)
{
    l->next = NULL;
}

static inline void slist_append(slist_t *l, slist_t *n)
{
    slist_t *node;

    node = l;
    while (node->next)
    {
        node = node->next;
    }

    /* append the node to the tail */
    node->next = n;
    n->next = NULL;
}

static inline void slist_insert(slist_t *l, slist_t *n)
{
    n->next = l->next;
    l->next = n;
}

static inline unsigned int slist_len(const slist_t *l)
{
    unsigned int len = 0;
    const slist_t *list = l->next;
    while (list != NULL)
    {
        list = list->next;
        len ++;
    }

    return len;
}

static inline slist_t *slist_remove(slist_t *l, slist_t *n)
{
    /* remove slist head */
    slist_t *node = l;
    while (node->next && node->next != n)
    {
        node = node->next;
    }
    /* remove node */
    if (node->next != (slist_t *)0)
    {
        node->next = node->next->next;
    }
    return l;
}

static inline slist_t *slist_first(slist_t *l)
{
    return l->next;
}

static inline slist_t *slist_tail(slist_t *l)
{
    while (l->next)
    {
        l = l->next;
    }
    return l;
}

static inline slist_t *slist_next(slist_t *n)
{
    return n->next;
}

static inline int slist_isempty(slist_t *l)
{
    return l->next == NULL;
}

/**
 * @brief get the struct for this single list node
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define slist_entry(node, type, member) \
    container_of(node, type, member)

/**
 * slist_for_each - iterate over a single list
 * @pos:    the slist_t * to use as a loop cursor.
 * @head:   the head for your single list.
 */
#define slist_for_each(pos, head) \
    for (pos = (head)->next; pos != NULL; pos = pos->next)

/**
 * slist_for_each_entry  -   iterate over single list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your single list.
 * @member: the name of the list_struct within the struct.
 */
#define slist_for_each_entry(pos, head, member) \
    for (pos = slist_entry((head)->next, typeof(*pos), member); \
         &pos->member != (NULL); \
         pos = slist_entry(pos->member.next, typeof(*pos), member))

/**
 * slist_first_entry - get the first element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define slist_first_entry(ptr, type, member) \
    slist_entry((ptr)->next, type, member)

/**
 * slist_tail_entry - get the tail element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define slist_tail_entry(ptr, type, member) \
    slist_entry(slist_tail(ptr), type, member)

#ifdef __cplusplus
}
#endif

#endif /* __LIST_H */
