# Markov Chain Tweet Generator

## Overview
This project implements a versatile Markov chain framework with two primary applications:

- Text Analysis and Tweet Generation
- Snakes and Ladders Game Simulation

The core of the project is built using generic programming, allowing for easy adaptation to various use cases beyond the initial implementations.
Text Analysis and Tweet Generation
## Key Features:

- Analyzes input text to build Markov chains
- Estimates transition probabilities between words
- Generates logically coherent tweets based on learned patterns

## Process:

- Text Analysis: The system processes input text, breaking it down into individual words and analyzing their relationships.
- Probability Estimation: It calculates the likelihood of transitioning from one word to another based on the frequency of occurrences in the input text.
- Markov Chain Construction: These probabilities are used to build Markov chains, representing the structure and patterns of the language in the input text.
- Tweet Generation: Using the constructed Markov chains, the system generates new tweets. These tweets are random but maintain a logical structure based on the learned patterns, resulting in more coherent and contextually appropriate content.
![Alt text](/markov_chain_example.png "Sample state machine")

## Usage

./tweets <seed_value> <number_of_tweets> <input_file> [number_of_words_to_read]
Copy
- `seed_value`: Unsigned integer used to initialize the random number generator
- `number_of_tweets`: Number of tweets to generate
- `input_file`: Path to the text file used to build the Markov chain
- `number_of_words_to_read` (optional): Limit the number of words to read from the input file. If not provided, the entire file will be read.
# Snakes and Ladders Game Simulation
### Key Features:

- Simulates the classic Snakes and Ladders board game
- Utilizes the same Markov chain framework for movement probabilities

## Process:

- Board Representation: The game board is represented as a series of states (positions).
- Movement Probabilities: Instead of using dice rolls, the system uses Markov chains to determine the probability of moving from one position to another.
- Game Simulation: The framework simulates game progression based on these probabilities, including the effects of snakes and ladders.

### Generic Programming Approach
The use of generic programming in this framework offers several advantages:

- Flexibility: The core Markov chain logic can be applied to various problems beyond text analysis and board game simulation.
- Reusability: The same codebase can be used for different data types and applications without significant modifications.
- Extensibility: New applications can be easily integrated by implementing appropriate interfaces or adapting existing components.



## Features

- Builds a Markov chain from input text
- Generates random tweets of specified length
- Customizable number of tweets to generate
- Option to limit the number of words read from the input file
## Usage
USAGE:./snake seed_value(unsigned_int) number_of_paths(int)


## Building the Project 
-  `make tweets`: Creates a tweets generator that works like in the example.
-   `make snake`: Creates a simulation of snakes and laddders game that works accoding to probabilty 
