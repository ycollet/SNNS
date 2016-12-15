#include "xvis.h"

int error;                     /* Fehler gdw. error <> 0          */
char errorInfo[MAX_LENGTH_ERRORINFO];
                               /* Zusatzinformation zum Fehler    */
long rowCount;                 /* Bei fehlerhaftem Format des     */
                               /* einzulesenden Pattern Files,    */
                               /* gibt rowCount die Nummer der    */
                               /* Zeile an, in der der Fehler     */
                               /* auftrat.                        */
char tokenval[MAX_LENGTH_TOKENVAL];
                               /* Zusatzinformation zum Token     */
Boolean isDEC;                 /* TRUE gdw. Progr. auf DEC        */
                               /* compiliert.                     */
