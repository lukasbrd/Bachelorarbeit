#include "persistenceInterface.h"
#include "settings.h"
#include "fileHandler.h"
#include "sqliteDatabase.h"

void persistOneState(char *state, size_t len, const char digest[HASH_LEN]) {
#ifdef FILEHANDLER
    writeOneStateToFileStorage(state, len, digest);
#endif
#ifdef SQLITE
    writeOneStateToSQLiteDatabase(state, len, digest);
#endif
}

char *restoreOneState(const char digest[HASH_LEN], const size_t oldLen) {
#ifdef FILEHANDLER
    return restoreOneStateFromFileStorage(digest, oldLen);
#endif
#ifdef SQLITE
    return restoreOneStateFromSQLiteDatabase(digest, oldLen);
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

void restoreAllStatesToQueue(zsock_t *command, Queue *const q) {
#ifdef FILEHANDLER
    restoreAllStatesFromFileStorageToQueue(command, q);
#endif
#ifdef SQLITE
    restoreAllStatesFromSQLiteDatabaseToQueue(command, q);
#endif
}