/* author: Tobias Assarsson
 * email: tux@kth.se
 * description:
 * 		a simple circular-doubly linked list implementation in C
 */

#include <malloc.h>
#include "list.h"

/*****************************************************************************/
/*    List head operations                                                   */
/*****************************************************************************/

static void list_head_link(list_head_t* _new, list_head_t* p, list_head_t* n)
{
	_new->next = n;
	_new->prev = p;
	p->next = _new;
	n->prev = _new;
}

static void list_head_unlink(list_head_t* p, list_head_t* n)
{
	p->next = n;
	n->prev = p;
}

static void list_head_add(list_head_t* _new, list_head_t* h)
{
	list_head_link(_new, h, h->next);
}

static void list_head_add_tail(list_head_t* _new, list_head_t* h)
{
	list_head_link(_new, h->prev, h);
}

static void list_head_del(list_head_t* h)
{
	list_head_unlink(h->prev, h->next);
	h->next = h->prev = NULL;
}

/*****************************************************************************/
/*    List operations                                                        */
/*****************************************************************************/

/*
 * list_new:
 * 		create a list dynamically. allocated and initialized.
 * @return : the newly created list.
 */
list_t* list_new()
{
	list_t* _new = (list_t*)malloc(sizeof(list_t));
	if(!_new)
		return NULL;
	list_init(_new);
	return _new;
}

/*
 * list_init:
 * 		initializes a list to default values.
 * @lst : the list to be initialized.
 * @return : void.
 */
void list_init(list_t* lst)
{
	lst->size = 0;
	lst->root.next = &lst->root;
	lst->root.prev = &lst->root;
}

/*
 * list_del:
 * 		deletes a list. pops all elements and frees the list (lst).
 * 		(lst) have to be dynamically allocated first.
 * @lst : the list to delete.
 * @return : void.
 */
void list_del(list_t* lst)
{
	if(!lst)
		return;
	while(list_popb(lst));
	free(lst);
}

/* 
 * list_size: 
 * 		returns the lists (lst) size.
 * @lst : the list.
 * @return : the size of lst.
 */
size_t list_size(list_t* lst)
{
	return lst->size;
}

/*
 * list_pushf:
 * 		push header (h) onto the lists (lst) front.
 * @lst : the list.
 * @h : the header.
 * @return : void.
 */
void list_pushf(list_t* lst, list_head_t* h)
{
	list_head_add(h, &lst->root);
	lst->size++;
}

/*
 * list_popf:
 * 		pop the lists (lst) front.
 * @lst : the lst to pop from.
 * @return : the header at the front of (lst).
 */
list_head_t* list_popf(list_t* lst)
{
	list_head_t* tmp = NULL;
	if(lst->size == 0)
		return NULL;
	tmp = lst->root.next;
	list_head_del(tmp);
	lst->size--;
	return tmp;
}

/*
 * list_pushb:
 * 		push the header (h) into the lists (lst) back.
 * @lst : list to push to.
 * @h : the header to push on the lists back.
 * @return : void.
 */
void list_pushb(list_t* lst, list_head_t* h)
{
	list_head_add_tail(h, &lst->root);
	lst->size++;
}

/*
 * list_popb:
 * 		pop the lists lst back.
 * @lst : the list to pop from.
 * @return : the header poped from lst ( the last one element ).
 */
list_head_t* list_popb(list_t* lst)
{
	list_head_t* tmp = NULL;
	if(lst->size == 0)
		return NULL;
	tmp = lst->root.prev;
	list_head_del(tmp);
	lst->size--;
	return tmp;
}

/*
 * list_index:
 * 		get element at index (index) from the list (lst).
 * @lst : the list.
 * @index : the index to get.
 * @return : the header at the specified index if found,
 * 			 else NULL.
 */
list_head_t* list_index(list_t* lst, int index)
{
	int i;
	list_head_t* tmp = lst->root.next;
	for(i = 0; (i < index) && (tmp != &lst->root); tmp = tmp->next, i++);
	if(tmp == &lst->root)
		return NULL;
	return tmp;
}

/*
 * list_insert:
 * 		insert header (_new) into the list (lst) at index (index).
 * @lst : list to insert into.
 * @_new : header to insert into lst.
 * @index : insert _new as index.
 * @return : 0 if success, -1 if out of bounds.
 */
int list_insert(list_t* lst, list_head_t* _new, int index)
{
	list_head_t* target = list_index(lst, index);
	if(!target)
		return -1;
	list_head_add_tail(_new, target);
	lst->size++;
	return 0;
}

/*
 * list_insert_after:
 * 		insert header (_new) after header (h) in list (lst).
 * @lst : list to insert into.
 * @_new : the header to insert into lst.
 * @h : after this header.
 * @return : void.
 */
void list_insert_after(list_t* lst, list_head_t* _new, list_head_t* h)
{
	list_head_add(_new, h);
	lst->size++;
}

