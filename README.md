# Markov Chain Tweet Generator

This project implements a Markov chain-based tweet generator in C. It reads input from a text file, builds a Markov chain model, and generates random tweets based on the learned patterns.

## Features

- Builds a Markov chain from input text
- Generates random tweets of specified length
- Customizable number of tweets to generate
- Option to limit the number of words read from the input file

## Usage

./program_name <seed_value> <number_of_tweets> <input_file> [number_of_words_to_read]
Copy
- `seed_value`: Unsigned integer used to initialize the random number generator
- `number_of_tweets`: Number of tweets to generate
- `input_file`: Path to the text file used to build the Markov chain
- `number_of_words_to_read` (optional): Limit the number of words to read from the input file. If not provided, the entire file will be read.

## Building the Project 
-  `make tweets`: Creates a tweets generator that works like in the example.
-   `make snake`: Creates a simulation of snakes and laddders game that works accoding to probabilty 
