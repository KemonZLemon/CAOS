#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define MAXWORDLENGTH 32

typedef struct trie
{
  char *word;
  struct trie *next;
} trie_t;

/* Utility: convert a character into Trie index (0–25), else -1 */
int char_index(char c)
{
  int idx = c - 'A';
  if (idx < 0 || idx >= 26)
    return -1;
  return idx;
}

/* Convert all chars in word to uppercase */
void toupper_inplace(char *word, int len)
{
  char *p = word;
  int i = 0;
  while (i < len)
  {
    *p = (char)toupper((unsigned char)*p);
    ++p;
    ++i;
  }
}

/* Print all words stored in Trie */
void print_trie(const trie_t *node)
{
  if (node == NULL)
    return;

  const trie_t *curr = node;
  int i = 0;
  while (i < 26)
  {
    if ((*(curr + i)).word)
      printf("%s\n", (*(curr + i)).word);

    if ((*(curr + i)).next)
      print_trie((*(curr + i)).next);
    ++i;
  }
}

/* Recursive free of Trie (deepest first) */
void free_trie(trie_t *node)
{
  if (node == NULL)
    return;

  trie_t *curr = node;
  int i = 0;
  while (i < 26)
  {
    if ((*(curr + i)).next)
      free_trie((*(curr + i)).next);
    if ((*(curr + i)).word)
      free((*(curr + i)).word);
    ++i;
  }
  free(node);
}

/* Insert a word into the Trie */
void insert_word(trie_t *root, char *word)
{
  trie_t *level = root;
  int i = 0;
  char ch;

  while (*(word + i) != '\0')
  {
    ch = *(word + i);
    int idx = char_index(ch);
    if (idx < 0)
      return;

    /* end of word */
    if (*(word + i + 1) == '\0')
    {
      if ((*(level + idx)).word == NULL)
      {
        char *copy = calloc(strlen(word) + 1, sizeof(char));
        strcpy(copy, word);
        (*(level + idx)).word = copy;
      }
    }
    else
    {
      if ((*(level + idx)).next == NULL)
      {
#ifndef QUIET
        printf("PARENT: creating new Trie layer for '%c' of \"%s\"\n", ch, word);
#endif
        (*(level + idx)).next = calloc(26, sizeof(trie_t));
        if ((*(level + idx)).next == NULL)
        {
          perror("calloc() failed");
          return;
        }
      }
      level = (*(level + idx)).next;
    }
    ++i;
  }
}

/* Find suggested alternate word */
char *find_suggestion(trie_t *root, const char *word, int length)
{
  trie_t *curr = root;
  char *result = NULL;

  for (int i = 0; i < length; ++i)
  {
    char c = *(word + i);
    int idx = char_index(c);
    if (idx < 0)
      break;

    char *candidate = NULL;

    /* look forward circularly */
    for (int j = idx; j < 26 && candidate == NULL; ++j)
    {
      if ((*(curr + j)).word != NULL)
        candidate = (*(curr + j)).word;
    }

    if (candidate == NULL)
    {
      for (int j = 0; j < idx; ++j)
      {
        if ((*(curr + j)).word != NULL)
        {
          candidate = (*(curr + j)).word;
          break;
        }
      }
    }

    if (candidate != NULL)
      result = candidate;

    if ((*(curr + idx)).next == NULL || i + 1 == length)
      break;
    curr = (*(curr + idx)).next;
  }

  return result;
}

/* Check if a word is valid recursively */
bool check_word(trie_t *root, char *word, int idx)
{
  if (root == NULL)
    return false;

  char c = *(word + idx);
  int pos = char_index(c);
  if (pos < 0)
    return false;

  int len = strlen(word);
  if (idx == len - 1)
    return (*(root + pos)).word != NULL;

  if ((*(root + pos)).next == NULL)
    return false;

  return check_word((*(root + pos)).next, word, idx + 1);
}

/* Wait for a specific or any child process */
void wait_for(pid_t pid)
{
  int status;
  waitpid(pid, &status, 0);
}

