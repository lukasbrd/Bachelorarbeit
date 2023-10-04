#include "sqliteDB.h"

sqlite3 *sqlitedb = NULL;

void initSqliteDB() {
    int rc;
    char *err_msg = 0;

    mkdir("sqliteDB", 0700);

    // database connection
    rc = sqlite3_open("sqliteDB/sqlite.db", &sqlitedb);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %d: %s\n", rc, sqlite3_errmsg(sqlitedb));
        exit(1);
    }

    // create table if not exists
    const char *sql = "CREATE TABLE IF NOT EXISTS state(digest BLOB PRIMARY KEY, len INT, state TEXT);";
    rc = sqlite3_exec(sqlitedb, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot create table: %d: %s\n", rc, sqlite3_errmsg(sqlitedb));
        sqlite3_close(sqlitedb);
        exit(1);
    }
}

void closeSQLiteDB() {
    if (sqlitedb != NULL) {
        sqlite3_close(sqlitedb);
        sqlitedb = NULL;
    }
}

void writeOneStateToSQLiteDatabase(char *state, size_t len, const char digest[HASH_LEN]) {
    sqlite3_stmt *stmt = NULL;
    int rc;

    // prepare statement
    // if the state already exists in the database, it is not inserted.
    const char *sql2 = "INSERT OR IGNORE INTO state (digest, len, state) VALUES (?,?,?)";
    rc = sqlite3_prepare_v2(sqlitedb, sql2, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %d: %s\n", rc, sqlite3_errmsg(sqlitedb));
        sqlite3_finalize(stmt);
        return;
    }

    // bind parameters
    rc = sqlite3_bind_blob(stmt, 1, digest, 20, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind digest: %d: %s\n", rc, sqlite3_errmsg(sqlitedb));
        sqlite3_finalize(stmt);
        return;
    }

    rc = sqlite3_bind_int(stmt, 2, len);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind len : %d: %s\n", rc, sqlite3_errmsg(sqlitedb));
        sqlite3_finalize(stmt);
        return;
    }

    rc = sqlite3_bind_text(stmt, 3, state, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind state: %d: %s\n", rc, sqlite3_errmsg(sqlitedb));
        sqlite3_finalize(stmt);
        return;
    }

    // execute statement
    // if the state is already in the database, nothing happens.
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Cannot insert state: %d: %s\n", rc, sqlite3_errmsg(sqlitedb));
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
}

char *restoreOneStateFromSQLiteDatabase(const char digest[HASH_LEN], size_t oldLen) {
    sqlite3_stmt *stmt = NULL;
    char databaseDigest[HASH_LEN];
    char newDigest[HASH_LEN];
    size_t len;
    char *buf = NULL;
    char *state = NULL;
    int rc;

    // perpare statement to select by digest
    const char *sql2 = "SELECT digest, len, state FROM state WHERE digest = ?";
    rc = sqlite3_prepare_v2(sqlitedb, sql2, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %d: %s\n", rc, sqlite3_errmsg(sqlitedb));
        sqlite3_finalize(stmt);
        return NULL;
    }

    //bind parameters
    rc = sqlite3_bind_blob(stmt, 1, digest, HASH_LEN, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind digest: %d: %s\n", rc, sqlite3_errmsg(sqlitedb));
        sqlite3_finalize(stmt);
        return NULL;
    }

    // execute statement
    // if the state is not found, NULL is returned.
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return NULL;
    }

    // read databaseDigest
    memcpy(databaseDigest, sqlite3_column_blob(stmt, 0), HASH_LEN);

    // read databaseLength
    len = sqlite3_column_int(stmt, 1);
    if (len != oldLen) {
        fprintf(stderr, "The old stateLength is different from the restored stateLength.\n");
        return NULL;
    }

    // read state
    buf = (char *)sqlite3_column_text(stmt, 2);
    state = (char *)malloc(sizeof(char) * (len + 1));
    memcpy(state, buf, len);
    state[len] = '\0';

    sqlite3_finalize(stmt);

    // compare databaseDigest with newly calcultated digest
    hash(state, (int) len, newDigest);
    if (memcmp(databaseDigest, newDigest , HASH_LEN) != 0) {
        fprintf(stderr, "The State was corrupted.\n ");
        free(state);
        return NULL;
    }
    return state;
}

void deleteOneStateFromSQLiteDatabase(char digest[HASH_LEN]) {
    sqlite3_stmt *stmt = NULL;
    int rc;

    // prepare statement to delete row by digest
    const char *sql2 = "DELETE FROM state WHERE digest = ?";
    rc = sqlite3_prepare_v2(sqlitedb, sql2, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %d: %s\n", rc, sqlite3_errmsg(sqlitedb));
        sqlite3_finalize(stmt);
        return;
    }

    // bind parameter
    rc = sqlite3_bind_blob(stmt, 1, digest, HASH_LEN, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind digest: %d: %s\n", rc, sqlite3_errmsg(sqlitedb));
        sqlite3_finalize(stmt);
        return;
    }

    // execute statement
    // if the state is already deleted, nothing happens.
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Cannot delete row: \n");
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
}

void restoreAllStatesFromSQLiteDatabaseToQueue(zsock_t *command, Queue *const q) {
    int rc;
    sqlite3_stmt *stmt = NULL;

    // prepare statement to restore all states from database
    const char *sql2 = "SELECT digest, len, state FROM state";
    rc = sqlite3_prepare_v2(sqlitedb, sql2, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(sqlitedb));
        sqlite3_finalize(stmt);
        return;
    }

    // restore all states from database
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char databaseDigest[HASH_LEN];
        char newDigest[HASH_LEN];
        memcpy(databaseDigest, sqlite3_column_blob(stmt, 0), HASH_LEN);
        size_t len = sqlite3_column_int(stmt, 1);
        char *state = (char *)malloc(sizeof(char) * (len + 1));
        char *buf = (char *)sqlite3_column_text(stmt, 2);
        memcpy(state, buf, len);
        state[len] = '\0';

        hash(state, (int) len, newDigest);
        if (memcmp(databaseDigest, newDigest , HASH_LEN) != 0) {
            fprintf(stderr, "The State was corrupted.\n ");
            free(state);
        } else {
            printf("Restored: %s\n", state);
            sendElement(command, state, RESTORED, q);
        }
    }
    sqlite3_finalize(stmt);
}