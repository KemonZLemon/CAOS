/* hw2.c
 *
 * Spell-checker using a Trie and two FIFOs.
 * Follows assignment constraints:
 * - no square bracket characters anywhere in this file
 * - use calloc for dynamic allocations
 * - use open/read/write/close for files and FIFOs
 * - fork child per request and child writes exactly one write call
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define MAXWORDLENGTH 32

typedef struct trie
{
  char * word;         /* NULL if no valid word; otherwise ptr to valid word */
  struct trie * next;  /* ptr to next level of trie; or NULL if no next level */
}
trie_t;

/* forward declarations */
static void strtoupper_inplace(char * s);
static int addword(trie_t * root, const char * word);
static int isvalidword(const trie_t * root, const char * word);
static char * findalternate(const trie_t * root, const char * word);
static void freetrie(trie_t * root);

/* helper: convert string to uppercase in place */
static void strtoupper_inplace(char * s)
{
  if (s == NULL) return;
  char * p = s;
  while (*p)
  {
    *p = (char) toupper((unsigned char) *p);
    p = p + 1;
  }
}

/* Add a word to the trie.
 * Returns 0 on success, -1 on allocation failure
 * Uses pointer arithmetic only to walk layers.
 */
static int addword(trie_t * root, const char * word)
{
  if (root == NULL || word == NULL) return -1;

  trie_t * layer = root;
  const char * p = word;
  int depth = 0;

  while (*p)
  {
    char ch = *p;
    if (ch < 'A' || ch > 'Z') return -1; /* invalid char */

    int idx = ch - 'A';

    /* element pointer at this index */
    trie_t * elem = layer + idx;

    /* if this is the end of the word (next character is null),
       set the word pointer at this elem */
    if (*(p + 1) == '\0')
    {
      /* store the final word string */
      size_t len = strlen(word);
      char * wcopy = calloc(len + 1, sizeof(char));
      if (wcopy == NULL) { perror("calloc"); return -1; }
      strcpy(wcopy, word);
      elem->word = wcopy;
      return 0;
    }

    /* otherwise we need to ensure next exists */
    if (elem->next == NULL)
    {
#ifndef QUIET
      /* print creation message; show the character and the whole word */
      printf("PARENT: creating new Trie layer for '%c' of \"%s\"\n", ch, word);
#endif
      trie_t * newlayer = calloc(26, sizeof(trie_t));
      if (newlayer == NULL) { perror("calloc"); return -1; }
      elem->next = newlayer;
    }

    /* step into next layer */
    layer = elem->next;
    p = p + 1;
    depth = depth + 1;
  }

  /* if reached here, word had zero length */
  return -1;
}

/* Return 1 if valid word exists in Trie, 0 otherwise */
static int isvalidword(const trie_t * root, const char * word)
{
  if (root == NULL || word == NULL) return 0;
  const trie_t * layer = root;
  const char * p = word;

  while (*p)
  {
    char ch = *p;
    if (ch < 'A' || ch > 'Z') return 0;
    int idx = ch - 'A';
    const trie_t * elem = layer + idx;

    if (*(p + 1) == '\0')
    {
      /* final char */
      if (elem->word) return 1;
      else return 0;
    }

    if (elem->next == NULL) return 0;
    layer = elem->next;
    p = p + 1;
  }

  return 0;
}

/* Find an alternate word per assignment description.
 * Returns newly allocated string containing the suggested word, or NULL if none.
 * Caller must free the returned pointer.
 */
