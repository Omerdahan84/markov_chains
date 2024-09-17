#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY (-1)
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

#define NUMBER_OF_ARGUNEBTS 3
#define INT_BASE 10
/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell
{
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of the ladder in case
    // there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in case
    // there is one from this square
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

/** Error handler **/
static int handle_error (char *error_msg, MarkovChain **database)
{
  printf ("%s", error_msg);
  if (database != NULL)
  {
    free_database (database);
  }
  return EXIT_FAILURE;
}
//==============generator==============//
static void paths_generator (MarkovChain *markov_chain, int number_of_paths)
{
  int i = 1;
  while (number_of_paths >= i)
  {
    MarkovNode *first_cell = markov_chain->database->first->data;
    printf ("Random Walk %d: ", i);
    generate_tweet (markov_chain, first_cell,
                    MAX_GENERATION_LENGTH);
    printf ("\n");
    i++;
  }
}
//===========struct funcs============//
/**
 * this function frees the memory of the cell
 * @param pointer
 */
static void free_func (void *pointer)
{
  free (((Cell *) pointer));
}
/**
 * return true if we its last cell and false else.
 * cell defined to be last if its number field is 100
 */
static bool is_last (const void *data)
{
  return (((Cell *) data)->number == BOARD_SIZE);
}
static int comp_func (const void *data1, const void *data2)
{
  return (((Cell *) data1)->number - ((Cell *) data2)->number);

}
/**
 * This function creates a copy of a cell using 'memcpy'.
 * @param source - a source cell to be copied.
 * @return - a copy of 'source'.
 */
static void *copy_func (const void *source)
{
  char *p_src = (char *) source;
  void *dest = malloc (sizeof (Cell));
  if (!dest)
  {
    return NULL;
  }
  memcpy (dest, p_src, sizeof (Cell));
  return dest;
}
static void print_func (const void *data)
{
  Cell *cell = (Cell *) data;

  printf ("[%d]", cell->number);
  if (cell->number != BOARD_SIZE)
  {
    if (cell->snake_to != EMPTY)
    {
      fprintf (stdout, "-snake to %d", cell->snake_to);
    }
    if (cell->ladder_to != EMPTY)
    {
      printf ("-ladder to %d", cell->ladder_to);
    }
    printf (" -> ");
  }
}

//=====================================//
static int create_board (Cell *cells[BOARD_SIZE])
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    cells[i] = malloc (sizeof (Cell));
    if (cells[i] == NULL)
    {
      for (int j = 0; j < i; j++)
      {
        free (cells[j]);
      }
      handle_error (ALLOCATION_ERROR_MASSAGE, NULL);
      return EXIT_FAILURE;
    }
    *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
  }

  for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
  {
    int from = transitions[i][0];
    int to = transitions[i][1];
    if (from < to)
    {
      cells[from - 1]->ladder_to = to;
    }
    else
    {
      cells[from - 1]->snake_to = to;
    }
  }
  return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database (MarkovChain *markov_chain)
{
  Cell *cells[BOARD_SIZE];
  if (create_board (cells) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }
  MarkovNode *from_node = NULL, *to_node = NULL;
  size_t index_to;
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    add_to_database (markov_chain, cells[i]);
  }

  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    from_node = get_node_from_database (markov_chain, cells[i])->data;

    if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
    {
      index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
      to_node = get_node_from_database (markov_chain, cells[index_to])
          ->data;
      add_node_to_frequencies_list (from_node, to_node,
                                    markov_chain);
    }
    else
    {
      for (int j = 1; j <= DICE_MAX; j++)
      {
        index_to = ((Cell *) (from_node->data))->number + j - 1;
        if (index_to >= BOARD_SIZE)
        {
          break;
        }
        to_node = get_node_from_database (markov_chain, cells[index_to])
            ->data;
        add_node_to_frequencies_list (from_node, to_node,
                                      markov_chain);
      }
    }
  }
  // free temp arr
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    free (cells[i]);
  }
  return EXIT_SUCCESS;
}

/**
 * this function checks if the number of the arguments the user entered is
 * valid
 * @param argc
 * @return
 */
static _Bool check_number_arguments (int argc)
{
  if (argc != NUMBER_OF_ARGUNEBTS)
  {
    printf ("USAGE:./program_name seed_value(unsigned_int) "
            "number_of_paths(int)\n");
    return false;
  }
  return true;
}
/**
 * this function updates the seed and number of paths values according to
 * the user inputs
 * @param: seed , num_paths, argv
 */
static void update_argumantes (size_t *seed, int *num_paths, char *argv[])
{
  *seed = strtol (argv[1], NULL, INT_BASE);
  *num_paths = (int) strtol (argv[2], NULL, INT_BASE);
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main (int argc, char *argv[])
{
  if (!check_number_arguments (argc))
  {
    return EXIT_FAILURE;
  }
  size_t seed;
  int num_paths;
  update_argumantes (&seed, &num_paths, argv);
  MarkovChain *markov_chain = create_markov_chain (print_func, copy_func,
                                                   free_func,
                                                   comp_func, is_last);
  if (markov_chain == NULL)
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  if (fill_database (markov_chain) == EXIT_FAILURE)
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    free (markov_chain);
    return EXIT_FAILURE;
  }
  srand (seed);
  paths_generator (markov_chain, num_paths);
  free_database (&markov_chain);
  return EXIT_SUCCESS;
}
