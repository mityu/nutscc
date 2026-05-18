#include "containers/list.h"
#include "gc/gc.h"

typedef struct ListEntry ListEntry;

struct List {
    ListEntry *head, *tail;
};

struct ListEntry {
    ListEntry *prev, *next;
    void *item;
};

struct ListIter {
    bool forward;
    List *list;
    ListEntry *entry; // The entry that currently points to.
};

static ListEntry *new_list_entry(void *item);
static ListIter *new_list_iter(List *iter, bool forward);

List *list_new(void) {
    List *l = (List *)gc_malloc_or_die(sizeof(List));
    l->head = l->tail = NULL;
    return l;
}

ListEntry *new_list_entry(void *item) {
    ListEntry *entry = (ListEntry *)gc_malloc_or_die(sizeof(ListEntry));
    entry->item = item;
    return entry;
}

void list_push_front(List *list, void *item) {
    ListEntry *entry = new_list_entry(item);
    if (list_empty(list)) {
        list->head = list->tail = entry;
    } else {
        entry->next = list->head;
        list->head->prev = entry;
        list->head = entry;
    }
}

void list_push_back(List *list, void *item) {
    ListEntry *entry = new_list_entry(item);
    if (list_empty(list)) {
        list->head = list->tail = entry;
    } else {
        entry->prev = list->tail;
        list->tail->next = entry;
        list->tail = entry;
    }
}

void *list_pop_front(List *list) {
    ListEntry *head = list->head;

    if (list_empty(list)) {
        return NULL;
    }

    list->head = head->next;
    if (list->head == NULL) {
        list->tail = NULL;
    } else {
        list->head->prev = NULL;
    }
    return head->item;
}

void *list_pop_back(List *list) {
    ListEntry *tail = list->tail;

    if (list_empty(list)) {
        return NULL;
    }

    list->tail = tail->prev;
    if (list->tail == NULL) {
        list->head = NULL;
    } else {
        list->tail->next = NULL;
    }
    return tail->item;
}

bool list_empty(const List *list) { return list->head == NULL; }

ListIter *list_find(List *list, void *item) {
    ListIter *iter = list_get_forward_iterator(list);
    while (!listiter_is_end(iter)) {
        if (listiter_get_item(iter) == item) {
            break;
        }
        listiter_next(iter);
    }
    return iter;
}

ListIter *new_list_iter(List *list, bool forward) {
    ListIter *iter = (ListIter *)gc_malloc_or_die(sizeof(ListIter));
    iter->forward = forward;
    iter->list = list;
    if (forward) {
        iter->entry = list->head;
    } else {
        iter->entry = list->tail;
    }
    return iter;
}

ListIter *list_get_forward_iterator(List *list) { return new_list_iter(list, true); }

ListIter *list_get_backward_iterator(List *list) { return new_list_iter(list, false); }

void listiter_next(ListIter *iter) {
    if (iter->entry == NULL) {
        // Already at the end.  Do nothing.
        return;
    }

    if (iter->forward) {
        iter->entry = iter->entry->next;
    } else {
        iter->entry = iter->entry->prev;
    }
}

void listiter_prev(ListIter *iter) {
    if (iter->forward) {
        if (iter->entry != iter->list->head) {
            if (iter->entry == NULL) {
                iter->entry = iter->list->tail;
            } else {
                iter->entry = iter->entry->prev;
            }
        }
    } else {
        if (iter->entry != iter->list->tail) {
            if (iter->entry == NULL) {
                iter->entry = iter->list->head;
            } else {
                iter->entry = iter->entry->next;
            }
        }
    }
}

bool listiter_is_end(const ListIter *iter) { return iter->entry == NULL; }

void *listiter_remove(ListIter *iter) {
    ListEntry *entry = NULL;
    void *item = NULL;
    if (listiter_is_end(iter)) {
        return NULL;
    }

    entry = iter->forward ? iter->entry->next : iter->entry->prev;
    item = iter->entry->item;
    if (iter->entry == iter->list->head) {
        list_pop_front(iter->list);
        iter->entry = entry;
    } else if (iter->entry == iter->list->tail) {
        list_pop_back(iter->list);
        iter->entry = entry;
    } else {
        ListEntry *prev = iter->entry->prev;
        ListEntry *next = iter->entry->next;
        prev->next = next;
        next->prev = prev;
        iter->entry = entry;
    }
    return item;
}

void *listiter_get_item(ListIter *iter) {
    return listiter_is_end(iter) ? NULL : iter->entry->item;
}

void listiter_assign_value(ListIter *iter, void *value) {
    if (!listiter_is_end(iter)) {
        iter->entry->item = value;
    }
}