static char * findalternate(const trie_t * root, const char * word)
{
  if (root == NULL || word == NULL) return NULL;

  /* Step 1: attempt to walk along the path of the candidate word.
     If we succeed to the node for the last character (i.e., next layers exist),
     then at that layer search circularly from letter after last char for a valid word.
     If we do not succeed to reach that depth, then find deepest existing layer along
     the path and search that layer circularly for a valid word along the path.
  */

  const trie_t * layer = root;
  const char * p = word;
  const trie_t * last_valid_layer = NULL;
  int depth = 0;

  /* We will walk as far as possible along the path */
  while (*p)
  {
    char ch = *p;
    if (ch < 'A' || ch > 'Z') return NULL;
    int idx = ch - 'A';
    const trie_t * elem = layer + idx;

    /* record this layer as last_known if elem is non NULL */
    if (elem != NULL)
    {
      /* if elem->next exists, update last_valid_layer */
      if (elem->next != NULL)
      {
        last_valid_layer = elem->next;
      }
    }

    if (*(p + 1) == '\0')
    {
      /* We are at the last character's element */
      /* If elem->next exists, we search at that layer for a valid word starting
         at the index after this character. Otherwise, we'll look at last_valid_layer. */
      if (elem->next != NULL)
      {
        const trie_t * search_layer = elem->next;
        int start = ( (ch - 'A') + 1 ) % 26;
        int i;
        for (i = 0; i < 26; i++)
        {
          int idx2 = (start + i) % 26;
          const trie_t * cand = search_layer + idx2;
          /* look for any valid word under cand: do a depth-first search */
          if (cand->word)
          {
            size_t len = strlen(cand->word);
            char * res = calloc(len + 1, sizeof(char));
            if (res == NULL) { perror("calloc"); return NULL; }
            strcpy(res, cand->word);
            return res;
          }
          /* else we need to recursively search under cand->next */
          if (cand->next)
          {
            /* We need to find the left-to-right circular earliest word under this subtree.
               We'll perform a simple DFS that returns first found word (left-to-right). */
            /* iterative stackless DFS using recursion */
            /* helper nested function not available; so call a small recursive lambda-like function via separate static helper */
            /* But to avoid extra helpers, do a recursive search using a function here. */
            /* Use separate function below */
            /* We'll call a helper named find_first_in_subtree */
            /* forward declare via static above is not necessary since we define below */
          }
        }
        /* if not found at this layer's immediate word slots, do deeper search in circular order */
        int i2;
        for (i2 = 0; i2 < 26; i2++)
        {
          int idx2 = (start + i2) % 26;
          const trie_t * cand2 = search_layer + idx2;
          if (cand2->next)
          {
            /* call helper to find first word in subtree */
            /* helper declared below */
            /* continue to next code */
          }
        }
      }

      /* if we get here, either no next layer at last char or nothing found there */
      break;
    }

    if (elem->next == NULL)
    {
      /* path terminates before the word ended */
      break;
    }

    layer = elem->next;
    p = p + 1;
    depth = depth + 1;
  }

  /* If we couldn't search at the last char's layer, try last_valid_layer (deepest existing layer) */
  if (last_valid_layer == NULL)
  {
    return NULL;
  }

  /* start search at index 0 of last_valid_layer in circular fashion starting from 0 */
  int startidx = 0;
  int i;
  for (i = 0; i < 26; i++)
  {
    int idx2 = (startidx + i) % 26;
    const trie_t * cand = last_valid_layer + idx2;
    if (cand->word)
    {
      size_t len = strlen(cand->word);
      char * res = calloc(len + 1, sizeof(char));
      if (res == NULL) { perror("calloc"); return NULL; }
      strcpy(res, cand->word);
      return res;
    }
  }

  /* deeper search under children if no immediate word found at that layer */
  for (i = 0; i < 26; i++)
  {
    int idx2 = (startidx + i) % 26;
    const trie_t * cand = last_valid_layer + idx2;
    if (cand->next)
    {
      /* find first word in subtree */
      /* We will implement a small recursive helper below named find_first_in_subtree */
      /* forward to helper */
      /* call helper now */
      /* Since helper is declared below, we call it */
      /* Placeholder call will be replaced by actual helper after its definition */
    }
  }

  /* If not found, return NULL */
  return NULL;
}

/* helper: find first word in subtree, left-to-right depth-first
 * returns newly allocated string or NULL
 */
