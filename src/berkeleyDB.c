#include "berkeleyDB.h"
#include <db.h>


void writeOneStateToBerkeleyDB(char *state, size_t len, char digest[HASH_LEN]){
    DB *db;
    int rc;
    mkdir("berkeleyDB",0700);

    rc = db_create(&db, NULL, 0);
    if (rc != 0) {
        fprintf(stderr, "Cannot create BerkeleyDB. \n");
        return;
    }

    rc = db->open(db, NULL, "berkeleyDB/berkeley.db", NULL, DB_BTREE, DB_CREATE, 0600);
    if (rc != 0) {
        fprintf(stderr, "Error: %s\n", db_strerror(rc));
        return;
    }

    DBT key, data;
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));
    key.data = digest;
    key.size = HASH_LEN;
    data.data = state;
    data.size = len;

    rc = db->put(db, NULL, &key, &data, 0);
    if (rc != 0) {
        fprintf(stderr, "Cannot insert data into Berkeley DB\n");
    }

    db->close(db, 0);
}

char *restoreOneStateFromBerkeleyDB(char digest[HASH_LEN], size_t oldLen) {
    DB *db;
    int rc;
    mkdir("berkeleyDB",0700);
    char newDigest[HASH_LEN];
    char *state = NULL;

    rc = db_create(&db, NULL, 0);
    if (rc != 0) {
        fprintf(stderr, "Cannot create BerkeleyDB. \n");
        return NULL;
    }

    rc = db->open(db, NULL, "berkeleyDB/berkeley.db", NULL, DB_BTREE, DB_RDONLY, 0);
    if (rc != 0) {
        fprintf(stderr, "Error opening Berkeley DB\n");
        return NULL;
    }

    DBT key, data;
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));
    key.data = digest;
    key.size = HASH_LEN;

    rc = db->get(db, NULL, &key, &data, 0);
    if (rc != 0) {
        fprintf(stderr, "Cannot get data from Berkeley DB\n");
        return NULL;
    }

    if (data.size != oldLen) {
        fprintf(stderr, "The old stateLength is different from the restored stateLength.\n");
        return NULL;
    }

    state = (char *)malloc(sizeof(char) * (data.size + 1));
    memcpy(state, data.data, data.size);
    state[data.size] = '\0';

    db->close(db, 0);

    hash(state, (int) data.size, newDigest);
    if (memcmp(key.data, newDigest , HASH_LEN) != 0) {
        fprintf(stderr, "The State was corrupted.\n ");
        free(state);
        return NULL;
    }
    return state;
}


void deleteOneStateFromBerkeleyDB(char digest[HASH_LEN]) {
    DB *db;
    int rc;
    mkdir("berkeleyDB", 0700);

    rc = db_create(&db, NULL, 0);
    if (rc != 0) {
        fprintf(stderr, "Cannot create BerkeleyDB. \n");
        return;
    }

    rc = db->open(db, NULL, "berkeleyDB/berkeley.db", NULL, DB_BTREE, 0, 0);
    if (rc != 0) {
        fprintf(stderr, "Error opening BerkeleyDB\n");
        return;
    }

    DBT key;
    memset(&key, 0, sizeof(DBT));
    key.data = digest;
    key.size = HASH_LEN;

    rc = db->del(db, NULL, &key, 0);
    if (rc != 0) {
        fprintf(stderr, "Cannot delete data from BerkeleyDB\n");
    }

    db->close(db, 0);
}


void restoreAllStatesFromBerkeleyDBToQueue(zsock_t *command, Queue *const q) {
    DB *db;
    DBT key, data;
    DBC *cursor;
    int rc = 0;
    mkdir("berkeleyDB", 0700);

    rc = db_create(&db, NULL, 0);
    if (rc != 0) {
        fprintf(stderr, "Cannot create BerkeleyDB. \n");
        return;
    }

    rc = db->open(db, NULL, "berkeleyDB/berkeley.db", NULL, DB_BTREE, DB_CREATE, 0);
    if (rc != 0) {
        fprintf(stderr, "Error opening BerkeleyDB\n");
        return;
    }

    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    db->cursor(db, NULL, &cursor, 0);

    while(cursor->c_get(cursor, &key, &data, DB_NEXT) == 0) {
        char databaseDigest[HASH_LEN];
        char newDigest[HASH_LEN];

        memcpy(databaseDigest, key.data, HASH_LEN);
        char *state = (char *)malloc(data.size + 1);
        memcpy(state, data.data, data.size);
        state[data.size] = '\0';

        hash(state, (int) data.size, newDigest);
        if (memcmp(databaseDigest, newDigest, HASH_LEN) != 0) {
            fprintf(stderr, "The state was corrupted.\n");
            free(state);
        } else {
            printf("Berkeley:%s\n", state);
            sendElement(command, state, RESTORED, q);
        }
    }
    cursor->c_close(cursor);
    db->close(db, 0);
}