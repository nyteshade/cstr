#ifndef CSTRING_H
#define CSTRING_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef BOOL
#define BOOL short
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// Default string padding
extern const char CS_DEFAULT_PADSTRING[];

// Bitwise operators denoting status for use with StringAction actions
// sa.lastAction |= CSSA_NEW | CSSA_REALLOC to assign both flags
// sa.lastAction &= ~CSSA_NEW to remove one flag
// sa.lastAction &= ~CSSA_NEW & CSSA_REALLOC to remove both
// sa.lastAction ^= CSSA_NEW to toggle the flag from off to on to off again
// sa.lastAction ^= CSSA_NEW | CSSA_REALLOC to toggle both
// 
extern const size_t CSSA_NEW;  // new allocation
extern const size_t CSSA_FREE;  // memory was freed
extern const size_t CSSA_CONCAT;  // new string was allocated
extern const size_t CSSA_REALLOC;  // backing store was reallocated
extern const size_t CSSA_FAILED; // failed to allocate ram
extern const size_t CSSA_HEAP; // memory is on the heap
extern const size_t CSSA_SKIPPED; // changes skipped for a reason

typedef struct StringAction {
  char *string;      // string pointer
  size_t length;     // length of string up to first null character
  size_t size;       // new allocation size
  size_t lastAction; // last action taken constant
  BOOL recalloced;   // re/c/alloc'ed?
  void *reserved;    // for extensions and future changes
} StringAction;

// Prototypes
StringAction  cs_heapAction(char *string);
StringAction  *cs_new(size_t size);
StringAction  *cs_copy(const char *string);
StringAction  *cs_copyAndResize(const char *string, size_t maxSize);
StringAction  *cs_copyAndResizeWith(
                const char *string, 
                size_t maxSize, 
                StringAction *useAction
              );
void          cs_free(StringAction *action);
void          cs_freeAndRenew(
                StringAction *action, 
                const char *string, 
                size_t maxSize
              );

// Prototypes working directly with StringAction structures
void          cssa_flags(StringAction *action, size_t flags);
void          cssa_modFlags(StringAction *action, size_t flags);
BOOL          cssa_test(StringAction *action, size_t flag);
BOOL          cssa_testAndClear(StringAction *action, size_t flag);
StringAction  *cssa_concat(StringAction *string, const char *extra);
char          *cssa_padEndWith(
                StringAction *action,
                size_t length,
                const char *padString
              );                      
char          *cssa_padStartWith(
                StringAction *action,
                size_t length,
                const char *padString
              );                      
        
// Prototypes working directly with strings; may use StringAction underneath
char          cs_charAt(const char *string, size_t index);
int           cs_charCodeAt(const char *string, size_t index);
BOOL          cs_endsWith(const char *string, const char *ending);
char          *cs_concat(char *stringToModify, const char *extra);
BOOL          cs_includes(const char *haystack, const char *needle);
BOOL          cs_includesAt(
                const char *haystack, 
                const char *needle, 
                size_t fromIndex
              );
size_t        cs_indexOf(const char *haystack, const char *needle);
char          *cs_padEnd(char *string, size_t length);
char          *cs_padEndWith(
                char *string,
                size_t length,
                const char *padString
              );
char          *cs_padStart(char *string, size_t length);
char          *cs_padStartWith(
                char *string,
                size_t length,
                const char *padString
              );


#endif