#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <cmath>

using namespace std;

#define BOOL bool
#define TRUE true
#define FALSE false

// Default string padding
extern const char CS_DEFAULT_PADSTRING[] = " ";

// Bitwise operators denoting status for use with StringAction actions
// sa.lastAction |= CSSA_NEW | CSSA_REALLOC to assign both flags
// sa.lastAction &= ~CSSA_NEW to remove one flag
// sa.lastAction &= ~CSSA_NEW & CSSA_REALLOC to remove both
// sa.lastAction ^= CSSA_NEW to toggle the flag from off to on to off again
// sa.lastAction ^= CSSA_NEW | CSSA_REALLOC to toggle both
// 
extern const size_t CSSA_NEW      = pow(2, 0); // new allocation
extern const size_t CSSA_FREE     = pow(2, 1); // memory was freed
extern const size_t CSSA_CONCAT   = pow(2, 2); // new string was allocated
extern const size_t CSSA_REALLOC  = pow(2, 3); // backing store was reallocated
extern const size_t CSSA_FAILED   = pow(2, 4); // failed to allocate ram
extern const size_t CSSA_HEAP     = pow(2, 5); // memory is on the heap
extern const size_t CSSA_SKIPPED  = pow(2, 6); // changes skipped for a reason

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
inline  BOOL          cssa_test(StringAction *action, size_t flag);
inline  BOOL          cssa_testAndClear(StringAction *action, size_t flag);
        StringAction  *cssa_concat(StringAction *string, const char *extra);
        char          *cssa_padEndWith(
                        StringAction *action,
                        size_t length,
                        const char *padString
                      );                      
        
// Prototypes working directly with strings; may use StringAction underneath
inline  char          cs_charAt(const char *string, size_t index);
inline  int           cs_charCodeAt(const char *string, size_t index);
        BOOL          cs_endsWith(const char *string, const char *ending);
        char          *cs_concat(char *stringToModify, const char *extra);
