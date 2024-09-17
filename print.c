
#include "markov_chain.h"
void printMarkovChain (MarkovChain *chain)
{
  printf ("MarkovChain:\n");
  Node *list = chain->database->first;
  while (list != NULL)
  {
    MarkovNode *node = (MarkovNode *) list->data;
    printf ("MarkovNode: %s\n", (char*)node->data);
    printf ("this word is %d\n",chain->is_last(node->data));
    printf ("total words after %d\n",node->current_successive_nodes);
    if (node->current_successive_nodes > 0)
    {
      printf ("\tfollowing words: ");

    }
    else
    {
      printf ("\t no following words");
    }
    for (int i = 0; i < node->current_successive_nodes; i++)
    {
      MarkovNodeFrequency *freq = &(node->frequencies_list[i]);
      printf ("%s (%d), ", (char*)freq->next->data, freq->frequency);
    }
    printf ("\n");
    list = list->next;
  }
}