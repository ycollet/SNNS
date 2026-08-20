#include "xvis.h"

/* labs(): computes the absolute value of an expression of type long */
#define labs(magnitude)    ((magnitude) < 0L ? - (magnitude) : (magnitude))

/* min3(): index of the minimum of a, b and c */
#define min3(a,b,c)        ((a) <= (b)? ((a) <= (c)? 1 : 3) :       \
                                        ((b) <= (c)? 2 : 3));
#define FROM_CUR     1
#define FROM_BEGIN   2
#define FROM_END     3


static struct Member *locate(Collection, long, int);
static struct Member *nodeAt(Collection, long);


/****************************************************/
/* Returns a new collection. Allocates the          */
/* necessary memory for it and initializes the      */
/* collection. Returns NULL if not enough memory    */
/* is available.                                    */
/****************************************************/
Collection newColl() {
    Collection coll;

    coll = (Collection) malloc(sizeof(*coll));
    if(coll) {
        coll->count = 0L;
        coll->first = NULL;
        coll->last = NULL;
        coll->lastVisited = NULL;
        coll->lastIndex = 0;
    }

    return coll;
}          /* newColl */


/****************************************************/
/* Appends the element member to the end of the     */
/* collection coll. Returns coll if this succeeds,  */
/* otherwise NULL (no free memory).                 */
/****************************************************/
Collection add(Collection coll, void *member) {
    struct Member *new;

    new = (struct Member *) malloc(sizeof(*new));
    if(! new) return NULL;
    else {
        new->next = NULL;
        new->data = member;
        new->previous = coll->last;
        if(coll->count) {
            coll->last->next = new;
            coll->last = new;
        } else coll->first = coll->last = new;
        coll->count++;
        return coll;
    }
}               /* add */


/****************************************************/
/****************************************************/
static struct Member * locate(Collection coll, long offset, int start) {
    struct Member *mp;
    long i;

    switch(start) {
    case FROM_CUR:
        mp = coll->lastVisited;
        break;
    case FROM_BEGIN:
        mp = coll->first;
        break;
    case FROM_END:
        mp = coll->last;
        break;
    }

    if(offset >= 0L)
        for(i = 1L; i <= offset; i++) mp = mp->next;
    else
        for(i = -1L; i >= offset; i--) mp = mp->previous;
    return mp;
}                /* locate */


/****************************************************/
/* Returns the pos-th element of collection coll.   */
/****************************************************/
void *at(Collection coll, long pos) {
    struct Member *mp;

    mp = nodeAt(coll, pos);
    coll->lastIndex = pos;
    coll->lastVisited = mp;
    return mp->data;
}            /* at */


/****************************************************/
/* Replaces the element at position pos in coll     */
/* with elem. The memory of the previous element    */
/* is freed.                                        */
/****************************************************/
void put(Collection coll, long pos, void *elem) {
    struct Member *mp;

    mp = nodeAt(coll, pos);
    free(mp->data);
    mp->data = elem;
    coll->lastIndex = pos;
    coll->lastVisited = mp;
}            /* put */


/****************************************************/
/****************************************************/
static struct Member *nodeAt(Collection coll, long pos) {
    long distToFirst, distToLast, distToLastVisited;
    struct Member *mp;
    int selector;

    distToLast = size(coll) - pos;
    distToFirst = pos - 1L;
    distToLastVisited = coll->lastIndex ? pos - coll->lastIndex : size(coll);
    selector = min3(labs(distToLastVisited), distToFirst, distToLast);
    switch(selector) {
    case 1: /* coll->lastIndex has the shortest distance to pos */
        mp = locate(coll, distToLastVisited, FROM_CUR);
        break;
    case 2: /* pos has the shortest distance to the beginning of collection coll */
        mp = locate(coll, distToFirst, FROM_BEGIN);
        break;
    default:
        mp = locate(coll, - distToLast, FROM_END);
        break;
    }     /* switch */

