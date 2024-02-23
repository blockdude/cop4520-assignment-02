# Results and Documentation

To ensure efficiency and proper load balancing I used a random
number generator to pick out which guests should enter the
labyrinth first.

To ensure correctness I used mutex locks and condition variables.
I used the mutex lock and condition varaible together to make
sure that shared memory is safe and to notify threads when it is
their time to write to the shared memory.

I ran my program multiple times and expected to see different
times for each run and I did. I also checked values of variables
to make sure the guest was stopping the party when certain
conditions were met.


# Problem 1

How I solved this is by appointing one guest to eat the cupcakes
as a way to count if all the guests have entered the labyrinth.

If the counting guest sees a cupcake they will eat the cupcake.
once they have eaten a number of cupcakes equal to the number of
guests then they will know that all guests have entered the
labyrinth.

The other guests will do one of two things. If there is a
cupcake at the end of the labyrinth then they will leave it. If
there is no cupcake and they have not requested a new cupcake
before then they will request a new cupcake.


# Problem 2

The guests should choose option three as it allows for any guest
to guarantee that they will be able to view the vase. A
disadvantage might be that if a guest wants to look at the vase
then they have to wait in the queue and can not do anything else.

I chose to implement option two where each guest can enter the
room as long as the sign in front of the door does not say busy.


# Building and Compiling

To compile you will need GCC and optionally Make.

## Method 1: using GCC

To compile using GCC navigate to directory where the source file
is then run the command:

> gcc parta.cpp -lstdc++ -lm -o parta
> gcc partb.cpp -lstdc++ -lm -o partb

Or 

> g++ parta.cpp -o parta
> g++ partb.cpp -o partb

This should generate a binary file named "main".

## Method 2: using Make

To use Make to build this program first navigate to the directory
with the source file then run the command:

> make

This should generate a two binary files named "parta" and "partb".


# Runing

To run the program use:

> ./parta
> ./partb

Or if you are using Make:

> make run

This should run both programs and print to standard output if the
program ran successfully or not.
