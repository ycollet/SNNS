#include "xvis.h"

/* labs(): bilde den Betrag eines Ausdrucks vom Typ long */
#define labs(magnitude)    ((magnitude) < 0L ? - (magnitude) : (magnitude))

/* min3(): Index des Minimums von a, b und c */
#define min3(a,b,c)        ((a) <= (b)? ((a) <= (c)? 1 : 3) :       \
                                        ((b) <= (c)? 2 : 3));
#define FROM_CUR     1
#define FROM_BEGIN   2
#define FROM_END     3


static struct Member *locate(Collection, long, int);
static struct Member *nodeAt(Collection, long);


/****************************************************/
/* Antworte eine neue Collection. Allokiere dafuer  */
/* den entsprechenden Speicher und initialisiere    */
/* die Collection. Antworte NULL, wenn zuwenig      */
/* Speicher zur Verfuegung steht.                   */
/****************************************************/
Collection newColl()
{
  Collection coll;

  coll = (Collection) malloc(sizeof(*coll));
  if(coll)
  { coll->count = 0L;
    coll->first = NULL;
    coll->last = NULL;
    coll->lastVisited = NULL;
    coll->lastIndex = 0;
  }

  return coll;
}          /* newColl */


/****************************************************/
/* Fuege der Collection coll ans Ende das Element   */
/* member an. Antworte coll, wenn dies gelingt,     */
/* sonst NULL (kein freier Speicher).               */
/****************************************************/
Collection add(Collection coll, void *member)
{
  struct Member *new;

  new = (struct Member *) malloc(sizeof(*new));
  if(! new) return NULL;
  else
  { new->next = NULL;
    new->data = member;
    new->previous = coll->last;
    if(coll->count)
    { coll->last->next = new;
      coll->last = new;
    }
    else coll->first = coll->last = new;
    coll->count++;
    return coll;
  }
}               /* add */


/****************************************************/
/****************************************************/
static struct Member * locate(Collection coll, long offset, int start)
{ struct Member *mp;
  long i;

  switch(start)
  {
    case FROM_CUR:   mp = coll->lastVisited;
                     break;
    case FROM_BEGIN: mp = coll->first;
                     break;
    case FROM_END:   mp = coll->last;
                     break;
  }

  if(offset >= 0L)
    for(i = 1L; i <= offset; i++) mp = mp->next;
  else
    for(i = -1L; i >= offset; i--) mp = mp->previous;
  return mp;
}                /* locate */


/****************************************************/
/* Antworte das pos-te Element der Collection coll. */
/****************************************************/
void *at(Collection coll, long pos)
{ struct Member *mp;

  mp = nodeAt(coll, pos);
  coll->lastIndex = pos;
  coll->lastVisited = mp;
  return mp->data;
}            /* at */


/****************************************************/
/* Ersetze das Element an der Position pos in coll  */
/* durch elem. Der Speicher des bisherigen Elements */
/* wird freigegeben.                                */
/****************************************************/
void put(Collection coll, long pos, void *elem)
{ struct Member *mp;

  mp = nodeAt(coll, pos);
  free(mp->data);
  mp->data = elem;
  coll->lastIndex = pos;
  coll->lastVisited = mp;
}            /* put */


/****************************************************/
/****************************************************/
static struct Member *nodeAt(Collection coll, long pos)
{ long distToFirst, distToLast, distToLastVisited;
  struct Member *mp;
  int selector;

  distToLast = size(coll) - pos;
  distToFirst = pos - 1L;
  distToLastVisited = coll->lastIndex ? pos - coll->lastIndex : size(coll);
  selector = min3(labs(distToLastVisited), distToFirst, distToLast);
  switch(selector)
  {
    case 1: /* die geringste Entfernung zu pos hat coll->lastIndex */
            mp = locate(coll, distToLastVisited, FROM_CUR);
            break;
    case 2: /* pos hat zum Beginn der Collection coll geringsten Abstand */
            mp = locate(coll, distToFirst, FROM_BEGIN);
            break;
    default: mp = locate(coll, - distToLast, FROM_END);
             break;
  }     /* switch */

  return mp;
}           /* nodeAt */


/****************************************************/
/* Loesche das erste Auftreten des Elements elem von*/
/* coll. Es werden die Adressen verglichen.         */
/* Antworte coll.                                   */ 
/****************************************************/
Collection rmv(Collection coll, void *elem)
{ long pos;

  pos = indexOf(coll, elem);
  if(pos > 0) removeAt(coll, pos);
  return coll;
}           /* rmv */