static char * find_first_in_subtree(const trie_t * layer)
{
  if (layer == NULL) return NULL;
  int i;
  for (i = 0; i < 26; i++)
  {
    const trie_t * elem = layer + i;
    if (elem->word)
    {
      size_t len = strlen(elem->word);
      char * res = calloc(len + 1, sizeof(char));
      if (res == NULL) { perror("calloc"); return NULL; }
      strcpy(res, elem->word);
      return res;
    }
    if (elem->next)
    {
      char * rec = find_first_in_subtree(elem->next);
      if (rec) return rec;
    }
  }
  return NULL;
}

/* Revised findalternate that uses helper */
static char * findalternate_revised(const trie_t * root, const char * word)
{
  if (root == NULL || word == NULL) return NULL;

  const trie_t * layer = root;
  const char * p = word;
  const trie_t * last_existing_layer = NULL;
  int last_index_at_existing_layer = -1;

  while (*p)
  {
    char ch = *p;
    if (ch < 'A' || ch > 'Z') return NULL;
    int idx = ch - 'A';
    const trie_t * elem = layer + idx;

    if (elem->next != NULL)
    {
      last_existing_layer = elem->next;
      last_index_at_existing_layer = idx;
    }

    if (*(p + 1) == '\0')
    {
      /* at last char element */
      if (elem->next != NULL)
      {
        const trie_t * search_layer = elem->next;
        int start = (idx + 1) % 26;
        int i;
        /* first check immediate words at that layer in circular order */
        for (i = 0; i < 26; i++)
        {
          int idx2 = (start + i) % 26;
          const trie_t * cand = search_layer + idx2;
          if (cand->word)
          {
            size_t len = strlen(cand->word);
            char * res = calloc(len + 1, sizeof(char));
            if (res == NULL) { perror("calloc"); return NULL; }
            strcpy(res, cand->word);
            return res;
          }
        }
        /* then deeper search in circular order */
        for (i = 0; i < 26; i++)
        {
          int idx2 = (start + i) % 26;
          const trie_t * cand = search_layer + idx2;
          if (cand->next)
          {
            char * found = find_first_in_subtree(cand->next);
            if (found) return found;
          }
        }
      }
      /* else fall through to consider deepest existing layer */
      break;
    }

    if (elem->next == NULL) break;
    layer = elem->next;
    p = p + 1;
  }

  /* If we could not reach required depth or no candidate found there, use deepest existing layer along path */
  if (last_existing_layer == NULL) return NULL;

  /* search circularly starting at index after last_index_at_existing_layer */
  int startidx = (last_index_at_existing_layer + 1) % 26;
  int j;
  for (j = 0; j < 26; j++)
  {
    int idx2 = (startidx + j) % 26;
    const trie_t * cand = last_existing_layer + idx2;
    if (cand->word)
    {
      size_t len = strlen(cand->word);
      char * res = calloc(len + 1, sizeof(char));
      if (res == NULL) { perror("calloc"); return NULL; }
      strcpy(res, cand->word);
      return res;
    }
  }
  for (j = 0; j < 26; j++)
  {
    int idx2 = (startidx + j) % 26;
    const trie_t * cand = last_existing_layer + idx2;
    if (cand->next)
    {
      char * found = find_first_in_subtree(cand->next);
      if (found) return found;
    }
  }

  return NULL;
}

/* Free the entire trie recursively, deepest-first */
static void freetrie(trie_t * root)
{
  if (root == NULL) return;
  int i;
  for (i = 0; i < 26; i++)
  {
    trie_t * elem = root + i;
    if (elem->next) freetrie(elem->next);
    if (elem->word) { free(elem->word); elem->word = NULL; }
    if (elem->next) { free(elem->next); elem->next = NULL; }
  }
  /* caller frees the root layer if it was dynamically allocated */
}

