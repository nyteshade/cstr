#include "cstr.h"

const char CS_DEFAULT_PADSTRING[] = " ";
const char CS_EMPTY_STRING[] = "";

const size_t CSSA_NEW      = 1; 
const size_t CSSA_FREE     = 2; 
const size_t CSSA_CONCAT   = 4; 
const size_t CSSA_REALLOC  = 8; 
const size_t CSSA_FAILED   = 16;
const size_t CSSA_HEAP     = 32;
const size_t CSSA_SKIPPED  = 64;

void cs_heapAction(StringAction *sa, char *string) {
  sa->string = string;
  sa->length = strlen(string);
  sa->size = (sa->length + 1) * sizeof(char);
  sa->lastAction = CSSA_HEAP | CSSA_NEW;
  sa->recalloced = FALSE;
  sa->reserved = 0L;
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
    memset(result, 0L, sizeof(StringAction));
  }
  else {
    result = useAction;
  }
  
  result->size = maxSize * sizeof(char);
  result->string = (char *)malloc(result->size);
  result->length = 0;
  result->lastAction = string ? CSSA_NEW : CSSA_FAILED;
  result->recalloced = TRUE;
  
  memset(result->string, 0L, result->size);
  
  if (cssa_testAndClear(result, CSSA_FAILED)) {
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

void cssa_flags(StringAction *action, size_t flags) {
  action->lastAction = 0;
  action->lastAction |= flags;
}

void cssa_modFlags(StringAction *action, size_t flags) {
  action->lastAction = 0;
  action->lastAction |= flags;
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
  StringAction sa;
  cs_heapAction(&sa, string);
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
  action->size = (action->length + 1) * sizeof(char);
  action->lastAction |= CSSA_REALLOC;
  action->recalloced = TRUE;
  
  return action;
}

BOOL cs_includes(const char *haystack, const char *needle) {
  return strstr(haystack, needle) != NULL;
}

BOOL cs_includesAt(const char *haystack, const char *needle, size_t fromIndex) {
  return strstr(&haystack[fromIndex], needle) != NULL;
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
  StringAction sa;
  cs_heapAction(&sa, string);
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
  if (cssa_testAndClear(action, CSSA_FAILED)) {
    fprintf(stderr, "Could not allocate memory!");
    return action->string;
  }

  if (strcmp(padding, CS_DEFAULT_PADSTRING) == 0) {
    sprintf(action->string, "%-*s", (int)length, action->string);
  }
  else {
    for (i = strLen; i < length; i += padLen) {
      strcat(action->string, padding);
      if ((i + padLen - 1) >= length) {
        strncat(action->string, padding, length - i);
      }
    }
  }
  
  return action->string;
}

char *cs_padStart(char *string, size_t length) {
  return cs_padStartWith(string, length, CS_DEFAULT_PADSTRING);
}

char *cs_padStartWith(char *string, size_t length, const char *padString) {
  StringAction sa;
  cs_heapAction(&sa, string);
  return cssa_padStartWith(&sa, length, padString);
}

char *cssa_padStartWith(
  StringAction *action,
  size_t length,
  const char *padString
) {
  if (!action) {
    return NULL;
  }
  
  if (!action->string) {
    cssa_flags(action, CSSA_FAILED);
    return NULL;
  }
  
  char *original;
  const char *padding;
  
  size_t strLen;   // String length
  size_t padLen;   // Length of padding string
  size_t diffLen;  // Length of difference between desired and original  
  int i;           // Iterator index  
  
  strLen = strlen(action->string);
  diffLen = length - strLen;
  
  if (strLen >= length) {
    action->lastAction |= CSSA_SKIPPED;
    return action->string;
  }

  original = strdup(action->string);
  padding = padString ? padString : CS_DEFAULT_PADSTRING;
  padLen = strlen(padding);
  
  cs_copyAndResizeWith(action->string, length, action);
  if (cssa_testAndClear(action, CSSA_FAILED)) {
    fprintf(stderr, "Could not allocate memory!");
    return action->string;
  }
  
  for (i = 0; i < diffLen; i++) {
    action->string[i] = padding[i % padLen];
  }
  
  strcpy(&action->string[i], original);
  
  free(original);
  
  return action->string;
}

char *cs_repeat(char *string, unsigned int times) {
  StringAction sa; 
  cs_heapAction(&sa, string);
  return cssa_repeat(&sa, times);
}

char *cssa_repeat(StringAction *action, unsigned int times) {
  char *buffer;
  int i;
  
  if (times == 0) {
    return strdup(CS_EMPTY_STRING);
  }
  
  if (!action) {
    return NULL;
  }
  
  buffer = (char *)malloc(action->size * times);
  memset(buffer, 0L, action->size * times);
  for (i = 0; i < times; i++) {
    strcat(buffer, action->string);
  }
  
  return buffer;
}