/****************************************************/
/* Loesche das Element mit Index pos.               */
/* Der Speicher, auf den dieses Element zeigt, wird */
/* nicht freigegeben.                               */
/* Antworte coll.                                   */
/****************************************************/
Collection removeAt(Collection coll, long pos)
{ struct Member *toRemove, *pred, *succ;

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
/* Loesche das pos-te Element von coll.             */
/* Der Speicher, auf den dieses Element verweist,   */
/* wird ebenfalls freigegeben.                      */
/* Antworte coll.                                   */
/****************************************************/
Collection removeComplete(Collection coll, long pos)
{ struct Member *toRemove, *pred, *succ;

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
/* Loesche alle Elemente von Position from bis      */
/* Position to einschliesslich.                     */
/****************************************************/
Collection removeFromTo(Collection coll, long from, long to)
{ struct Member *toRemove, *pred, *succ;
  long i;

  toRemove = nodeAt(coll, from);
  pred = toRemove->previous;
  for(i = from; i <= to; i++)
  { succ = toRemove->next;
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
/* Loesche die Collection coll. Fuehre vorher mit   */
/* jedem Element von coll die Funktion func aus.    */
/****************************************************/
void freeDeep(Collection coll, void (*func)())
{ 
  if(notEmpty(coll)) freeFromTo(coll, 1L, size(coll), func);
  free(coll);
}               /* freeDeep */


/****************************************************/
/* Loesche alle Elemente von Position from bis      */
/* Position to einschliesslich.                     */
/* Fuehre vorher mit jedem Element die Fkt. func    */
/* aus, die das jeweilige Element als einzigen      */
/* Parameter erwartet und nichts (void) zurueck-    */
/* liefert.                                         */
/****************************************************/
Collection freeFromTo(Collection coll, long from, long to, void (*func)())
{ struct Member *toRemove, *pred, *succ;
  long i;

  toRemove = nodeAt(coll, from);
  pred = toRemove->previous;
  for(i = from; i <= to; i++)
  { succ = toRemove->next;
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
/* Antworte die Position, an der das Element elem   */
/* zum ersten Mal in coll auftaucht.                */
/* Fuer den Gleichheitstest wird ein Adressvergleich*/
/* durchgefuehrt und nicht ein Vergleich der Werte, */
/* auf die die Pointer zeigen.                      */
/* Ist elem nicht in coll enthalten, so wird mit    */
/* -1L geantwortet.                                 */
/****************************************************/
long indexOf(Collection coll, void *elem)
{ long collSize, i;

  collSize = size(coll);
  for(i = 1L; i <= collSize; i++)
    if(at(coll, i) == elem) return i;

  return -1L;
}               /* indexOf */


/****************************************************/
/* Antworte die Position des ersten Elements von    */
/* coll, fuer das ein Aufruf der Funktion equals()  */
/* mit sich und search als Parameter TRUE zurueck-  */
/* gibt. Falls dies fuer kein Element von coll der  */
/* Fall ist, so antworte -1L.                       */
/****************************************************/
long detectPos(Collection coll, void *search, Boolean (*equals)())
{ long i, collSize = size(coll);

  for(i = 1L; i <= collSize; i++)
    if( (*equals)(search, at(coll, i)) ) return i;

  return -1L;
}               /* detectPos */


/****************************************************/
/* Gebe den Speicherplatz der Collection coll       */
/* inclusive den Platz der Elemente von coll wieder */
/* frei.                                            */
/****************************************************/
void freeCollAll(Collection coll)
{ struct Member *mp, *hilf;

  mp = coll->first;
  while(mp)
  { hilf = mp;
    mp = mp->next;
    free(hilf->data);
    free(hilf);
  }
  free(coll);
}            /* freeCollAll */


/****************************************************/
/* Gebe den Speicherplatz der Collection coll       */
/* wieder frei. Der Speicherplatz fuer der Elemente */
/* von coll bleibt jedoch allokiert.                */
/****************************************************/
void freeColl(Collection coll)
{
  struct Member *mp, *hilf;

  mp = coll->first;
  while(mp)
  { hilf = mp;
    mp = mp->next;
    free(hilf);
  }

  free(coll);
}            /* freeColl */


/****************************************************/
/* Antworte die Anzahl der Elemente von coll.       */
/****************************************************/
long size(Collection coll)
{
  return coll == NULL ? 0 : coll->count;
}            /* size */


/****************************************************/
/* Antworte true gdw. coll kein Element enthaelt,   */
/* sonst false.
/****************************************************/
Boolean isEmpty(Collection coll)
{
  return coll->count == 0L;
}            /* isEmpty */


/****************************************************/
/* Antworte true gdw. coll nicht leer ist, sonst    */
/* false.                                           */
/****************************************************/
Boolean notEmpty(Collection coll)
{
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