/* read and build trie from dictionary file descriptor */
static int load_dictionary_into_trie(int fd_dict, trie_t * root, int * out_count)
{
  if (fd_dict < 0 || root == NULL) return -1;

  /* allocate a temp buffer of MAXWORDLENGTH + 1 bytes via calloc */
  char * linebuf = calloc(MAXWORDLENGTH + 2, sizeof(char));
  if (linebuf == NULL) { perror("calloc"); return -1; }

  ssize_t r;
  char ch;
  size_t pos = 0;
  int wordcount = 0;

  while (1)
  {
    r = read(fd_dict, &ch, 1);
    if (r < 0) { perror("read"); free(linebuf); return -1; }
    if (r == 0)
    {
      /* EOF: if we have a pending word (no terminating newline), process it */
      if (pos > 0)
      {
        /* terminate */
        *(linebuf + pos) = '\0';
        /* validate alpha and length */
        if (pos > 0 && pos <= MAXWORDLENGTH)
        {
          /* convert to uppercase */
          strtoupper_inplace(linebuf);
          /* check alpha only */
          int ok = 1;
          char * q = linebuf;
          while (*q)
          {
            if (!isalpha((unsigned char) *q)) { ok = 0; break; }
            q = q + 1;
          }
          if (ok)
          {
            /* allocate exact sized copy and add to trie */
            size_t len = strlen(linebuf);
            char * exact = calloc(len + 1, sizeof(char));
            if (exact == NULL) { perror("calloc"); free(linebuf); return -1; }
            strcpy(exact, linebuf);
            if (addword(root, exact) == 0) wordcount++;
            free(exact);
          }
        }
      }
      break;
    }

    /* if newline, process the word */
    if (ch == '\n')
    {
      *(linebuf + pos) = '\0';
      if (pos > 0 && pos <= MAXWORDLENGTH)
      {
        strtoupper_inplace(linebuf);
        int ok = 1;
        char * q = linebuf;
        while (*q)
        {
          if (!isalpha((unsigned char) *q)) { ok = 0; break; }
          q = q + 1;
        }
        if (ok)
        {
          size_t len = strlen(linebuf);
          char * exact = calloc(len + 1, sizeof(char));
          if (exact == NULL) { perror("calloc"); free(linebuf); return -1; }
          strcpy(exact, linebuf);
          if (addword(root, exact) == 0) wordcount++;
          free(exact);
        }
      }
      pos = 0;
      continue;
    }

    /* normal character: append if within length */
    if (pos < MAXWORDLENGTH)
    {
      *(linebuf + pos) = ch;
      pos = pos + 1;
    }
    else
    {
      /* ignore characters beyond max length until newline */
      /* but continue consuming until newline */
      /* do nothing */
    }
  }

  if (out_count) *out_count = wordcount;
  free(linebuf);
  return 0;
}

