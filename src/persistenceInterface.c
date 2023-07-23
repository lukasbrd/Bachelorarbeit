#include "persistenceInterface.h"
#include "settings.h"
#include "fileHandler.h"
#include "sqliteDatabase.h"

void persistOneTerm(char *const term, const size_t len, const char digest[HASH_LEN]) {
#ifdef FILEHANDLER
    writeOneTermToFileStorage(term, len, digest);
#endif
#ifdef SQLITE
    writeOneTermToSQLiteDatabase(term, len, digest);
#endif
}

char *loadOneTerm(const char digest[HASH_LEN]) {
#ifdef FILEHANDLER
    return readOneTermFromFileStorage(digest);
#endif
#ifdef SQLITE
    return readOneTermFromSQLiteDatabase(digest);
#endif
}

void deleteOneTerm(const char digest[HASH_LEN]) {
#ifdef FILEHANDLER
    deleteOneTermFromFileStorage(digest);
#endif
#ifdef SQLITE
    deleteOneTermFromSQLiteDatabse(digest);
#endif
}

void readAllTerms(zsock_t *command, wQueue *const q) {
#ifdef FILEHANDLER
    readAllTermsFromFileStorageToQueue(command, q);
#endif
#ifdef SQLITE
    readAllTermsFromSQLiteDatabase(command,q);
#endif
}