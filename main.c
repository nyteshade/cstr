#include "cstr.h"

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
  
  // padStart
  {
    char *tempString, *tempString2;
    
    tempString = (char *)malloc(11 * sizeof(char));
    tempString2 = (char *)malloc(11 * sizeof(char));
    
    strcpy(tempString, "1234567");
    strcpy(tempString2, tempString);
    
    // test with a heap string using spaces
    printf("The end padded string is '%s'\n", cs_padStart(tempString, 10));

    // test with a heap string using periods
    printf(
      "The end padded string is '%s'\n", 
      cs_padStartWith(tempString2, 10, "*")
    );
    
    // test with the primary string
    cssa_padStartWith(stringMeta, 10, CS_DEFAULT_PADSTRING);
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