    return mp;
}           /* nodeAt */


/****************************************************/
/* Removes the first occurrence of the element elem */
/* from coll. Addresses are compared.               */
/* Returns coll.                                    */
/****************************************************/
Collection rmv(Collection coll, void *elem) {
    long pos;

    pos = indexOf(coll, elem);
    if(pos > 0) removeAt(coll, pos);
    return coll;
}           /* rmv */


/****************************************************/
/* Removes the element with index pos.              */
/* The memory that this element points to is        */
/* not freed.                                       */
/* Returns coll.                                    */
/****************************************************/
Collection removeAt(Collection coll, long pos) {
    struct Member *toRemove, *pred, *succ;

    toRemove = nodeAt(coll, pos);
    pred = toRemove->previous;
    succ = toRemove->next;
    if(pred) pred->next = succ;
    if(succ) succ->previous = pred;
    if(pos == 1L) coll->first = succ;
    if(pos == size(coll)) coll->last = pred;
    coll->lastIndex = 0;
    coll->lastVisited = NULL;
    coll->count--;
    free(toRemove);
    return coll;
}               /* removeAt */


/****************************************************/
/* Removes the pos-th element of coll.              */
/* The memory that this element refers to is        */
/* also freed.                                      */
/* Returns coll.                                    */
/****************************************************/
Collection removeComplete(Collection coll, long pos) {
    struct Member *toRemove, *pred, *succ;

    toRemove = nodeAt(coll, pos);
    free(toRemove->data);
    pred = toRemove->previous;
    succ = toRemove->next;
    if(pred) pred->next = succ;
    if(succ) succ->previous = pred;
    if(pos == 1L) coll->first = succ;
    if(pos == size(coll)) coll->last = pred;
    coll->lastIndex = 0;
    coll->lastVisited = NULL;
    coll->count--;
    free(toRemove);
    return coll;
}               /* removeComplete */


/****************************************************/
/* Removes all elements from position from to       */
/* position to inclusive.                           */
/****************************************************/
Collection removeFromTo(Collection coll, long from, long to) {
    struct Member *toRemove, *pred, *succ;
    long i;

    toRemove = nodeAt(coll, from);
    pred = toRemove->previous;
    for(i = from; i <= to; i++) {
        succ = toRemove->next;
        free(toRemove);
        toRemove = succ;
    }
    if(pred) pred->next = succ;
    if(succ) succ->previous = pred;
    if(! pred) coll->first = succ;
    if(! succ) coll->last = pred;
    coll->lastIndex = 0;
    coll->lastVisited = NULL;
    coll->count -= to - from + 1;
    return coll;
}               /* removeFromTo */


/****************************************************/
/* Frees the collection coll. Beforehand, runs      */
/* the function func on every element of coll.      */
/****************************************************/
void freeDeep(Collection coll, void (*func)(void *)) {
    if(notEmpty(coll)) freeFromTo(coll, 1L, size(coll), func);
    free(coll);
}               /* freeDeep */


/****************************************************/
/* Removes all elements from position from to       */
/* position to inclusive.                           */
/* Beforehand, runs the function func on every      */
/* element, which expects the respective element    */
/* as its only parameter and returns nothing        */
/* (void).                                          */
/****************************************************/
Collection freeFromTo(Collection coll, long from, long to, void (*func)(void *)) {
    struct Member *toRemove, *pred, *succ;
    long i;

    toRemove = nodeAt(coll, from);
    pred = toRemove->previous;
    for(i = from; i <= to; i++) {
        succ = toRemove->next;
        (*func)(toRemove->data);
        free(toRemove);
        toRemove = succ;
    }
    if(pred) pred->next = succ;
    if(succ) succ->previous = pred;
    if(! pred) coll->first = succ;
    if(! succ) coll->last = pred;
    coll->lastIndex = 0;
    coll->lastVisited = NULL;
    coll->count -= to - from + 1;
    return coll;
}               /* freeFromTo */


