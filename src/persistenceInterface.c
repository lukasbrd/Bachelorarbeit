#include "persistenceInterface.h"
#include "settings.h"
#include "fileHandler.h"
#include "sqliteDatabase.h"

void persistOneState(char *const state, const size_t len, const char digest[HASH_LEN]) {
#ifdef FILEHANDLER
    writeOneStateToFileStorage(state, len, digest);
#endif
#ifdef SQLITE
    writeOneStateToSQLiteDatabase(state, len, digest);
#endif
}

char *restoreOneState(const char digest[HASH_LEN], const size_t oldLen) {
#ifdef FILEHANDLER
    return readOneStateFromFileStorage(digest);
#endif
#ifdef SQLITE
    return readOneStateFromSQLiteDatabase(digest, oldLen);
#endif
}

void deleteOneState(const char digest[HASH_LEN]) {
#ifdef FILEHANDLER
    deleteOneStateFromFileStorage(digest);
#endif
#ifdef SQLITE
    deleteOneStateFromSQLiteDatabse(digest);
#endif
}

void restoreAllStates(zsock_t *command, Queue *const q) {
#ifdef FILEHANDLER
    restoreAllStatesFromFileStorageToQueue(command, q);
#endif
#ifdef SQLITE
    restoreAllStatesFromSQLiteDatabaseToQueue(command, q);
#endif
}