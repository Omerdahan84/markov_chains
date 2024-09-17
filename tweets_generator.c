#include "linked_list.h"
#include "markov_chain.h"
#include <string.h>
#define MAX_LINE_LEN 1000
#define MAX_WORD_LEN 100
#define ARGUMENTS_WORD_COUNTER 5
#define ARGUMENTS_NO_COUNTER 4
#define READ_WHOLE_FILE (-1)
#define INT_BASE 10
#define MAX_LEN_TWEET 20


static void free_func (void *pointer);
/**
 * this function print the data inside the generic pointer.
 * @param data
 */
static void print_func (const void *data);
/**
 * takes in a word in a collection a return true if the word is last and
 * false else.
 * @param:data
 * @note: we call a word last if the last char of it its '.'
 */
static bool is_last (const void *data);
/**
 * compare the two string using strcmp.
 * @param:data1,data2
 * return:0 if equal, positive val if first one is first in alphabetic
 * order,negative number else.
 */
static int comp_func (const void *data1, const void *data2);
/**
 * Duplicates a string or data pointed to by a void pointer.
 *
 * This function takes a void pointer `source` that points to a string or
 * data and creates a dynamically allocated copy of it.
 *
 * @param source A void pointer pointing to the source string or data to
 * be duplicated.
 * @return A void pointer pointing to the dynamically allocated copy of the
 * source string or data. Returns NULL if the allocation fails.
 *
 * @note The caller is responsible for freeing the dynamically allocated
 * memory using the `free` function when it is no longer needed to avoid memory
 * leaks.
 */
static void *copy_func (const void *source);
/**
 * this number checks if the number of arguments the user entered is valid
 * to run the program.
 * @param argc
 * @return true if the number of arguments is 4 or 5 and false else.
 */
static _Bool check_number_arguments (int argc)
{
  if (argc != ARGUMENTS_WORD_COUNTER && argc != ARGUMENTS_NO_COUNTER)
  {
    printf ("USAGE:./program_name seed_value(unsigned_int) "
            "number_of_twits(int) optional- number_of_words_to_read(int)\n");
    return false;
  }
  return true;
}
/**
 * This function reads from the file the number of words the user choose to
 * read. If the number of words to read is bigger than the number of words
 * in the file,it will read the while file and won't produce and error message.
 * @param fp
 * @param word_to_read
 * @param markov_chain
 * @return 0 if database filled successfully and 1 if not.
 */
static int fill_database (FILE *fp, int word_to_read, MarkovChain
*markov_chain);
/**
 * activate the function fill database according to the user arguments.
 * @param words_from_cli
 * @param file_to_read
 * @param markov_chain
 */
static int database_with_parameters (char *words_from_cli, FILE *file_to_read,
                                     MarkovChain *markov_chain);
/**
 * updating current frequencies array with prev.
 * @param markov_chain
 * @param prev
 * @param current
 * @return 1 if the process failed 0 if finished successfully
 */
static int adding_prev (Node *prev, Node *current, MarkovChain *markov_chain);
/**
 * pharse a line from fgets into words and put them in the database
 * @param word
 * @param word_to_read
 * @param markov_chain
 * @return 1 if failed and 0 if finished successfully
 */
static int
word_phraser (char *word, Node *current, int *word_to_read,
              MarkovChain **markov_chain, Node **prev);