inline  BOOL          cs_includes(const char *haystack, const char *needle);
inline  BOOL          cs_includesAt(
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

// Testbed
int main(int argc, char **argv) {
  StringAction *stringMeta;
  char *string;
  int maxLen;
  

  maxLen = 512;
  stringMeta = cs_copyAndResize("Brielle", maxLen);
  string = stringMeta->string;

  // charAt
  {
    printf("3rd character of %s is %c\n", string, cs_charAt(string, 2));
  }

  // charCodeAt
  {
    printf(
      "Code of 3rd character of %s is %d\n", 
      string, 
      cs_charCodeAt(string, 2)
    );
  }

  // endsWith
  {
    printf(
      "Does %s end with 'lle'? %s\n",
      string,
      cs_endsWith(string, "lle") ? "yes" : "no"
    );
    printf(
      "Does %s end with 'LLE'? %s\n",
      string,
      cs_endsWith(string, "LLE") ? "yes" : "no"
    );
  }

  // concat
  {
    cssa_concat(stringMeta, " is great");
    printf("Concatenated output is '%s'\n", stringMeta->string);    
    cs_freeAndRenew(stringMeta, "Brielle", 0L);
  }
  
  // includes
  {
    printf(
      "'%s' contains '%s'? %s\n", 
      string,
      "iel",
      cs_includes(string, "iel") ? "yes" : "no"
    );
    printf(
      "'%s' contains '%s'? %s\n", 
      string,
      "IEL",
      cs_includes(string, "IEL") ? "yes" : "no"
    );
  }

  // includes, position
  {
    printf(
      "'%s' contains '%s' after the 4th char? %s\n", 
      string, 
      "Brie",
      cs_includesAt(string, "Brie", 3) ? "yes" : "no"
    );
    printf(
      "'%s' contains '%s' from the start? %s\n",
      string,
      "Brie",
      cs_includesAt(string, "Brie", 0) ? "yes" : "no"
    );
  }

  // indexOf
  {
    printf(
      "Index of '%s' in '%s' is %ld\n",
      "elle",
      string,
      cs_indexOf(string, "elle")
    );

    printf(
      "Index of '%s' in '%s' is %ld\n",
      "foo",
      string,
      cs_indexOf(string, "foo")
    );
  }

  // lastIndexOf
  {
    // size_t index = -1;
    // 
    // index = cs_indexOf(string, "e");
    // if (~index) {
    //   while (cs_includesAt(string, "e", index)) {
    //     printf("%s (%ld)\n", string, index);
    //     index += index + cs_indexOf(&string[index], "e");
    //   }
    // }
    // 
    // printf("The last index of '%s' should be %ld\n", "e", index);
  }

  // padEnd
  {
    char *tempString, *tempString2;
    
    tempString = (char *)malloc(11 * sizeof(char));
    tempString2 = (char *)malloc(11 * sizeof(char));
    
    strcpy(tempString, "1234567");
    strcpy(tempString2, tempString);
    
    // test with a heap string using spaces
    printf("The end padded string is '%s'\n", cs_padEnd(tempString, 10));

    // test with a heap string using periods
    printf(
      "The end padded string is '%s'\n", 
      cs_padEndWith(tempString2, 10, "*")
    );
    
    // test with the primary string
    cssa_padEndWith(stringMeta, 10, CS_DEFAULT_PADSTRING);
    printf("The end padded string is '%s'\n", stringMeta->string);
    
    // clean up the temp strings
    free(tempString);
    free(tempString2);
    
    // reset for subsequent tests
    cs_freeAndRenew(stringMeta, "Brielle", maxLen);    
  }

  cs_free(stringMeta);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

StringAction cs_heapAction(char *string) {
  StringAction sa;
  
  sa.string = string;
  sa.length = strlen(string);
  sa.size = sa.length + 1;
  sa.lastAction = CSSA_HEAP | CSSA_NEW;
  sa.recalloced = FALSE;
  sa.reserved = 0L;
  
  return sa;
}

StringAction *cs_new(size_t size) {
  return cs_copyAndResize(0L, size);
}

StringAction *cs_copy(const char *string) {
  return cs_copyAndResize(string, strlen(string) + 1);
}

StringAction *cs_copyAndResize(const char *string, size_t maxSize) {
  return cs_copyAndResizeWith(string, maxSize, 0L);
}

StringAction *cs_copyAndResizeWith(
  const char *string, 
  size_t maxSize, 
  StringAction *useAction
) {
  StringAction *result = 0L;
  
  if (!useAction) {
    result = (StringAction *)malloc(sizeof(StringAction));
    if (!result) {
      return 0L;
    }
  }
  else {
    result = useAction;
  }
  
  result->string = (char *)malloc(maxSize * sizeof(char));
  result->length = 0;
  result->size = maxSize;
  result->lastAction = string ? CSSA_NEW : CSSA_FAILED;
  result->recalloced = TRUE;
  
  if (result->lastAction & CSSA_FAILED) {
    return result;
  }
  
  memset(result->string, 0L, maxSize * sizeof(char));
  
  if (string) {
    strcpy(result->string, string);
    result->length = strlen(string);
  }
  
  return result;  
}

void cs_free(StringAction *action) {
  // Free the string if allocated
  if (action->string) {
    free(action->string);
  }
  
  // Null out the memory space of the action to be thorough 
  memset(action, 0L, sizeof(StringAction));
  
  // Finally free the ram used with the action
  free(action);
}

void cs_freeAndRenew(
  StringAction *action, 
  const char *string, 
  size_t maxSize
) {
  size_t size = maxSize;

  // Free the string if allocated
  if (action->string) {
    free(action->string);
  }
  
  // Zero out the memory for this object
  memset(action, 0L, sizeof(StringAction));
  
  if (maxSize <= 0L) {
    size = strlen(string);
  }
  
  cs_copyAndResizeWith(string, size, action);
}

BOOL cssa_test(StringAction *action, size_t flag) {
  return action->lastAction & flag;
}

BOOL cssa_testAndClear(StringAction *action, size_t flag) {
  BOOL result = action->lastAction & flag;
  action->lastAction = 0L;
  return result;
}

char cs_charAt(const char *string, size_t index) {
  return (char)string[index];
}

int cs_charCodeAt(const char *string, size_t index) {
  return (int)string[index];
}

BOOL cs_endsWith(const char *string, const char *ending) {
  char *substr;
  size_t lastEnd;
  size_t lastStr;
  BOOL result;
  
  substr = strstr(string, ending);
  lastEnd = strlen(ending) - 1;
  lastStr = strlen(string) - 1; 
  result = substr && substr[lastEnd] == string[lastStr]; 
  
  return result;
}

char *cs_concat(char *string, const char *extra) {
  StringAction sa = cs_heapAction(string);
  cssa_concat(&sa, extra);
  return sa.string;
}

StringAction *cssa_concat(StringAction *action, const char *extra) {
  char *string;
  char *newstr;
  char *cattedOut;
  
  string = action->string;
  newstr = (char *)realloc(
    string, 
    (strlen(string) + strlen(extra)) * sizeof(char)
  );
  cattedOut = 0L;
  
  if (!newstr) {
    fprintf(stderr, "Ignoring concat due to memory allocation failure\n");
    action->lastAction |= CSSA_FAILED;
  }
  else {
    cattedOut = strcat(string, extra);
  }
  
  action->string = cattedOut;
  action->length = strlen(cattedOut);
  action->size = action->length;
  action->lastAction |= CSSA_REALLOC;
  action->recalloced = TRUE;
  
  return action;
}

BOOL cs_includes(const char *haystack, const char *needle) {
  return strstr(haystack, needle) != NULL;
}

BOOL cs_includesAt(const char *haystack, const char *needle, size_t fromIndex) {
  return strstr(&haystack[fromIndex], needle);
}

size_t cs_indexOf(const char *haystack, const char *needle) {
  size_t index;
  char *idxString;
  
  index = -1;
  idxString = strstr(haystack, needle);
  
  if (idxString) {
    index = (size_t)idxString - (size_t)haystack;
  }  
  
  return index;
}

char *cs_padEnd(char *string, size_t length) {
  return cs_padEndWith(string, length, CS_DEFAULT_PADSTRING);
}

char *cs_padEndWith(char *string, size_t length, const char *padString) {
  StringAction sa = cs_heapAction(string);
  return cssa_padEndWith(&sa, length, padString);
}

char *cssa_padEndWith(
  StringAction *action,
  size_t length,
  const char *padString
) {
  const char *padding;
  size_t padLen;
  size_t strLen;
  size_t i;
  
  padding = padString ? padString : CS_DEFAULT_PADSTRING;
  
  strLen = strlen(action->string);
  padLen = strlen(padding);
  
  if (strLen >= length) {
    action->lastAction |= CSSA_SKIPPED;
    return action->string;
  }
  
  cs_copyAndResizeWith(action->string, length, action);
  if (action->lastAction & CSSA_FAILED) {
    fprintf(stderr, "Could not allocate memory!");
    return action->string;
  }

  if (strcmp(padding, CS_DEFAULT_PADSTRING) == 0) {
    sprintf(action->string, "%-*s", (int)length, action->string);
  }
  else {
    for (i = strLen; i < length && i < action->size; i += padLen) {
      strcat(action->string, padding);
      if ((i + padLen - 1) >= length) {
        strncat(action->string, padding, length - i);
      }
    }
  }
  
  return action->string;
}