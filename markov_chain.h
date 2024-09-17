#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "linked_list.h"
#include <stdio.h>  // For printf(), sscanf()
#include <stdlib.h> // For exit(), malloc()
#include <stdbool.h> // for bool

#define ALLOCATION_ERROR_MASSAGE "Allocation failure: Failed to allocate "\
            "new memory\n"



/***************************/
/*   insert typedefs here  */
/***************************/

// Define a typedef for a function pointer that takes a void pointer and prints
// it
typedef void (*print_func_ptr) (const void *);

// Define a typedef for a function pointer that takes two void pointers
// and compares them
// The function returns an integer value to indicate whether the first
// argument is less than, equal to, or greater than the second argument
typedef int (*cmp_func_ptr) (const void *, const void *);

// Define a typedef for a function pointer that takes a void pointer and
// frees the memory it points to
typedef void (*free_func_ptr) (void *);

// Define a typedef for a function pointer that takes a const void pointer
// and returns a newly allocated copy of it
// The function returns a void pointer that can be cast to any other pointer
// type
typedef void *(*copy_func_ptr) (const void *);

// Define a typedef for a function pointer that takes a const
// void pointer and returns a boolean value indicating whether it is the
// last item in a collection
typedef bool (*is_last_func_ptr) (const void *);


/***************************/



/***************************/
/*        STRUCTS          */
/***************************/
/**
 * this struct represent a node in the markov chain.
 */
typedef struct MarkovNode
{
    void *data;// the actual data
    struct MarkovNodeFrequency *frequencies_list;//a dynamic array to all
    // successor data possible after the current data.
    int current_successive_nodes;
} MarkovNode;

typedef struct MarkovNodeFrequency
/**
 *represent the frequency of a successor word after specific word according
 * to the data.
 */
{
    MarkovNode *next; // the MarkovNode of the successor node.
    int frequency;// the frequency of the successor node after the current
    // node
} MarkovNodeFrequency;

/* DO NOT ADD or CHANGE variable names in this struct */
typedef struct MarkovChain
{
    LinkedList *database;

    // pointer to a func that receives data from a generic type and prints it
    // returns void.
    print_func_ptr print_func;

    // pointer to a func that gets 2 pointers of generic data type(same one)
    // and compare between them */
    // returns: - a positive value if the first is bigger
    //          - a negative value if the second is bigger
    //          - 0 if equal
    cmp_func_ptr comp_func;

    // a pointer to a function that gets a pointer of generic data type and
    // frees it.
    // returns void.
    free_func_ptr free_data;

    // a pointer to a function that  gets a pointer of generic data type and
    // returns a newly allocated copy of it
    // returns a generic pointer.
    copy_func_ptr copy_func;

    //  a pointer to function that gets a pointer of generic data type and
    //  returns:
    //      - true if it's the last state.
    //      - false otherwise.
    is_last_func_ptr is_last;
} MarkovChain;
//********************random chain generators*************//
/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode *get_first_random_node (MarkovChain *markov_chain);

/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr);

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with, if NULL- choose a random
 * markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_tweet (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length);
/**
 * Get random number between 0 and max_number [0,max_number)
 * @param max_number
 * @return Random number
 */
int get_random_number (int max_number);

//****************adders-getters**************************//
/**
* Check if data_ptr is in database. If so, return the markov_node wrapping it
 * in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */
Node *get_node_from_database (MarkovChain *markov_chain, void *data_ptr);
/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return node wrapping given data_ptr in given chain's database
 */
Node *add_to_database (MarkovChain *markov_chain, void *data_ptr);
/**
* creates new Linked list struct. The function assume the data fields are
    * valid/
* @param first
* @param last
* @param size
* @return a pointer to a new dynamically allocated Linked list, NULL pointer
* if allocation failed.
 *@attention this function allocates a memory needs to be free by the caller.
*/
LinkedList *create_linked_list (Node *first, Node *last, int size);
/**
 * This function gets a pointer to a copy function and use it to create a new
 * markov node with a the data in the data_ptr
 * @param data_ptr
 * @param copy_func
 * @return MarkovNode pointer if success(dynamically allocated),NULL if fails.
 * @attention:this function allocates and use memory need to be freed by
 * caller.
 */
MarkovNode *create_markov_node (void *data_ptr, copy_func_ptr copy_func);
/**
 * Creates a new MarkovChain with the specified function pointers.
 *
 * @param print_func    A function pointer that takes a void pointer and prints
 * it.
 * @param copy_func     A function pointer that takes a const void pointer and
 * returns a newly allocated copy of it.
 * @param free_func     A function pointer that takes a void pointer and frees
 * the memory it points to.
 * @param cmp_func      A function pointer that takes two void pointers and
 * compares them.
 *                      Returns an integer value indicating the order: less
 *                      than, equal to, or greater than.
 * @param is_last       A function pointer that takes a const void pointer
 * and returns a boolean value indicating
 *                      whether it is the last item in a collection.
 *
 * @return  A pointer to the newly created MarkovChain.
 *          Returns NULL if memory allocation fails.
 */
MarkovChain *create_markov_chain (print_func_ptr print_func, copy_func_ptr
copy_func, free_func_ptr free_func, cmp_func_ptr cmp_func, is_last_func_ptr
                                  is_last);
//****************free func************************//
/**
 * free a Markov Node and all the content inside it.
 * @param markov_node
 */
void free_markov_node (MarkovNode *markov_node, MarkovChain *ptr_chain);
/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_database (MarkovChain **markov_chain);
/**
 * free a node from a linked list
 * @param current
 */
void free_node (Node *current);
//*********************add to freq list and helpers ***************//
/**
 * Add the second markov_node to the counter list of the first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @param markov_chain
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool add_node_to_frequencies_list (MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain);
/**
 * Create a new dynamic array of Markov Node Frequency structs.
 * @param first_node,second_node
 * @return A pointer to the new allocated array if allocation succeed and
 * NULL if allocation failed.
 */
MarkovNodeFrequency *create_frequency_array (MarkovNode *first_node,
                                             MarkovNode *second_node);
/**
 * Expand an existing array of  Markov Node Frequency structs by allocating
 * more memory.
 * @param first_node
 * @param newNode
 * @return NULL if reallocation failed and the pointer to the new array if
 * allocation succeed.
 */
MarkovNodeFrequency *expand_frequency_array (MarkovNode *first_node,
                                             MarkovNode *newNode);
#endif /* MARKOV_CHAIN_H */
