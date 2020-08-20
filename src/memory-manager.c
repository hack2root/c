#include <stdlib.h>
#include "list.h"

/*
Memoty manager: Type 1
1) Should "alloc" virtually, and return list items, not pointers to arbitrary memory

Method name: alloc

Structure: alloc_buffer ==> list

Structure: alloc_ptr
_____________
| ptr       | - pointer to allocated buffer
|-----------|
| allocated | - number o"f allcated bytes
|___________|

why do we need "allocated on the client", is because we will merge with buffer to the right 

allocation:

we have pointer to allocated initial buffer length 1000, and list contains 1 record
we request 10 bytes to allocated
instead of allocation from OS, we cut first list element greater than allocation request
into two items,

before: [ list item 1 | 10000 ]
after: [ list item 1 | 9990 ]->[ list item 2 | 10 ]

so *(list1 + 10000) - is the next byte to the last allcated byte
and *(list2 + 10) == * (list1 + 10000), so allocated value of first element in a list
starts to decrease.

Easy to prove that we can easily track of list available chunks, 
we will do not make any memory movements (relocations) at all, to sort optimize, cause
we know that this is a very very very bad idea, i.e. one thread to GC, another to MM!

this is crasy and will not work in single-threaded environment well.

Instead, we will try to place blocks sorted from right to left from smallest to 
biggest

Picture I.

->all
______________________
|1    |xxx|2 |x|3  |x|
----------------------

->free
_________________
|1    ||2 ||3  ||
-----------------

->allocated
____________
||xxx||x||x|
------------

what we need to do is to think about

a) how do we know the size of allocated/freed blocks? we pass it to outer system
b) how we can get situation at picture (I)?

alloc 1 -> ptr1
alloc 3 -> ptr2
alloc 1 -> ptr3
alloc 2 -> ptr4
alloc 3 -> ptr5

free ptr2
free ptr4

c)should we compact or relocate memory? Nope if we got 4 Terabytes, and yes 
if we got calculator for 4K bytes of free memory

allocation (using doble stack):

1.1) moving from the head of list of free blocks towards the root, find one with enougth space
1.2) decrease allocated counter for that block (we do not need allcated counter, cause it is a ptr - ptr.prev.ptr), link it to newly created block 
1.3) add newly created allcation block to the list of allocated blocks record 

deallocation (freeing):

1.1) moving from the head of list allcated blocks torwards the root, find one matches the pointer
1.2) insert allcation block back to the list of free blocks or it there is a free block to the left,
1.3) increase counter of free block to the left (we do not need counter cause it is ptr - ptr.pre.ptr), delete block from the list of allocated blocks

2) Should be able to "free" memory pointer accordingly
*/

// thanks for watching!

// queue/list: vtable definition
struct mm_vtable {
    // returns next memory pointer
    struct list* (*alloc)(struct context* const ctx, size_t nmemb, size_t size);
    // releases memory pointer
    void* (*free)(struct context* const ctx, struct list* const pointer);
};

// queue/list: vtable
const struct mm_vtable mm_vt;