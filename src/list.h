#ifndef __DAEDALUS_LIST_H__
#define __DAEDALUS_LIST_H__

/* author: Tobias Assarsson
 * email: tux@kth.se
 * description:
 * 		a simple circular-doubly linked list implementation in C
 */

#ifndef offsetof
#define offsetof(st, m) \
	( (size_t)( (char*)&((st *)0)->m ) )
#endif /* offsetof */

#ifndef container_of
#define container_of(ptr, type, member) \
	( ((type *)((char*)ptr - offsetof(type, member))) )
#endif /* container_of */

typedef struct list_head {
	struct list_head* next;
	struct list_head* prev;
} list_head_t;

typedef struct list {
	struct list_head root;
	size_t size;
} list_t;

list_t* list_new();
void list_init(list_t* lst);
void list_del(list_t* lst);

size_t list_size(list_t* lst);

void list_pushf(list_t* lst, list_head_t* h);
list_head_t* list_popf(list_t* lst);

void list_pushb(list_t* lst, list_head_t* h);
list_head_t* list_popb(list_t* lst);

list_head_t* list_index(list_t* lst, int index);
int list_insert(list_t* lst, list_head_t* _new, int index);
void list_insert_after(list_t* lst, list_head_t* _new, list_head_t* h);
void list_insert_before(list_t* lst, list_head_t* _new, list_head_t* h);
list_head_t* list_remi(list_t* lst, int index);
void list_remh(list_t* lst, list_head_t* h);
list_t* list_splice(list_t* lst, list_head_t* first, list_head_t* last);
list_t* list_split(list_t* lst, size_t i1, size_t i2);
void list_swap(list_head_t* hdr1, list_head_t* hdr2);

#define list_for_each(pos, lst, member) \
	for(pos = container_of((lst)->root.next, typeof(*pos), member); \
			&pos->member != &(lst)->root; \
			pos = container_of(pos->member.next, typeof(*pos), member))

#endif /* __DAEDALUS_LIST_H__ */

