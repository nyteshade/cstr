#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * A structure that contains five bytes. Two characters behind, the
 * current character and two characters ahead. This is used to parse
 * and process what to do with a string as it is read. The unions in
 * the struct are there to make access of a given character easier
 * and basically serve as aliases.
 */
typedef struct ReadAhead {
  union { char pp; char prevPrev; char previosPrevious; };
  union { char p; char prev; char previous; };
  union { char c; char cur; char current; };
  union { char n; char next; };
  union { char nn; char nextNext; };
} ReadAhead;

/**
 * Given the pointers to a ReadAhead structure, a pointer to a string or
 * a pointer to a point in a given string, a pointer to the start of the
 * string and a pointer to the end of the string, the method populates 
 * the values relative to pos in the supplied ReadAhead structure. If the
 * two previous characters would appear before the strings start, they are
 * set to NULL. Likewise if the two future characters are out of bounds, they
 * too will be set to NULL. 
 * 
 * @param data the ReadAhead structure to populate
 * @param pos the pointer to the current position within a string
 * @param start a pointer to the start of the string
 * @param end a pointer to the end of the string
 */
void sense(ReadAhead *data, char *pos, char *start, char *end) {
  data->prevPrev = (pos > start + 2) ? *(pos - 2) : '\0';
  data->prev = (pos > start + 1) ? *(pos - 1) : '\0';
  data->cur = *pos;
  data->next = (pos + 1) != end ? *(pos + 1) : '\0';
  data->nextNext = (pos + 2) != end ? *(pos + 2) : '\0';
}

/**
 * Given a flag to toggle and a ReadAhead structure populated with
 * relevant context, the function detects if a non-escaped quotation
 * mark is found and toggles the flag to its opposite state. The quote
 * can be escpaed by preceding it with an asterisk (*) or backslash (\)
 * character.
 * 
 * @param flag a pointer to a boolean or short value that should be toggled
 * @param context the ReadAhead structure populated with relevant context
 */
void checkForQuote(short *flag, ReadAhead *context) {
  if (context->current == '"') {
    if (
      context->previous != '\\' &&
      context->previous != '*' 
    ) *flag = *flag ? 0 : 1;
  }
}

void truncateInPlace(char *before) {
  ReadAhead ra;
  
  char *after = strdup(before);
  char *start = after, *end = after + strlen(after);
  char *bb = before, *aa = after;
  
  if (before && after) {
    while (*bb) {
      sense(&ra, bb, before, before + strlen(before));
      if (ra.cur == '*' && ra.next == '"') {
        bb++;
        continue;
      }
      *aa = *bb;
      aa++;
      bb++;
    }
  }
  *aa = '\0';

  strcpy(before, after);
  free(after);
}

static int setargs(char *args, char **argv)
{
  int count = 0;
  short inQuote = 0;
  char *start = args;
  char *end = start + (strlen(args) - 1);
  ReadAhead ra;
  memset(&ra, 0L, sizeof(ReadAhead));

  // Skip leading whitespace
  while (*args && isspace(*args)) ++args;
  
  // This starts at the first non-whitespace character
  while (*args) {
    // Set the next argument pointer to this location
    if (argv) argv[count] = args;
    
    // Read two characters forward and behind for easier parsing
    sense(&ra, args, start, end);     
   
    // Check to see if the current character is a quotation mark and
    // if the previous character is not an asterisk or a back slash
    // then toggle the inQuote flag.
    checkForQuote(&inQuote, &ra);
         
    // As long as the first character isn't null and isn't a space,
    // unless it is within a quotation, then we zoom along and check 
    // the next character
    while (ra.cur && (!isspace(ra.cur) || (isspace(ra.cur) && inQuote))) {
      ++args;       
      sense(&ra, args, start, end);
      checkForQuote(&inQuote, &ra);
    }

    // If we are here we should be at a space. This is a cut off for us 
    // and we should replace the space with a null character. Any time we
    if (argv && ra.cur) {
      *args++ = '\0';
      sense(&ra, args, start, end);
    }
    
    while (isspace(ra.cur)) { ++args; sense(&ra, args, start, end); }
    count++;
  }
  return count;
}

char **parsedargs(char *args, int *argc)
{
   char **argv = NULL;
   int    argn = 0;

   if (args && *args
    && (args = strdup(args))
    && (argn = setargs(args,NULL))
    && (argv = malloc((argn+1) * sizeof(char *)))) {
      *argv++ = args;
      argn = setargs(args,argv);
   }

   if (args && !argv) free(args);

   *argc = argn;
   return argv;
}

void freeparsedargs(char **argv)
{
  if (argv) {
    free(argv[-1]);
    free(argv-1);
  } 
}

int main(int argc, char *argv[])
{
  int i;
  char **av;
  char **cav;
  int ac;
  char *as = NULL;
  
  if (argc > 1) as = argv[1];

  av = parsedargs(as,&ac);
  printf("== %d\n",ac);
  for (i = 0; i < ac; i++) {
    truncateInPlace(av[i]);
    printf("[%s]\n",av[i]);
  }

  freeparsedargs(av);
  freeparsedargs(cav);
  exit(0);
}