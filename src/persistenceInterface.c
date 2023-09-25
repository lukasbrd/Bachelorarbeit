#include "persistenceInterface.h"
#include "settings.h"
#include "berkeleyDB.h"
#include "fileHandler.h"
#include "sqliteDB.h"


void persistOneState(Element *element) {
#ifdef FILEHANDLER
    writeOneStateToFileStorage(element->state, element->stateLength, element->digest);
#endif
#ifdef SQLITE
    writeOneStateToSQLiteDatabase(element->state, element->stateLength, element->digest);
#endif
#ifdef BERKELEYDB
    writeOneStateToBerkeleyDB(element->state, element->stateLength, element->digest);
#endif
}

char *restoreOneState(Element *element) {
#ifdef FILEHANDLER
    return restoreOneStateFromFileStorage(element->digest, element->stateLength);
#endif
#ifdef SQLITE
    return restoreOneStateFromSQLiteDatabase(element->digest, element->stateLength);
#endif
#ifdef BERKELEYDB
    return restoreOneStateFromBerkeleyDB(element->digest, element->stateLength);
#endif
}

void deleteOneStateFromPersistentStorage(Element *element) {
#ifdef FILEHANDLER
    deleteOneStateFromFileStorage(element->digest);
#endif
#ifdef SQLITE
    deleteOneStateFromSQLiteDatabase(element->digest);
#endif
#ifdef BERKELEYDB
    deleteOneStateFromBerkeleyDB(element->digest);
#endif
}

void restoreAllStatesToQueue(zsock_t *command, Queue *const q) {
#ifdef FILEHANDLER
    restoreAllStatesFromFileStorageToQueue(command, q);
#endif
#ifdef SQLITE
    restoreAllStatesFromSQLiteDatabaseToQueue(command, q);
#endif
#ifdef BERKELEYDB
    restoreAllStatesFromBerkeleyDBToQueue(command, q);
#endif
}