/*
 * list_insert_before:
 * 		insert header (_new) before header (h) in list (lst).
 * @lst : list to insert into.
 * @_new : the header to be inserted into lst.
 * @h : before this header.
 * @return : void.
 */
void list_insert_before(list_t* lst, list_head_t* _new, list_head_t* h)
{
	list_head_add_tail(_new, h);
	lst->size++;
}

/* 
 * list_remi:
 * 		remove header at index (index) from list (lst).
 * @lst : list to remove from.
 * @index : index to be removed.
 * @return : the removed header.
 */
list_head_t* list_remi(list_t* lst, int index)
{
	list_head_t* target = list_index(lst, index);
	if(!target)
		return NULL;
	list_head_del(target);
	lst->size--;
	return target;
}

/*
 * list_remh:
 * 		remove header (h) from list (lst).
 * @lst : list to remove from.
 * @h : header to remove.
 * @return : void.
 */
void list_remh(list_t* lst, list_head_t* h)
{
	list_head_t* target = h;
	if(!target)
		return;
	list_head_del(target);
	lst->size--;
}

/*
 * list_join:
 * 		join two lists. the elements from (src) are consumed
 * 		into (dest).
 * @dest : destination list.
 * @src : source list to consume entries from.
 * @return : number of elements joined with dest.
 */
int list_join(list_t* dest, list_t* src)
{
	list_head_t* dest_root = &dest->root;
	list_head_t* dest_end = dest->root.prev;
	list_head_t* dest_start = dest->root.next;

	list_head_t* src_root = &src->root;
	list_head_t* src_start = src->root.next;
	list_head_t* src_end = src->root.prev;

	size_t tmp;

	if(src->size == 0)
		return 0;

	dest_end->next = src_start;
	src_start->prev = dest_end;
	dest_root->prev = src_end;
	src_end->next = dest_root;

	dest->size += src->size;
	tmp = src->size;
	list_init(src);
	return tmp;
}

/*
 * list_splice:
 * 		splice the list in two parts, new list begins with (first) and
 * 		ends with (last).
 * @lst : list to be spliced.
 * @first : from -
 * @last : to here.
 * @return : new list with the elemets.
 */
list_t* list_splice(list_t* lst, list_head_t* first, list_head_t* last)
{
	list_t* _new = list_new();
	if(!_new)
		return NULL;
	first->prev->next = last->next;
	last->next->prev = first->prev;
	first->prev = &_new->root;
	_new->root.next = first;
	last->next = &_new->root;
	_new->root.prev = last;
	_new->size = 1;
	for(; first != last; first = first->next, _new->size += 1);
	lst->size -= _new->size;
	return _new;
}

/*
 * list_split:
 * 		split list (lst) from index (i1) through (i2) into a new list.
 * @lst : list to split.
 * @i1 : index 1.
 * @i2 : index 2.
 * @return : new list containing index i1 though i2 inclusive.
 */
list_t* list_split(list_t* lst, size_t i1, size_t i2)
{
	size_t lefti = (i1 <= i2) ? i1:i2;
	size_t righti = (i2 <= i1) ? i1:i2;
	list_head_t* left;
	list_head_t* right;
	left = list_index(lst, lefti);
	right = list_index(lst, righti);
	if(left == NULL || right == NULL)
		return NULL;
	list_t* new = list_splice(lst, left, right);
	return new;
}

/* 
 * list_swap:
 * 		swap two headers. the headers can be swapped inter-lists.
 * @hdr1 : swapped with hdr2.
 * @hdr2 : swapped with hdr1.
 * @return : void.
 */
void list_swap(list_head_t* hdr1, list_head_t* hdr2)
{
	list_head_t tmp;
	list_head_t* hdr1_prev;
	list_head_t* hdr1_next;
	list_head_t* hdr2_prev;
	list_head_t* hdr2_next;

	if(!hdr1 || ! hdr2)
		return;

	hdr1_prev = hdr1->prev;
	hdr1_next = hdr1->next;
	hdr2_prev = hdr2->prev;
	hdr2_next = hdr2->next;
	
	hdr1_prev->next = hdr2;
	hdr1_next->prev = hdr2;
	hdr2_prev->next = hdr1;
	hdr2_next->prev = hdr1;

	tmp.next = hdr1->next;
	tmp.prev = hdr1->prev;
	hdr1->next = hdr2->next;
	hdr1->prev = hdr2->prev;
	hdr2->next = tmp.next;
	hdr2->prev = tmp.prev;

}

/* list_is_root:
 * 		tests whether header (h) is the root of the list (lst).
 * @lst : list to test against.
 * @h : the header to be tested.
 * @return : 1 if (h) is the root, else 0.
 */
int list_is_root(list_t* lst, list_head_t* h)
{
	return (&lst->root == h);
}

