#include "hash.h"
#include "queue.h"
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void writeToStorage(wQueue *const q, char *const term, const size_t len, const char digest[HASH_LEN]) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *err_msg = 0;
    int rc = sqlite3_open("queue.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    // create table
    const char *sql = "CREATE TABLE IF NOT EXISTS state(Id INTEGER PRIMARY KEY, term TEXT, len INT, digest TEXT);";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot create table: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    // prepare statement
    const char *sql2 = "INSERT INTO state (term, len, digest) VALUES (?,?,?)";
    rc = sqlite3_prepare_v2(db, sql2, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    // bind parameters
    rc = sqlite3_bind_text(stmt, 1, term, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        printf("Cannot bind term: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    rc = sqlite3_bind_int(stmt, 2, len);
    if (rc != SQLITE_OK) {
        printf("Cannot bind len: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    rc = sqlite3_bind_text(stmt, 3, digest, 20, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        printf("Cannot bind digest: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    // execute statement
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void deleteFromStorage(const char digest[HASH_LEN]) {
}

void readAllFromStorageToQueue(wQueue *const q) {
}