/****************************************************/
/* Returns the position at which the element elem   */
/* first appears in coll.                           */
/* The equality test is performed by comparing      */
/* addresses, not by comparing the values that the  */
/* pointers point to.                               */
/* If elem is not contained in coll, -1L is         */
/* returned.                                        */
/****************************************************/
long indexOf(Collection coll, void *elem) {
    long collSize, i;

    collSize = size(coll);
    for(i = 1L; i <= collSize; i++)
        if(at(coll, i) == elem) return i;

    return -1L;
}               /* indexOf */


/****************************************************/
/* Returns the position of the first element of     */
/* coll for which a call to the function equals()   */
/* with it and search as parameters returns TRUE.   */
/* If this is not the case for any element of coll, */
/* returns -1L.                                     */
/****************************************************/
long detectPos(Collection coll, void *search, Boolean (*equals)(void *, void *)) {
    long i, collSize = size(coll);

    for(i = 1L; i <= collSize; i++)
        if( (*equals)(search, at(coll, i)) ) return i;

    return -1L;
}               /* detectPos */


/****************************************************/
/* Frees the memory of the collection coll,         */
/* including the memory of the elements of coll.    */
/****************************************************/
void freeCollAll(Collection coll) {
    struct Member *mp, *hilf;

    mp = coll->first;
    while(mp) {
        hilf = mp;
        mp = mp->next;
        free(hilf->data);
        free(hilf);
    }
    free(coll);
}            /* freeCollAll */


/****************************************************/
/* Frees the memory of the collection coll.         */
/* The memory for the elements of coll, however,    */
/* remains allocated.                               */
/****************************************************/
void freeColl(Collection coll) {
    struct Member *mp, *hilf;

    mp = coll->first;
    while(mp) {
        hilf = mp;
        mp = mp->next;
        free(hilf);
    }

    free(coll);
}            /* freeColl */


/****************************************************/
/* Returns the number of elements of coll.          */
/****************************************************/
long size(Collection coll) {
    return coll == NULL ? 0 : coll->count;
}            /* size */


/****************************************************/
/* Returns true iff coll contains no element,       */
/* otherwise false.
/****************************************************/
Boolean isEmpty(Collection coll) {
    return coll->count == 0L;
}            /* isEmpty */


/****************************************************/
/* Returns true iff coll is not empty, otherwise    */
/* false.                                           */
/****************************************************/
Boolean notEmpty(Collection coll) {
    return coll->count != 0L;
}            /* notEmpty */


/* main()
{ unsigned *i, *v, pos;
  int j;
  Collection col = newColl();

  do
  { i = (unsigned *) malloc(sizeof(*i));
    printf("Liste aller Instruktionen -> Instruktion 6.\n");
    printf("Instruktion:  ");
    scanf("%d", i);
    switch(*i)
    {
      case 0: exit(0);
	      break;
      case 1: printf("\nPrintout\n");
	      printf("--------\n");
	      for(j=1; j<=size(col); j++)
		printf("%d\n", *(unsigned *) at(col,j));
	      printf("\n");
	      break;
      case 2: printf("Add:  ");
	      v = (unsigned *) malloc(sizeof(*v));
	      scanf("%d", v);
	      add(col, v);
	      break;
      case 3: printf("Remove at position:  ");
	      scanf("%d", &pos);
	      removeAt(col, pos);
	      break;
      case 4: freeColl(col);
	      break;
      case 5: printf("Get element of index:  ");
              scanf("%d", &pos);
              printf("At index: %d\n", *(unsigned *) at(col, pos));
              break;
      case 6: printf("\nBefehle:\n0  Ende\n1 Print\n2 Add\n3 Remove\n4 Destroy\n5 At\n6 Help\n\n");
	      break;
    }
  } while(1);
}
*/

