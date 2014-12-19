#include "interpreter.h"
#include <stdio.h>

/* http://www.gnu.org/software/bison/manual/html_node/Error-Reporting.html
 * """ After yyerror returns to yyparse, the latter will attempt error recovery
 * if you have written suitable error recovery grammar rules (see Error Recovery).
 * If recovery is impossible, yyparse will immediately return 1."""
 */
void yyerror(const char * msg) {
    fprintf (stderr, "%s\n", msg);
}
