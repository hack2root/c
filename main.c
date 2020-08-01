#include <stdio.h>
#include <stdlib.h>
#define DEBUG

typedef long long unsigned int LPADDR;

typedef void* payload_ptr;

typedef struct {
    union {
        struct q_type* ptr;
        LPADDR address;
    };
} q_type_ptr;

typedef struct q_type {
    q_type_ptr prev;
    q_type_ptr next;
    union {
        struct { payload_ptr payload; };
        struct { LPADDR address; };
    };
} q_type;


void q_push(q_type_ptr * const head, q_type_ptr* const next) {
    head->ptr->next.ptr = next->ptr;
    next->ptr->prev.ptr = head->ptr;
    head->ptr = next->ptr;
}

q_type_ptr q_pop(q_type_ptr * const head) {
    q_type_ptr tmp;
    if (head->ptr->prev.address == 0) {
        return tmp;
    }
    tmp.ptr = head->ptr;
    head->ptr = head->ptr->prev.ptr;
    head->ptr->next.ptr = 0;
    return tmp;
}

void list_alloc(q_type_ptr * const head, payload_ptr payload) {
    q_type_ptr tmp;
    tmp.ptr = (q_type*)malloc(sizeof(q_type));
    tmp.ptr->payload = payload;
    printf("alloc: 0x%llx 0x%llx\n", tmp.address, tmp.ptr->address);
    q_push(head, &tmp);
}

void list_print(const q_type_ptr const * const q_ptr) {
    int i = 0;
    q_type_ptr tmp;
    tmp.ptr = q_ptr->ptr;
    while (tmp.ptr->prev.address != 0) {
        printf("%d: 0x%llx 0x%llx\n", ++i, tmp.address, tmp.ptr->address);
        tmp.ptr = tmp.ptr->prev.ptr;
    }
    printf("\n");
}

void list_free(const q_type_ptr const * const q_ptr) {
    q_type_ptr tmp;
    tmp.ptr = q_ptr->ptr;
    while (tmp.address != 0) {
        printf("free: 0x%llx 0x%llx\n", tmp.address, tmp.ptr->address);
        q_type* ptr = tmp.ptr;
        tmp.ptr = tmp.ptr->next.ptr;
        free(ptr);
    }
    printf("\n");
}

int main() {
    q_type_ptr head;
    head.ptr = (q_type*)malloc(sizeof(q_type));
    payload_ptr payload = (payload_ptr)0xdeadbeef;
    list_alloc(&head, payload);
    list_alloc(&head, ++payload);
    list_alloc(&head, ++payload);
    list_alloc(&head, ++payload);
    list_alloc(&head, ++payload);
    printf("\n");
#ifdef DEBUG
    list_print(&head);
#endif
    q_type_ptr q_pop0 = q_pop(&head); 
    list_free(&q_pop0);
#ifdef DEBUG
    list_print(&head);
#endif
    q_type_ptr q_pop1 = q_pop(&head); 
    list_free(&q_pop1);
#ifdef DEBUG
    list_print(&head);
#endif
    q_type_ptr q_pop2 = q_pop(&head); 
    list_free(&q_pop2);
#ifdef DEBUG
    list_print(&head);
#endif
    q_type_ptr q_pop3 = q_pop(&head); 
    q_push(&head, &q_pop3);
    q_pop3 = q_pop(&head); 
    list_free(&q_pop3);
#ifdef DEBUG
    list_print(&head);
#endif
    q_type_ptr q_pop4 = q_pop(&head); 
    list_free(&q_pop4);
#ifdef DEBUG
    list_print(&head);
#endif
    free(head.ptr);
}