static void tweets_generator (MarkovChain *markov_chain, int number_of_tweets);
int main (int argc, char *argv[])
{
  if (!check_number_arguments (argc))
  {
    return EXIT_FAILURE;
  }
  FILE *file_to_read = fopen (argv[3], "r");//set file pointer
  if (file_to_read == NULL)//checks if succeed
  {
    printf ("Error: failed to open the file or path is invalid\n");
    return EXIT_FAILURE;
  }
  MarkovChain *markov_chain = create_markov_chain (print_func, copy_func,
                                                   free_func,
                                                   comp_func, is_last);
  if (markov_chain == NULL)//try to create a new markov chain and checks it
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  unsigned int seed = (unsigned int) strtol (argv[1], NULL,
                                             INT_BASE);
  srand (seed);
  int number_of_tweets = (int) strtol (argv[2], NULL,
                                       INT_BASE);
  if (argc == ARGUMENTS_WORD_COUNTER)
  {
    if (database_with_parameters (argv[4], file_to_read,
                                  markov_chain) == 1)
    {
      printf (ALLOCATION_ERROR_MASSAGE);
      free_database (&markov_chain);
      fclose (file_to_read);
      return EXIT_FAILURE;
    }
  }
  if (argc == ARGUMENTS_NO_COUNTER)
  {
    if (database_with_parameters ("-1", file_to_read,
                                  markov_chain) == 1)
    {
      printf (ALLOCATION_ERROR_MASSAGE);
      free_database (&markov_chain);
      fclose (file_to_read);
      return EXIT_FAILURE;
    }
  }
  tweets_generator (markov_chain, number_of_tweets);
  free_database (&markov_chain);
  fclose (file_to_read);
  return EXIT_SUCCESS;
}
static void tweets_generator (MarkovChain *markov_chain, int number_of_tweets)
{
  int i = 1;
  while (number_of_tweets >= i)
  {

    printf ("Tweet %d: ", i);
    generate_tweet (markov_chain, NULL, (MAX_LEN_TWEET));
    printf ("\n");
    i++;
  }
}
static int fill_database (FILE *fp, int word_to_read, MarkovChain
*markov_chain)
{

  char line[MAX_LINE_LEN];
  char *word = NULL;
  Node *prev = NULL;
  Node *current = NULL;
  while ((word_to_read > 0 || word_to_read == READ_WHOLE_FILE)
         && fgets (line, MAX_LINE_LEN, fp))
  {

    word = strtok (line, " \n\r");
    if (word_phraser (word, current, &word_to_read, &markov_chain, &prev) == 1)
    {
      return 1;
    }
  }
  return 0;
}
static int
word_phraser (char *word, Node *current, int *word_to_read, MarkovChain
**markov_chain, Node **prev)
{
  while ((word != NULL)
         && ((*word_to_read) > 0 || (*word_to_read) == READ_WHOLE_FILE))
  {
    current = add_to_database ((*markov_chain), word);

    if (current == NULL)
    {
//      free_database (markov_chain);
      return 1;
    }
    if (*prev)
    {
      //assert((*prev)->data);
      if (!(*markov_chain)->is_last ((*prev)->data->data))
      {
        if (adding_prev ((*prev), current, (*markov_chain)) == 1)// try to
          // add prev
          // to freq list of current
        {
          return 1;
        }
      }
    }
    *prev = current;
    word = strtok (NULL, " \n\r");
    if ((*word_to_read) != -1)
    {
      (*word_to_read)--;

    }
  }
  return 0;
}
static int adding_prev (Node *prev, Node *current, MarkovChain *markov_chain)
{
  if (prev != NULL)
  {
    if (!add_node_to_frequencies_list (prev->data,
                                       current->data, markov_chain))
    {
//      free_database (markov_chain);
      return 1;
    }
  }
  return 0;
}
static int database_with_parameters (char *words_from_cli, FILE *file_to_read,
                                     MarkovChain *markov_chain)
{
  int words_to_read = 0;
  if (strcmp (words_from_cli, "-1") == 0)
  {
    words_to_read = -1;
  }
  else
  {
    words_to_read = (int) strtol (words_from_cli, NULL, INT_BASE);
  }

  if (fill_database (file_to_read, words_to_read,
                     markov_chain) == 1)
  {
    return 1;
  }
  return 0;
}

//***************markov chain functions***************//
static void free_func (void *pointer)
{
  free ((char *) pointer);
}
static bool is_last (const void *data)
{

  char *str = (char *) data;
  if (str[strlen (str) - 1] == '.')
  {
    return true;
  }
  return false;
}
static int comp_func (const void *data1, const void *data2)
{
  return strcmp ((const char *) data1, (const char *) data2);

}
static void *copy_func (const void *source)
{
  const char *str = (const char *) source;
  int len = (int) strlen (str);
  char *duplicate = malloc (sizeof (char) * (len + 1));

  if (duplicate == NULL)
  {
    return NULL;
  }
  strcpy (duplicate, str);
  return (void *) duplicate;
}
static void print_func (const void *data)
{
  char *str = (char *) data;
  printf ("%s", str);
  if (!is_last (str))
  {
    printf (" ");
  }
}
