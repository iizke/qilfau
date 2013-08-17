/**
 * @file linked_list.h
 * Double Linked List structure
 *
 * @date Created on: Apr 6, 2011
 * @author iizke
 */

#ifndef LINEAR_LIST_H_
#define LINEAR_LIST_H_
#include <stddef.h>

/**
 * Linked list structure
 */
typedef struct linked_list {
  /// Connect to previous node
  struct linked_list *next;
  /// Connect to next node
  struct linked_list *prev;
  /// browsing at
  struct linked_list *browsing_at;
} LINKED_LIST;

/**
 * Linked list manager.
 * Manage not only nodes of list but also free nodes (used for allocating new node)
 */
typedef struct linked_list_manager {
  /// List of nodes
  LINKED_LIST entries;
  /// List of free nodes
  LINKED_LIST free_entries;
  /// List configuration: storing entries or not, storing free nodes or not
  int conf;
} LINKED_LIST_MAN;

/// Configure list to allow storing free nodes
#define LL_CONF_STORE_FREE          0b00000001
/// Configure list to allow storing nodes in list
#define LL_CONF_STORE_ENTRY         0b00000010

/// Error code when failed in allocating new memory for node
#define ERR_LL_MAN_ALLOC            (-2)
/// Error code when configuration of linked list is not correct
#define ERR_LL_CONF_WRONG           (-3)

/// Check linked list empty (return 1) or not (return 0)
#define linked_list_is_empty(_l)   ((_l)->next == (_l))

/// Return the pointer of struct TYPE having a member name MEMBER with pointer ptr
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

int linked_list_init (LINKED_LIST *l);
int linked_list_insert (LINKED_LIST *l, LINKED_LIST *e);
int linked_list_remove (LINKED_LIST *e);
int linked_list_get_first(LINKED_LIST *l, LINKED_LIST **e);
int linked_list_reset_browsing(LINKED_LIST*);
LINKED_LIST * linked_list_get_next(LINKED_LIST*);

int print_list (LINKED_LIST *l);
int test_linked_list ();

int linked_list_man_init (LINKED_LIST_MAN *lm);
int linked_list_man_init_conf (LINKED_LIST_MAN *lm, int conf);
int linked_list_man_insert (LINKED_LIST_MAN *lm, LINKED_LIST *e);
int linked_list_man_remove (LINKED_LIST_MAN *lm, LINKED_LIST *e);
int linked_list_man_get_first(LINKED_LIST_MAN *l, LINKED_LIST **e);
int linked_list_man_get_free_entry (LINKED_LIST_MAN *lm, LINKED_LIST **e);
int linked_list_man_alloc (LINKED_LIST_MAN *l, LINKED_LIST **e, int size);
int test_linked_list_man () ;

#endif /* LINEAR_LIST_H_ */