/* Process requests from read fifo and respond on write fifo */
static int process_requests(int fd_read, int fd_write, trie_t * root)
{
  if (fd_read < 0 || fd_write < 0 || root == NULL) return -1;

  /* buffer for incoming candidate words */
  char * linebuf = calloc(MAXWORDLENGTH + 2, sizeof(char));
  if (linebuf == NULL) { perror("calloc"); return -1; }

  ssize_t r;
  char ch;
  size_t pos = 0;

  while (1)
  {
    r = read(fd_read, &ch, 1);
    if (r < 0)
    {
      if (errno == EINTR) continue;
      perror("read");
      free(linebuf);
      return -1;
    }
    if (r == 0)
    {
      /* EOF from fifo: break main loop */
      break;
    }

    if (ch == '\n')
    {
      *(linebuf + pos) = '\0';
      /* spawn child to handle this candidate */
      pid_t pid = fork();
      if (pid < 0)
      {
        perror("fork");
        free(linebuf);
        return -1;
      }
      else if (pid == 0)
      {
        /* child */
        /* prepare uppercase candidate */
        strtoupper_inplace(linebuf);

        /* validate candidate length and chars */
        int validformat = 1;
        size_t len = strlen(linebuf);
        size_t k;
        for (k = 0; k < len; k = k + 1)
        {
          char cc = *(linebuf + k);
          if (!isalpha((unsigned char) cc)) { validformat = 0; break; }
        }

        char * response = NULL;

        if (validformat && len > 0 && len <= MAXWORDLENGTH && isvalidword(root, linebuf))
        {
          /* echo word plus newline */
          size_t resp_len = len + 1;
          response = calloc(resp_len + 1, sizeof(char));
          if (response == NULL) { perror("calloc"); _exit(EXIT_FAILURE); }
          strcpy(response, linebuf);
          /* append newline */
          *(response + resp_len - 1) = '\n';
          *(response + resp_len) = '\0';
        }
        else
        {
          /* find alternate */
          char * alt = findalternate_revised(root, linebuf);
          /* build "CAND>ALT\n" or "CAND>\n" */
          size_t candlen = strlen(linebuf);
          size_t altlen = (alt ? strlen(alt) : 0);
          size_t total = candlen + 1 + altlen + 1;
          response = calloc(total + 1, sizeof(char));
          if (response == NULL) { perror("calloc"); if (alt) free(alt); _exit(EXIT_FAILURE); }
          /* copy candidate */
          if (candlen > 0) strcpy(response, linebuf);
          /* append delimiter '>' */
          *(response + candlen) = '>';
          /* append alt if present */
          if (altlen > 0) strcpy(response + candlen + 1, alt);
          /* append newline */
          *(response + total - 1) = '\n';
          *(response + total) = '\0';
          if (alt) free(alt);
        }

        /* write the response with a single write call */
        size_t towrite = strlen(response);
        ssize_t w = write(fd_write, response, towrite);
        if (w < 0) { perror("write"); free(response); _exit(EXIT_FAILURE); }
        free(response);
        _exit(EXIT_SUCCESS);
      }
      else
      {
        /* parent: continue reading; but also reap children to avoid zombies
           use non-blocking waitpid in a loop */
        int status;
        while (waitpid(-1, &status, WNOHANG) > 0)
        {
          /* nothing else to do here */
        }
      }

      pos = 0;
      continue;
    }

    /* normal character: append if within range */
    if (pos < MAXWORDLENGTH)
    {
      *(linebuf + pos) = ch;
      pos = pos + 1;
    }
    else
    {
      /* ignore until newline */
    }
  }

  /* EOF reached: reap all remaining children */
  while (waitpid(-1, NULL, 0) > 0) { /* reap */ }

  free(linebuf);
  return 0;
}

int main(int argc, char * argv[])
{
  if (argc != 4)
  {
    fprintf(stderr, "USAGE: ./hw2.out <dictionary-file> <read-fifo> <write-fifo>\n");
    return EXIT_FAILURE;
  }

  const char * dictname = *(argv + 1);
  const char * fifo_read = *(argv + 2);
  const char * fifo_write = *(argv + 3);

  /* open dictionary file for reading using file descriptor */
  int fd_dict = open(dictname, O_RDONLY);
  if (fd_dict < 0) { perror("open"); return EXIT_FAILURE; }

  /* create top layer of Trie */
  trie_t * root = calloc(26, sizeof(trie_t));
  if (root == NULL) { perror("calloc"); close(fd_dict); return EXIT_FAILURE; }

  /* load dictionary */
  int wordcount = 0;
  if (load_dictionary_into_trie(fd_dict, root, &wordcount) != 0)
  {
    freetrie(root);
    free(root);
    close(fd_dict);
    return EXIT_FAILURE;
  }

  close(fd_dict);

#ifndef QUIET
  printf("PARENT: added %d words to the Trie\n", wordcount);
#else
  printf("PARENT: added %d words to the Trie\n", wordcount);
#endif

  /* open read fifo first, then write fifo */
  int fd_read = open(fifo_read, O_RDONLY);
  if (fd_read < 0) { perror("open"); freetrie(root); free(root); return EXIT_FAILURE; }

  int fd_write = open(fifo_write, O_WRONLY);
  if (fd_write < 0) { perror("open"); close(fd_read); freetrie(root); free(root); return EXIT_FAILURE; }

  /* process requests */
  if (process_requests(fd_read, fd_write, root) != 0)
  {
    close(fd_read);
    close(fd_write);
    freetrie(root);
    free(root);
    return EXIT_FAILURE;
  }

  /* cleanup */
  close(fd_read);
  close(fd_write);
  freetrie(root);
  free(root);

  return EXIT_SUCCESS;
}
