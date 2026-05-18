#pragma once

#include <stdbool.h>

typedef struct List List;
typedef struct ListIter ListIter;

// Make a new List instance.  Aborts on memory allocation failure.
List *list_new(void);

// Insert `item` at the head of `list`.
void list_push_front(List *list, void *item);

// Append `item` at the end of `list`.
void list_push_back(List *list, void *item);

// Remove the first item in `list` and returns the removed item.
// If the `list` is empty, then returns NULL.
void *list_pop_front(List *list);

// Remove the last item in `list` and returns the removed item.
// If the `list` is empty, then returns NULL.
void *list_pop_back(List *list);

// Check whether the given `list` is empty or not.
// Returns TRUE if the `list` is empty, otherwise returns FALSE.
bool list_empty(const List *list);

// Search for the `item` in `list` and returns an iterator that points to the `item`.
// If `item` does not found then returns an iterator that is already reached at the end.
ListIter *list_find(List *list, void *item);

// Returns a iterator that looks up items from the first item to the last item.
ListIter *list_get_forward_iterator(List *list);

// Returns a iterator that looks up items from the last item to the first item.
ListIter *list_get_backward_iterator(List *list);

// Make `iter` to look a next item.  Do nothing when `iter` already reached at the end.
void listiter_next(ListIter *iter);

// Make `iter` to look a previous item.  Do nothing when `iter` is at the start.
void listiter_prev(ListIter *iter);

// Check whether `iter` already reached at the end.
// Returns TRUE if `iter` points to the end, otherwise returns FALSE.
bool listiter_is_end(const ListIter *iter);

// Remove the item that is pointed by `iter` from list and returns the removed item.
// If `iter` doesn't point to a valid item, do nothing and returns NULL.
void *listiter_remove(ListIter *iter);

// Get the item that `iter` points to.  If `iter` doesn't point to a valid item, then
// returns NULL.
void *listiter_get_item(ListIter *iter);

// Change value of an list item pointed by `iter` into the given `value`.
// Do nothing when the given `iter` doesn't refer to an valid item.
void listiter_assign_value(ListIter *iter, void *value);
