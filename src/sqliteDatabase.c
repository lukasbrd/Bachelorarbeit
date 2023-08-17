#include "sqliteDatabase.h"
#include <sqlite3.h>


void writeOneStateToSQLiteDatabase(char *const state, const size_t len, const char digest[HASH_LEN]) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *err_msg = 0;
    int rc = sqlite3_open("queue.db", &db);

    // database connection
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // create table if not exists
    const char *sql = "CREATE TABLE IF NOT EXISTS state(digest BLOB, len INT, state TEXT);";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot create table: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    // prepare statement
    const char *sql2 = "INSERT INTO state (digest, len, state) VALUES (?,?,?)";
    rc = sqlite3_prepare_v2(db, sql2, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    // bind parameters
    rc = sqlite3_bind_blob(stmt, 1, digest, 20, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind digest: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_bind_int(stmt, 2, len);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind len : %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_bind_text(stmt, 3, state, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind state: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    // execute statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

char *readOneStateFromSQLiteDatabase(const char digest[HASH_LEN], const size_t oldLen) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char fileDigest[HASH_LEN];
    char newDigest[HASH_LEN];
    size_t len;
    char *buf = NULL;
    char *state = NULL;
    char *err_msg = 0;

    int rc = sqlite3_open("queue.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    // create table if not exists
    const char *sql = "CREATE TABLE IF NOT EXISTS state(digest BLOB, len INT, state TEXT);";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot create table: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    const char *sql2 = "SELECT digest, len, state FROM state WHERE digest = ?";
    rc = sqlite3_prepare_v2(db, sql2, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    rc = sqlite3_bind_blob(stmt, 1, digest, HASH_LEN, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind digest: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_ROW) {
        fprintf(stderr, "No data found with this digest: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    memcpy(fileDigest, sqlite3_column_blob(stmt, 0), HASH_LEN);
    len = sqlite3_column_int(stmt, 1);

    if (len != oldLen) {
        fprintf(stderr, "The old stateLength is different than the restored stateLength.\n");
    }

    buf = (char *)sqlite3_column_text(stmt, 2);
    state = (char *)malloc(sizeof(char) * (len + 1));
    memcpy(state, buf, len);


    state[len] = '\0';
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    hash(state, (int) len, newDigest);
    if (memcmp(fileDigest, newDigest , HASH_LEN) != 0) {
        fprintf(stderr, "The State was corrupted.\n ");
    }
    printf("readOneFromDatabase: %s\n", state);
    return state;
}

void deleteOneStateFromSQLiteDatabse(const char digest[HASH_LEN]) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *err_msg = 0;
    int rc = sqlite3_open("queue.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // create table if not exists
    const char *sql = "CREATE TABLE IF NOT EXISTS state(digest BLOB, len INT, state TEXT);";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot create table: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    const char *sql2 = "DELETE FROM state WHERE digest = ?";
    rc = sqlite3_prepare_v2(db, sql2, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_bind_blob(stmt, 1, digest, HASH_LEN, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind digest: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Cannot delete row: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void restoreAllStatesFromSQLiteDatabaseToQueue(zsock_t *command, Queue *const q) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *err_msg = 0;

    int rc = sqlite3_open("queue.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // create table if not exists
    const char *sql = "CREATE TABLE IF NOT EXISTS state(digest BLOB, len INT, state TEXT);";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot create table: %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    const char *sql2 = "SELECT digest, len, state FROM state";
    rc = sqlite3_prepare_v2(db, sql2, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char digest[HASH_LEN];
        memcpy(digest, sqlite3_column_blob(stmt, 0), HASH_LEN);
        size_t len = sqlite3_column_int(stmt, 1);
        char *state = (char *)malloc(sizeof(char) * (len + 1));
        char *buf = (char *)sqlite3_column_text(stmt, 2);
        memcpy(state, buf, len);
        state[len] = '\0';
        sendAndPersist(command, state, RESTORED, q);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}