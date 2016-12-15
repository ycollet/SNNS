#include "xvis.h"

static long strhash(char *, long);
static void freeList(struct Node *);


/****************************************************/
/* Antworte eine neue Symtab. Diese soll buckets    */
/* Buckets fuer offenes Hashing benutzen. Die       */
/* Variable error wird gesetzt.                     */
/****************************************************/
Symtab newSymtab(long buckets)
{ Symtab answer;
  long i;

  answer = (Symtab) malloc(sizeof(*answer));
  if(answer == NULL) error(1);
  answer->buckets = (struct Node **) malloc(sizeof(struct Node *) * buckets);
  if(answer->buckets == NULL) error(1);
  for(i = 0L; i < buckets; i++)
    answer->buckets[i] = NULL;
  if(! (answer->order = newColl())) error(1);
  answer->numBuckets = buckets;

  error = 0;
  return answer;
}               /* newSymtab */


/*****************************************************/
/* Gebe den Speicherplatz fuer Symtab st frei.       */
/* ACHTUNG: Dabei wird auch der Speicher aller       */
/* Klassen-Namen freigegeben. Zeiger auf diese       */
/* duerfen von nun an nicht mehr verwendet werden.   */
/*****************************************************/
void freeSymtab(Symtab st)
{ long i;

  freeCollAll(st->order);
  for(i = 0L; i < st->numBuckets; i++)
    freeList(st->buckets[i]);
  free(st->buckets);
  free(st);
}               /* freeSymtab */


/*****************************************************/
/* Antworte eine Collection aller Symbole, die in st */
/* gespeichert sind. Die Reihenfolge entspricht      */
/* der Reihenfolge, in der die Symbole zu st hinzu-  */
/* gefuegt wurden.                                   */
/*****************************************************/
Collection sequence(Symtab st)
{
  return st->order;
}               /* sequence */


/*****************************************************/
/* Antworte die Anzahl verschiedener Symbole, die in */
/* st enthalten sind.                                */
/*****************************************************/
unsigned numSymbols(Symtab st)
{
  return size(st->order);
}              /* numSymbols */


/*****************************************************/
/* Gebe den Speicher fuer die Liste, die bei node    */
/* beginnt, frei.                                    */
/*****************************************************/
static void freeList(struct Node *node)
{ struct Node *toFree;

  while(node)
  {
    toFree = node;
    node = node->next;
    free(toFree);
  }
}            /* freeList */


/*****************************************************/
/* Fuege den String sym zur Symboltabelle st hinzu.  */
/* Die Symboltabelle haelt sich eine Kopie von sym,  */
/* sodass der Speicher, auf den sym zeigt, nach      */
/* der Rueckkehr von addSymbol() wieder anderweitig  */
/* genutzt werden kann. Antworte die Kopie von sym,  */
/* die vom Aufrufer anstelle von sym benutzt werden  */
/* soll.                                             */
/* Variable error wird gesetzt.                      */
/*****************************************************/
char *addSymbol(Symtab st, char *sym)
{ long hash_val;
  struct Node *newNode;
  char *symCopy;

  symCopy = locateSymbol(st, sym);
  if(symCopy == NULL)
  {                     /* sym ist noch nicht in der Symboltabelle */
    newNode = (struct Node *) malloc(sizeof(*newNode));
    if(newNode == NULL) error(1);
    if(! (symCopy = my_strdup(sym))) error(1);
    hash_val = strhash(sym, st->numBuckets);
    newNode->name = symCopy;
    /* fuege newNode am Anfang ein */
    newNode->next = st->buckets[hash_val];
    st->buckets[hash_val] = newNode;
    /* haenge sym ans Ende der order-Liste */
    if(! add(st->order, symCopy)) error(1);
  }

  error = 0;
  return symCopy;
}               /* addSymbol */


/*****************************************************/
/* Antworte einen Hash-Wert zwischen 0 und           */
/* maxValue - 1 fuer String str.                     */
/*****************************************************/
static long strhash(char *str, long maxValue)
{ int count = 0;
  int sum = 0;

  while(count <= 3 && str[count]) sum += str[count++];
  return sum % maxValue;
}               /* strhash */


/*****************************************************/
/* Antworte einen Zeiger auf sym, falls String sym   */
/* in der Symboltabelle st enthalten ist, sonst      */
/* NULL.                                             */
/*****************************************************/
char * locateSymbol(Symtab st, char *sym)
{ struct Node *node;

  node = st->buckets[strhash(sym, st->numBuckets)];
  while(node)
  {
    if(! strcmp(sym, node->name)) return node->name;
    node = node->next;
  }
  return NULL;
}               /* locateSymbol */


/*****************************************************/
/* Schreibe die Symbole von st ins File f ab der     */
/* aktuellen File Position. Die Symbole werden in    */
/* Reihenfolge geschrieben, wie sie mittels der      */
/* Funktion addSymbol hinzugefuegt wurden. Duplikate */
/* werden vorher eliminiert.                         */
/* Setze Variable error.                             */
/*****************************************************/
void fprintSymbols(Symtab st, FILE *f)
{ long num, i;

  num = size(st->order);
  for(i = 1L; i <= num; i++)
    if(fprintf(f, "%s\n", (char *) at(st->order, i)) < 0) error(15);
  error = 0;
}              /* fprintSymbols */


/******************************************************/
/* Antworte eine Symtab, die die Symbole des Files f  */
/* enthaelt. Dabei werden alle durch White Space von- */
/* einander getrennten Strings als Symbole betrachtet,*/
/* unabhaengig davon, ob mehrere Strings in 1 Zeile   */
/* enthalten sind oder nicht.                         */
/* Variable error wird gesetzt.                       */
/******************************************************/
Symtab readSymtab(FILE *f)
{ Symtab answer;
  char *log;
  Collection lines;
  long len, i, nlines;

  len = flen(f);
  if(error) return;
  if(! (log = (char *) malloc((unsigned) len + 5))) error(1);
  diskToStr(f, log);
  if(! (lines = tokens(log, "\n"))) error(1);

  answer = newSymtab(NO_BUCKETS);
  if(error) return;

  nlines = size(lines);
  for(i = 1L; i <= nlines; i++)
  { addSymbol(answer, (char *) at(lines, i));
    if(error) return;
  }     /* for */

  free(log);
  freeColl(lines);
  error = 0;
  return answer;
}         /* readSymtab */

