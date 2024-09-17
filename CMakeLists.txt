cmake_minimum_required(VERSION 3.16)
project(ex3b_omerdahan C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(ex3b_omerdahan
        linked_list.c
        linked_list.h
        markov_chain.c
        markov_chain.h
        tweets_generator.c
         print.c print.h snakes_and_ladders.c )
