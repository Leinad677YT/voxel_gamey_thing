#pragma once



#define LEINAD_AUX
    // Auxiliary function/data/...



#define LEINAD_FDEFINITION
    // Defines data to be used later on

#define LEINAD_FITERATOR
    // Runs on every element of a collection

#define LEINAD_FCOMPARATOR 
    // a < b -> return < 0
    // a = b -> return = 0
    // a > b -> return > 0

#define LEINAD_FCALL
    // Just wraps a lot of stuff

#define LEINAD_FINITIALIZER
    // Initializes data

#define LEINAD_FBUILDER
    // Creates a data structure

#define LEINAD_FCLEANER
    // Frees an obj*ct / data

#define LEINAD_FGET
    // Gets data from an obj*ct

#define LEINAD_FRENDER
    // Renders an obj*ct