#include <string.h>
#include "markov_chain.h"

/********************************** MACROS **********************************/

// CLI arguments
#define ARGS_NUM_USAGE_MSG "Usage: invalid number of arguments\n"
#define ARGS_NUM 3

// Finding the maximum of 2 numbers
#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

// Board constants
#define BOARD_SIZE 100
#define EMPTY (-1)
#define NUM_OF_TRANSITIONS 20

// Game constants
#define MAX_GENERATION_LENGTH 60
#define DICE_MAX 6

// Numeral system for the function 'strrol()'
#define NUMERAL_SYSTEM 10

/************************** CONSTANTS AND STRUCTS ***************************/

/**
 * This global variable represents the transitions by ladders and snakes in
 * the game. Each tuple (x,y) represents a ladder from x to y if x<y, or a
 * snake, otherwise.
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
 * This struct that represents a Cell in the game board
 * @member number - Cell number 1-100.
 * @member ladder_to - represents the jump of the ladder in case there is
 * one from this square (-1 if the Cell doesn't have).
 * @member snake_to - snake_to represents the jump of the snake in case
 * there is one from this square (-1 if the Cell doesn't have).
 */
typedef struct Cell
{
    int number;
    int ladder_to;
    int snake_to;
} Cell;

/**************************** ARGUMENTS FUNCTION ****************************/

/**
 * This function update the values of variables according to the given
 * arguments from CLI.
 * @param argc - number of all given arguments.
 * @param argv - array of all given arguments.
 * @param seed - a pointer to seed variable.
 * @param walks_to_create - a pointer to walks_to_create variable.
 * @return - EXIT_SUCCESS if number of arguments is 2; else EXIT_FAILURE.
 */
static int update_arguments (int argc, char *argv[], size_t *seed, int
*walks_to_create)
{
  if (argc != ARGS_NUM)
  {
    printf (ARGS_NUM_USAGE_MSG);
    return EXIT_FAILURE;
  }
  *seed = strtol (argv[1], NULL, NUMERAL_SYSTEM);
  *walks_to_create = (int) strtol (argv[2], NULL, NUMERAL_SYSTEM);
  return EXIT_SUCCESS;
}

/******************** FUNCTIONS OF SNAKES & LADDERS TYPE ********************/

/**
 * This function prints a single cell (extra detail if needed) using 'printf'.
 * @param cell - a cell to be printed.
 */
static void print_func (const void *cell)
{
  Cell *p_cell = (Cell *) cell;
  int cell_number = p_cell->number;
  fprintf (stdout, "[%d]", cell_number);
  if (cell_number != BOARD_SIZE)
  {
    if (p_cell->snake_to != EMPTY)
    {
      fprintf (stdout, "-snake to %d", p_cell->snake_to);
    }
    if (p_cell->ladder_to != EMPTY)
    {
      fprintf (stdout, "-ladder to %d", p_cell->ladder_to);
    }
    fprintf (stdout, " -> ");
  }
}

/**
 * This function compares two cells (by their numbers).
 * @param data1 - first cell.
 * @param data2 - second cell.
 * @return - a positive value if the first is bigger.
 *         - a negative value if the second is bigger.
 *         - 0 if equal.
 */
static int comp_func (const void *cell1, const void *cell2)
{
  return (((Cell *) cell1)->number - ((Cell *) cell2)->number);
}

/**
 * This function frees a cell (Cell* type) using 'free'.
 * @param cell - a cell to be free.
 */
static void free_data (void *cell)
{
  free (((Cell *) cell));
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

/**
 * This function checks if a string is a last word (last character is a dot).
 * @param data - the word to be checked.
 * @return - true or false;
 */
static bool is_last (const void *cell)
{
  return (((Cell *) cell)->number == BOARD_SIZE);
}

/************************** ERROR HANDLER FUNCTION **************************/

static int handle_error (char *error_msg, MarkovChain **database)
{
  printf ("%s", error_msg);
  if (database != NULL)
  {
    free_database (database);
  }
  return EXIT_FAILURE;
}

/**************************** DATABASE FUNCTIONS ****************************/

/**
 * This function creates a Snakes & Ladders board.
 * @param cells - a dynamic array (board) to be filled.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
static int create_board (Cell *cells[BOARD_SIZE])
{
  // Filling the board with cells
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
  // Updating snakes and ladders in the board
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
 * This function fills the board game's database.
 * @param markov_chain - the MarkovChain to fill its database.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
static int fill_database (MarkovChain *markov_chain)
{
  // Adding all cells to database
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
  // Updating frequencies of each cell
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    from_node = get_node_from_database (markov_chain, cells[i])->data;
    if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
    {
      index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
      to_node = get_node_from_database (markov_chain, cells[index_to])
          ->data;
      add_node_to_frequencies_list (from_node, to_node, markov_chain);
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
        add_node_to_frequencies_list (from_node, to_node, markov_chain);
      }
    }
  }
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    free (cells[i]);
  }
  return EXIT_SUCCESS;
}

static void print_walks (MarkovChain *markov_chain, int walks_to_create)
{
  for (int i = 1; i <= walks_to_create; i++)
  {
    MarkovNode *first_cell = markov_chain->database->first->data;
    fprintf (stdout, "Random Walk %d: ", i);
    generate_tweet (markov_chain, first_cell, MAX_GENERATION_LENGTH);
  }
}

/****************************** MAIN FUNCTION *******************************/

/**
 * This function run the program 'snakes_and_ladders'.
 * @param argc - number of all given arguments.
 * @param argv - (1) seed value
 *               (2) number of paths to generate.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
int main (int argc, char *argv[])
{
  // Updating arguments
  size_t seed = 0;
  int walks_to_create = 0;
  if (update_arguments (argc, argv, &seed, &walks_to_create) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }
  // Creating database
  MarkovChain *markov_chain = create_markov_chain (print_func, copy_func,
                                                   free_data, comp_func,
                                                   is_last);
  if (!markov_chain)
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  if (fill_database (markov_chain) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }
  // Printing random walks
  srand (seed);
  print_walks (markov_chain, walks_to_create);
  free_database (&markov_chain);
  return EXIT_SUCCESS;
}
