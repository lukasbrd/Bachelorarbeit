#include "fileHandler.h"
#include "persistenceInterface.h"
#include "settings.h"

void persistOneTerm(char *const term, const size_t len, const char digest[HASH_LEN]) {
#ifdef FILEHANDLER
    writeOneTermToFileStorage(term, len, digest);
#endif
}

char *loadOneTerm(const char digest[HASH_LEN]) {
#ifdef FILEHANDLER
    return readOneTermFromFileStorage(digest);
#endif
}

void deleteOneTerm(const char digest[HASH_LEN]) {
#ifdef FILEHANDLER
    deleteOneTermFromFileStorage(digest);
#endif
}

void readAllTerms(zsock_t *command, wQueue *const q) {
#ifdef FILEHANDLER
    readAllTermsFromFileStorageToQueue(command, q);
#endif
}