int main(int argc, char **argv)
{
  setvbuf(stdout, NULL, _IONBF, 0);
  if (argc != 4)
  {
    fprintf(stderr, "USAGE: ./hw2.out <dictionary-file> <read-fifo> <write-fifo>\n");
    return EXIT_FAILURE;
  }

  int fd = open(*(argv + 1), O_RDONLY);
  if (fd < 0)
  {
    perror("open");
    return EXIT_FAILURE;
  }

  int total_words = 0;
  int read_bytes;
  bool skip = false;

  char *temp = calloc(MAXWORDLENGTH + 1, sizeof(char));
  char *buffer = calloc(1024, sizeof(char));
  trie_t *trie = calloc(26, sizeof(trie_t));

  if (temp == NULL || buffer == NULL || trie == NULL)
  {
    perror("calloc() failed");
    free(temp);
    free(buffer);
    free(trie);
    close(fd);
    return EXIT_FAILURE;
  }

  int wordlen = 0;
  while ((read_bytes = read(fd, buffer, 1024)) > 0)
  {
    char *p = buffer;
    while (p < buffer + read_bytes)
    {
      char c = *p;
      if (c == '\r') { ++p; continue; }

      if (c == '\n')
      {
        if (!skip && wordlen > 0)
        {
          *(temp + wordlen) = '\0';
          toupper_inplace(temp, wordlen);
          insert_word(trie, temp);
          ++total_words;
        }
        wordlen = 0;
        skip = false;
        ++p;
        continue;
      }

      if (skip) { ++p; continue; }

      if (!isalpha((unsigned char)c))
      {
        skip = true;
        wordlen = 0;
        ++p;
        continue;
      }

      if (wordlen >= MAXWORDLENGTH)
      {
        skip = true;
        wordlen = 0;
        ++p;
        continue;
      }

      *(temp + wordlen) = c;
      ++wordlen;
      ++p;
    }
  }

  if (!skip && wordlen > 0)
  {
    *(temp + wordlen) = '\0';
    toupper_inplace(temp, wordlen);
    insert_word(trie, temp);
    ++total_words;
  }

  close(fd);

  char *fifo_in = *(argv + 2);
  char *fifo_out = *(argv + 3);

  int fd_in = open(fifo_in, O_RDONLY);
  if (fd_in < 0)
  {
    perror("open read fifo");
    free_trie(trie);
    free(temp);
    free(buffer);
    return EXIT_FAILURE;
  }

  int fd_out = open(fifo_out, O_WRONLY);
  if (fd_out < 0)
  {
    perror("open write fifo");
    close(fd_in);
    free_trie(trie);
    free(temp);
    free(buffer);
    return EXIT_FAILURE;
  }

  char *wordbuf = calloc(MAXWORDLENGTH + 1, sizeof(char));
  int child_count = 0;
  int pos = 0;
  bool skip_word = false;

  while (1)
  {
    int bytes = read(fd_in, buffer, 128);
    if (bytes < 0)
    {
      perror("read fifo");
      break;
    }
    if (bytes == 0)
      break;

    char *p = buffer;
    while (p < buffer + bytes)
    {
      char c = *p;
      if (c == '\r') { ++p; continue; }

      if (c == '\n')
      {
        if (!skip_word && pos > 0)
        {
          *(wordbuf + pos) = '\0';
          toupper_inplace(wordbuf, pos);
          pid_t pid = fork();

          if (pid == 0)
          {
            close(fd_in);
            char *msg = calloc(128, sizeof(char));

            if (msg == NULL)
            {
              perror("calloc() failed");
              close(fd_out);
              _exit(EXIT_FAILURE);
            }

            if (check_word(trie, wordbuf, 0))
              snprintf(msg, 128, "%s\n", wordbuf);
            else
            {
              char *alt = find_suggestion(trie, wordbuf, pos);
              snprintf(msg, 128, "%s>%s\n", wordbuf, alt ? alt : "");
            }

            if (write(fd_out, msg, strlen(msg)) == -1)
              perror("write fifo");

            free(msg);
            free_trie(trie);
            free(temp);
            free(buffer);
            free(wordbuf);
            close(fd_out);
            _exit(EXIT_SUCCESS);
          }
          ++child_count;
        }
        skip_word = false;
        pos = 0;
        ++p;
        continue;
      }

      if (!isalpha((unsigned char)c))
      {
        skip_word = true;
        pos = 0;
        ++p;
        continue;
      }

      if (pos >= MAXWORDLENGTH)
      {
        skip_word = true;
        pos = 0;
        ++p;
        continue;
      }

      *(wordbuf + pos) = c;
      ++pos;
      ++p;
    }
  }

  while (child_count-- > 0)
    wait_for(-1);

  printf("PARENT: added %d words to the Trie\n", total_words);

  close(fd_in);
  close(fd_out);
  free_trie(trie);
  free(temp);
  free(buffer);
  free(wordbuf);

  return EXIT_SUCCESS;
}
