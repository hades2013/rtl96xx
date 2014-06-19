#ifndef __LW_LIST_H_
#define __LW_LIST_H_

#ifdef __GNUC__
#define _INLINE_ static __inline__
#else                         /* For Watcom C */
#define _INLINE_ static inline
#endif

#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/**
 * container_of - cast a member of a structure out to the containing structure
 *
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - offsetof(type,member) );})

#ifdef __KERNEL__
#error "prefetch() need to redefine in kernel"
#define prefetch(x)		1
#endif

/* empty define to make this work in userspace -HW */
#ifdef __KERNEL__
#error "smp_wmb() need to redefine in kernel"
#else
#define smp_wmb()
#endif

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define DEFINE_LIST_HEAD(name) struct list_head name

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
_INLINE_ void __list_add(struct list_head * add,
	struct list_head * prev,
	struct list_head * next)
{
	next->prev = add;
	add->next = next;
	add->prev = prev;
	prev->next = add;
}

/**
 * list_add - add a new entry
 * @add:	new entry to be added
 * @head:	list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
_INLINE_ void list_add(struct list_head *add, struct list_head *head)
{
	__list_add(add, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @add:	new entry to be added
 * @head:	list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
_INLINE_ void list_add_tail(struct list_head *add, struct list_head *head)
{
	__list_add(add, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
_INLINE_ void __list_del(struct list_head * prev,
				  struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry:	the element to delete from the list.
 *
 * list_empty() on @entry does not return true after this, @entry is
 * in an undefined state.
 */
_INLINE_ void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry:	the element to delete from the list.
 */
_INLINE_ void list_del_init(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}

/**
 * list_empty - tests whether a list is empty
 * @head:	the list to test.
 */
_INLINE_ int list_empty(struct list_head *head)
{
	return head->next == head;
}

/**
 * list_splice - join two lists
 * @list:	the new list to add.
 * @head:	the place to add it in the first list.
 */
_INLINE_ void list_splice(struct list_head *list, struct list_head *head)
{
	struct list_head *first = list->next;

	if (first != list) {
		struct list_head *last = list->prev;
		struct list_head *at = head->next;

		first->prev = head;
		head->next = first;

		last->next = at;
		at->prev = last;
	}
}

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
     /*lint -save -e413 */ \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
     /*lint -restore */ \
/**
 * list_for_each - iterate over elements in a list
 * @pos:	the &struct list_head to use as a loop counter.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_safe - iterate over elements in a list, but don't dereference
 *                      pos after the body is done (in case it is freed)
 * @pos:	the &struct list_head to use as a loop counter.
 * @pnext:	the &struct list_head to use as a pointer to the next item.
 * @head:	the head for your list (not included in iteration).
 */
#define list_for_each_safe(pos, pnext, head) \
	for (pos = (head)->next, pnext = pos->next; pos != (head); \
	     pos = pnext, pnext = pos->next)

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop counter.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)			\
    /*lint -save -e666 -e718 -e613 -e737 -e26 -e10 -e48 -e40 -e64 -e413 -e520 -e521 -e628 */ \
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		n = list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member)) \
    /*lint -restore */

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop counter.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#ifdef __KERNEL__
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		     prefetch(pos->member.next);			\
	     &pos->member != (head); 					\
	     pos = list_entry(pos->member.next, typeof(*pos), member),	\
		     prefetch(pos->member.next))
#else
/* in userspace, prefetch() will rise the compiler warning */
#define list_for_each_entry(pos, head, member)				\
    /*lint -save -e666 -e718 -e737 -e26 -e10 -e48 -e40 -e64 -e413 -e520 -e521 -e628 */ \
	for (pos = list_entry((head)->next, typeof(*pos), member);\
	     &pos->member != (head); 					\
	     pos = list_entry(pos->member.next, typeof(*pos), member)) \
    /*lint -restore */
#endif

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop counter.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#ifdef __KERNEL__
#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_entry((head)->prev, typeof(*pos), member);	\
	     prefetch(pos->member.prev), &pos->member != (head); 	\
	     pos = list_entry(pos->member.prev, typeof(*pos), member))
#else
/* in userspace, prefetch() will rise the compiler warning */
#define list_for_each_entry_reverse(pos, head, member)			\
    /*lint -save -e666  -e718 -e737 -e26 -e10 -e48 -e40 -e64 -e413 -e520 -e521 -e628 */ \
	for (pos = list_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.prev, typeof(*pos), member)) \
    /*lint -restore */
#endif
/**
 * list_for_each_entry_continue	-	iterate over list of given type continuing after existing point
 * @pos:	the type * to use as a loop counter.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */

/**
 * list_for_each_entry_safe_reverse - iterate backwards over list of given type safe against
 *				      removal of list entry
 * @pos:	the type * to use as a loop counter.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_safe_reverse(pos, n, head, member)		\
    /*lint -save -e666 -e718 -e737 -e26 -e10 -e48 -e40 -e64 -e413 -e520 -e521 -e628 */ \
	for (pos = list_entry((head)->prev, typeof(*pos), member),	\
		n = list_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.prev, typeof(*n), member)) \
    /*lint -restore */
	
#ifdef __KERNEL__
#define list_for_each_entry_continue(pos, head, member)				\
    /*lint -save -e666 -e718 -e737 -e26 -e10 -e48 -e40 -e64 -e413 -e520 -e521 -e628 */ \
	for (pos = list_entry(pos->member.next, typeof(*pos), member),	\
		     prefetch(pos->member.next);			\
	     &pos->member != (head); 					\
	     pos = list_entry(pos->member.next, typeof(*pos), member),	\
		     prefetch(pos->member.next)) \
    /*lint -restore */
#else
/* in userspace, prefetch() will rise the compiler warning */
#define list_for_each_entry_continue(pos, head, member)				\
    /*lint -save -e666 -e718 -e737 -e26 -e10 -e48 -e40 -e64 -e413 -e520 -e521 -e628 */ \
	for (pos = list_entry(pos->member.next, typeof(*pos), member);  \
	     &pos->member != (head); 					                \
	     pos = list_entry(pos->member.next, typeof(*pos), member))  \
    /*lint -restore */
#endif
#ifndef __KERNEL__

#define list_first_entry(first, head, member,type)\
    /*lint -save -e666 -e718 -e737 -e26 -e10 -e48 -e40 -e64 -e413 -e520 -e521 -e628 */ \
    do{                                  \
        if((head)->next==(head))                \
        {                                   \
            first=NULL;                      \
            break;                            \
        }                                      \
        first = list_entry((head)->next, type,member); \
    }while(0)\
    /*lint -restore */

#define  list_next_entry(pos,pnext,head, member,type)\
    /*lint -save -e666 -e718 -e737 -e26 -e10 -e48 -e40 -e64 -e413 -e520 -e521 -e628 */ \
    do{                                     \
        if(((head)->next==(head))||(pos == NULL)||(pos->member.next==(head)))\
        {                                   \
          pnext=NULL;                      \
          break;                         \
        }                                \
        pnext = list_entry(pos->member.next,type,member); \
    }while(0)\
   /*lint -restore */
#endif
#endif /* __LW_LIST_H_ */

