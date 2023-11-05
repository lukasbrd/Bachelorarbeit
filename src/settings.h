// Uncomment the constant to restore all states in the
// persistent storage to the queue at the start of the program.
//#define RESTOREALLSTATESTOQUEUE

// Uncomment the constant to delete all regained states from
// the persistent storage at the end of the program.
#define DELETEREGAINEDSTATES

// Decide how many states are created.
#define NUMBEROFSTATES 100

// Decide how many letters can be added at a maximum.
// The value must be greater than 0.
#define MAXADDEDLENGTH 100

// Decide how many states should stay in memory.
#define MAXINMEMORY 0

// Uncomment one constant to choose the persistence-mode
// of the program.
#define FILEHANDLER
// #define SQLITE
// #define BERKELEYDB