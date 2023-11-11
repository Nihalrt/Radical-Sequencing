# Radical-Formation-Multi-threading

## Description
The Kosmos-MCV project is a multi-threaded C program simulating interactions between Carbon (C) and Hydrogen (H) atoms. These atoms combine to form ethynyl radicals (C2H) in a controlled environment using mutexes and condition variables.

## Files
- **kosmos-mcv.c**: Implements the multi-threaded code with mutexes and condition variables for atom interactions.
- **kosmos-mcv-semaphore.c**: An alternate version using semaphores for atom interactions.

## Implementation Overview
The code initializes a specific number of C and H atoms, controlling their interactions based on defined conditions. It uses synchronization primitives to ensure the correct formation of ethynyl radicals.

## Mutexes and Condition Variables
- **mutex**: Controls access to shared variables and critical sections.
- **mutex_c, mutex_h**: Ensures exclusive access to variables specific to C and H atoms.
- **logging**: Manages logging activities to prevent concurrent access issues.
- **wait_c, wait_h**: Condition variables used to signal availability of C and H atoms for combination.
- **staging_area**: Synchronizes the formation of radicals.

## Functions
- **kosmos_init()**: Initializes mutexes, condition variables, and shared variables.
- **c_ready() and h_ready()**: Functions representing C and H atom behaviors, coordinating combinations and radical formation.
- **make_radical()**: Forms the ethynyl radical and updates shared variables.
- **wait_to_terminate()**: Terminates the program after a specified duration.

## Conclusion
This code serves as an educational tool to understand and demonstrate the use of mutexes and condition variables for managing multi-threaded interactions between atoms. Additionally, the provided semaphore version (**kosmos-mcv-semaphore.c**) offers an alternate implementation using semaphores for achieving the same functionality.
