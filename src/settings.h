// Uncomment the constant to restore all states in the persistent Storage to Queue.
//#define RESTOREALLSTATESTOQUEUE

//Uncomment the constant to delete all created states from persistent Storage after the execution.
#define DELETESTATESAFTEREXECUTION

// Decide how many states are created.
#define NUMBEROFSTATES 1000

// Decide how long the states can become.
#define MAXADDEDLENGTH 100

// Decide how many states should stay in memory.
#define MAXINMEMORY 0

// Uncomment one constant to choose the persistence-mode of the program.
// #define FILEHANDLER
// #define SQLITE
 #define BERKELEYDB