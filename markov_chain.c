#include "markov_chain.h"
Node *enter_empty_database (MarkovChain *markov_chain, void *data_ptr);
// see all documentation in the header file
//*********************initializers***************************//
MarkovNode *create_markov_node (void *data_ptr, copy_func_ptr copy_func)
{
  MarkovNode *new_node = (MarkovNode *) malloc (sizeof (MarkovNode));
  if (new_node == NULL || data_ptr == NULL)
  {
    return NULL; // error: failed to allocate memory
  }
  void *dup = copy_func (data_ptr);
  if (dup == NULL)
  {
    free (new_node);
    return NULL;
  }
  new_node->data = dup;
  new_node->frequencies_list = NULL;
  new_node->current_successive_nodes = 0;
  return new_node;
}
LinkedList *create_linked_list (Node *first, Node *last, int size)
{
  LinkedList *list = malloc (sizeof (LinkedList));
  if (list == NULL)
  {
    return NULL;
  }
  list->first = first;
  list->last = last;
  list->size = size;
  return list;
}
//*****************adders************************************//
Node *add_to_database (MarkovChain *markov_chain, void *data_ptr)
{
  // check if the database is not NULL, if it does we create a database and
  // then add the MarkovNode
  if (markov_chain->database == NULL)
  {
    Node *new = enter_empty_database (markov_chain, data_ptr);
    if (new == NULL)
    {
      return NULL;
    }
    return new;

  }
  Node *check = get_node_from_database (markov_chain, data_ptr);
  //checks if the element is already in the database.
  if (check == NULL)
  {
    MarkovNode *new_markov_node = create_markov_node (data_ptr,
                                                      markov_chain->copy_func);
    if (new_markov_node == NULL)
    {
      return NULL;
    }
    int add_try = add (markov_chain->database, new_markov_node);
    if (add_try == 1)
    {
      free_markov_node (new_markov_node, markov_chain);
      free_database (&markov_chain);
    }
    return markov_chain->database->last;
  }
  return check;

}
Node *enter_empty_database (MarkovChain *markov_chain, void *data_ptr)
{
  LinkedList *list = create_linked_list (NULL, NULL, 0);
  if (list == NULL)
  {
    return NULL;
  }
  markov_chain->database = list;
  MarkovNode *new_markov_node = create_markov_node (data_ptr,
                                                    markov_chain->copy_func);
  if (new_markov_node == NULL)
  {
    free (list);
    return NULL;
  }
  int add_try = add (list, new_markov_node);
  if (add_try == 1)
  {

    free_markov_node (new_markov_node, markov_chain);
    free_database (&markov_chain);
  }
  return markov_chain->database->first;

}
Node *get_node_from_database (MarkovChain *markov_chain, void *data_ptr)
{
  if (markov_chain->database == NULL || markov_chain->database->first == NULL)
  {
    return NULL;
  }
  Node *current = markov_chain->database->first;
  // searching for a node contain the same word, if ones exist return it.
  while (current != NULL)
  {

    if (markov_chain->comp_func (current->data->data, data_ptr) == 0)
    {
      return current;
    }
    current = current->next;
  }
  return NULL;
}
MarkovChain *create_markov_chain (print_func_ptr print_func, copy_func_ptr
copy_func, free_func_ptr free_func, cmp_func_ptr cmp_func, is_last_func_ptr
                                  is_last)
{
  MarkovChain *new_chain = malloc (sizeof (MarkovChain));
  if (new_chain == NULL)
  {
    return NULL;
  }
  new_chain->database = NULL;
  new_chain->print_func = print_func;
  new_chain->free_data = free_func;
  new_chain->copy_func = copy_func;
  new_chain->comp_func = cmp_func;
  new_chain->is_last = is_last;
  return new_chain;
}
//*********************add to freq list and helpers ***************//
MarkovNodeFrequency *create_frequency_array (MarkovNode *first_node,
                                             MarkovNode *second_node)
{
  MarkovNodeFrequency *new_frequency_array = malloc
      (sizeof (MarkovNodeFrequency));
  if (new_frequency_array == NULL)
  {
    return NULL;
  }
  first_node->frequencies_list = new_frequency_array;
  first_node->frequencies_list[first_node->current_successive_nodes].next =
      second_node;
  first_node->frequencies_list[first_node->current_successive_nodes].frequency
      = 1;
  first_node->current_successive_nodes++;
  return new_frequency_array;

}
MarkovNodeFrequency *expand_frequency_array (MarkovNode *first_node,
                                             MarkovNode *second_node)
{
  MarkovNodeFrequency *new_frequency_list = realloc (first_node->
      frequencies_list, (first_node->current_successive_nodes + 1)
                        * sizeof (MarkovNodeFrequency));
  if (new_frequency_list == NULL)
  {
    return NULL;
  }
  // Add second_node to the end of the frequency list and set its frequency
  // to 1
  first_node->frequencies_list = new_frequency_list;
  first_node->frequencies_list[first_node->current_successive_nodes].next =
      second_node;
  first_node->frequencies_list[first_node->current_successive_nodes].frequency
      = 1;
  first_node->current_successive_nodes++;
  return new_frequency_list;
}
bool add_node_to_frequencies_list (MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain)
{
  if (!first_node || !second_node)
  {
    return false;
  }
  if (first_node->frequencies_list == NULL)
  {
    MarkovNodeFrequency *new_frequency_array = create_frequency_array
        (first_node, second_node);
    if (new_frequency_array == NULL)
    {
      return false;
    }
    return true;

  }
  // Check if second_node is already in the frequency list of first_node
  for (int i = 0; i < first_node->current_successive_nodes; i++)
  {
    if (!markov_chain->comp_func (first_node->frequencies_list[i].next->data,
                                  second_node->data))
    {
      // If so, increment its frequency and return success
      first_node->frequencies_list[i].frequency++;
      return true;
    }
  }
  // If second_node is not in the frequency list, allocate new memory for it
  MarkovNodeFrequency *new_frequency_list = expand_frequency_array
      (first_node, second_node);
  if (new_frequency_list == NULL)
  {
    return false;
  }

  return true;

}
//**********************free functions**********************************//
void free_markov_node (MarkovNode *markov_node, MarkovChain *ptr_chain)
{
  free (markov_node->frequencies_list);
  markov_node->frequencies_list = NULL;
  ptr_chain->free_data ((markov_node->data));
  markov_node->data = NULL;
  free (markov_node);
  markov_node = NULL;
}
void free_database (MarkovChain **markov_chain)
{
  // if the pointer is null or the data inside is null exit.
  if (markov_chain == NULL || *markov_chain == NULL)
  {
    return;
  }
  LinkedList *database = (*markov_chain)->database;
  // if database is null just free the chain.
  if (database == NULL)
  {
    free (*markov_chain);
    *markov_chain = NULL;
    return;
  }
  Node *current = database->first;
  // for each node in the database we free the frequency list of the
  // markovnode of the node first. Then we free the markovnode in the data
  // field of the node itself, and finally we free the node. then moving to
  // the next node.
  while (current != NULL)
  {
    MarkovNode *markov_node = current->data;
    free_markov_node (markov_node, *markov_chain);
    Node *next = current->next;
    free_node (current);
    current = next;
  }
  free ((*markov_chain)->database);
  (*markov_chain)->database = NULL;
  free (*markov_chain);
  *markov_chain = NULL;
}
void free_node (Node *current)
{
  free (current);
  current = NULL;
}
//***********random chain generators*****************************//
MarkovNode *get_first_random_node (MarkovChain *markov_chain)
{
  // checks if the pointer or the data inside is valid
  if ((markov_chain == NULL) || (markov_chain->database == NULL) ||
      (markov_chain->database->first == NULL) || (
          (markov_chain->database->first->data == NULL)))
  {
    return NULL;
  }
  Node *current = markov_chain->database->first;

  while (true)
  {
    current = markov_chain->database->first;
    int i = get_random_number (markov_chain->database->size);
    for (int j = 0; j < i; j++)
    {
      current = current->next;
    }
    if (current == NULL)
    {
      return NULL;
    }
    if (!(markov_chain->is_last (current->data->data)))
    {
      break;
    }

  }

  return current->data;
}
MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr)
{
  // Check if the state_struct_ptr or its frequencies_list is NULL
  if (state_struct_ptr == NULL || state_struct_ptr->frequencies_list == NULL)
  {
    return NULL;
  }
  int total_occurrences = 0;
  for (int i = 0; i < state_struct_ptr->current_successive_nodes; i++)
  {
    total_occurrences += state_struct_ptr->frequencies_list[i].frequency;
  }

  // Generate a random number within the range of total_occurrences
  int random_number = get_random_number (total_occurrences);

  // Iterate through the frequencies_list to find the word corresponding
  // to the random_number
  int cumulative_occurrences = 0;
  for (int i = 0; i < state_struct_ptr->current_successive_nodes; i++)
  {
    cumulative_occurrences += state_struct_ptr->frequencies_list[i].frequency;
    if (random_number < cumulative_occurrences)
    {
      return state_struct_ptr->frequencies_list[i].next;
    }
  }

  // If no word is selected, return NULL
  return NULL;
}
void generate_tweet (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length)
{
  // checks if the pointer or the data inside is valid
  if ((markov_chain == NULL) || (markov_chain->database == NULL) ||
      (markov_chain->database->first == NULL) ||
      (markov_chain->database->first->data == NULL)
      )
  {
    return;
  }
  if (first_node == NULL)
  {
    first_node = get_first_random_node (markov_chain);
  }
  MarkovNode *current = first_node;
  while ((max_length > 1) && (current->current_successive_nodes > 0) && (!
      (markov_chain->is_last
          (current->data))))
  {
    markov_chain->print_func (current->data);
    current = get_next_random_node (current);

    max_length--;
  }
  markov_chain->print_func (current->data);

}
int get_random_number (int max_number)
{

  return rand () % max_number;
}