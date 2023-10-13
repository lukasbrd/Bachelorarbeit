#include "berkeleyDB.h"
#include <db.h>

DB *berkeleydb = NULL;

void initBerkeleyDB() {
    int rc;

    mkdir("berkeleyDB", 0700);

    rc = db_create(&berkeleydb, NULL, 0);
    if (rc != 0) {
        fprintf(stderr, "Cannot create BerkeleyDB.\n");
        exit(1);
    }

    rc = berkeleydb->open(berkeleydb, NULL, "berkeleyDB/berkeley.db", NULL, DB_BTREE, DB_CREATE, 0600);
    if (rc != 0) {
        fprintf(stderr, "Cannot open BerkeleyDB: %s\n", db_strerror(rc));
        exit(1);
    }
}

void closeBerkeleyDB() {
    if (berkeleydb != NULL) {
        berkeleydb->close(berkeleydb, 0);
        berkeleydb = NULL;
    }
}

void writeOneStateToBerkeleyDB(char *state, size_t len, char digest[HASH_LEN]){
    int rc;
    DBT key, data;

    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    key.size = HASH_LEN;
    key.data = digest;

    data.size = len;
    data.data = state;

    rc = berkeleydb->put(berkeleydb,NULL, &key, &data, 0);
    if (rc != 0) {
        fprintf(stderr, "Cannot insert data into Berkeley DB\n");
    }
}

char *restoreOneStateFromBerkeleyDB(char digest[HASH_LEN], size_t oldLen) {

    int rc;
    char newDigest[HASH_LEN];
    char *state = NULL;
    DBT key, data;

    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    key.size = HASH_LEN;
    key.data = digest;

    // if the state is not found, NULL is returned
    rc = berkeleydb->get(berkeleydb, NULL, &key, &data, 0);
    if (rc != 0) {
        return NULL;
    }

    if (data.size != oldLen) {
        fprintf(stderr, "A saved stateLength is different from the restored stateLength.\n");
        return NULL;
    }

    state = (char *)malloc(sizeof(char) * (data.size + 1));
    memcpy(state, data.data, data.size);
    state[data.size] = '\0';

    hash(state, (int) data.size, newDigest);
    if (memcmp(key.data, newDigest , HASH_LEN) != 0) {
        fprintf(stderr, "A state was corrupted.\n ");
        free(state);
        return NULL;
    }
    return state;
}


void deleteOneStateFromBerkeleyDB(char digest[HASH_LEN]) {
    DBT key;

    memset(&key, 0, sizeof(DBT));
    key.data = digest;
    key.size = HASH_LEN;

    // if the state is already deleted, nothing happens.
    berkeleydb->del(berkeleydb, NULL, &key, 0);
}


void restoreAllStatesFromBerkeleyDBToQueue(zsock_t *command, Queue *const q) {
    DBT key, data;
    DBC *cursor;

    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    berkeleydb->cursor(berkeleydb, NULL, &cursor, 0);

    while(cursor->c_get(cursor, &key, &data, DB_NEXT) == 0) {
        char databaseDigest[HASH_LEN];
        char newDigest[HASH_LEN];

        memcpy(databaseDigest, key.data, HASH_LEN);
        char *state = (char *)malloc(data.size + 1);
        memcpy(state, data.data, data.size);
        state[data.size] = '\0';

        hash(state, (int) data.size, newDigest);
        if (memcmp(databaseDigest, newDigest, HASH_LEN) != 0) {
            fprintf(stderr, "A state was corrupted.\n");
            free(state);
        } else {
            printf("Restored:%s\n", state);
            enqueueElementWithState(command, state, RESTORED, q);
        }
    }
    cursor->c_close(cursor);
}