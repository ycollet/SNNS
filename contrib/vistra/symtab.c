#include "xvis.h"

static long strhash(char *, long);
static void freeList(struct Node *);


/****************************************************/
/* Return a new Symtab. It shall use "buckets"      */
/* buckets for open hashing.                        */
/* Sets the global variable error.                  */
/****************************************************/
Symtab newSymtab(long buckets) {
    Symtab answer;
    long i;

    answer = (Symtab) malloc(sizeof(*answer));
    if(answer == NULL) errorR(1,NULL);
    answer->buckets = (struct Node **) malloc(sizeof(struct Node *) * buckets);
    if(answer->buckets == NULL) errorR(1,NULL);
    for(i = 0L; i < buckets; i++)
        answer->buckets[i] = NULL;
    if(! (answer->order = newColl())) errorR(1,NULL);
    answer->numBuckets = buckets;

    error = 0;
    return answer;
}               /* newSymtab */


/*****************************************************/
/* Free the memory used by Symtab st.                */
/* WARNING: This also frees the memory of all class  */
/* names. Pointers to these must no longer be used   */
/* after this call.                                  */
/*****************************************************/
void freeSymtab(Symtab st) {
    long i;

    freeCollAll(st->order);
    for(i = 0L; i < st->numBuckets; i++)
        freeList(st->buckets[i]);
    free(st->buckets);
    free(st);
}               /* freeSymtab */


/*****************************************************/
/* Return a Collection of all symbols stored in st.  */
/* The order corresponds to the order in which the   */
/* symbols were added to st.                         */
/*****************************************************/
Collection sequence(Symtab st) {
    return st->order;
}               /* sequence */


/*****************************************************/
/* Return the number of distinct symbols contained   */
/* in st.                                            */
/*****************************************************/
unsigned numSymbols(Symtab st) {
    return size(st->order);
}              /* numSymbols */


/*****************************************************/
/* Free the memory for the list starting at node.    */
/*****************************************************/
static void freeList(struct Node *node) {
    struct Node *toFree;

    while(node) {
        toFree = node;
        node = node->next;
        free(toFree);
    }
}            /* freeList */


/*****************************************************/
/* Add the string sym to the symbol table st.        */
/* The symbol table keeps its own copy of sym, so    */
/* the memory that sym points to can be reused for   */
/* other purposes after addSymbol() returns. Return  */
/* the copy of sym that the caller should use         */
/* instead of sym.                                   */
/* Sets the global variable error.                   */
/*****************************************************/
char *addSymbol(Symtab st, char *sym) {
    long hash_val;
    struct Node *newNode;
    char *symCopy;

    symCopy = locateSymbol(st, sym);
    if(symCopy == NULL) {
        /* sym is not yet in the symbol table */
        newNode = (struct Node *) malloc(sizeof(*newNode));
        if(newNode == NULL) errorR(1,NULL);
        if(! (symCopy = my_strdup(sym))) errorR(1,NULL);
        hash_val = strhash(sym, st->numBuckets);
        newNode->name = symCopy;
        /* insert newNode at the beginning */
        newNode->next = st->buckets[hash_val];
        st->buckets[hash_val] = newNode;
        /* append sym to the end of the order list */
        if(! add(st->order, symCopy)) errorR(1,NULL);
    }

    error = 0;
    return symCopy;
}               /* addSymbol */


/*****************************************************/
/* Return a hash value between 0 and maxValue - 1    */
/* for string str.                                   */
/*****************************************************/
static long strhash(char *str, long maxValue) {
    int count = 0;
    int sum = 0;

    while(count <= 3 && str[count]) sum += str[count++];
    return sum % maxValue;
}               /* strhash */


/*****************************************************/
/* Return a pointer to sym if the string sym is      */
/* contained in the symbol table st, otherwise       */
/* NULL.                                             */
/*****************************************************/
char * locateSymbol(Symtab st, char *sym) {
    struct Node *node;

    node = st->buckets[strhash(sym, st->numBuckets)];
    while(node) {
        if(! strcmp(sym, node->name)) return node->name;
        node = node->next;
    }
    return NULL;
}               /* locateSymbol */


/*****************************************************/
/* Write the symbols of st to file f starting at the */
/* current file position. The symbols are written in */
/* the order in which they were added via the        */
/* addSymbol function. Duplicates are eliminated     */
/* beforehand.                                       */
/* Sets the global variable error.                   */
/*****************************************************/
void fprintSymbols(Symtab st, FILE *f) {
    long num, i;

    num = size(st->order);
    for(i = 1L; i <= num; i++)
        if(fprintf(f, "%s\n", (char *) at(st->order, i)) < 0) error(15);
    error = 0;
}              /* fprintSymbols */


/******************************************************/
/* Return a Symtab containing the symbols of file f.  */
/* All strings separated from each other by white     */
/* space are considered symbols, regardless of        */
/* whether several strings are contained in a single  */
/* line or not.                                       */
/* Sets the global variable error.                    */
/******************************************************/
Symtab readSymtab(FILE *f) {
    Symtab answer;
    char *log;
    Collection lines;
    long len, i, nlines;

    len = flen(f);
    if(error) return NULL;
    if(! (log = (char *) malloc((unsigned) len + 5))) errorR(1,NULL);
    diskToStr(f, log);
    if(! (lines = tokens(log, "\n"))) errorR(1,NULL);

    answer = newSymtab(NO_BUCKETS);
    if(error) return NULL;

    nlines = size(lines);
    for(i = 1L; i <= nlines; i++) {
        addSymbol(answer, (char *) at(lines, i));
        if(error) return NULL;
    }     /* for */

    free(log);
    freeColl(lines);
    error = 0;
    return answer;
}         /* readSymtab */

