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

    // database connection
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    // create table
    const char *sql = "CREATE TABLE IF NOT EXISTS state(digest BLOB PRIMARY KEY, len INT, term TEXT);";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot create table: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    // prepare statement
    const char *sql2 = "INSERT INTO state (digest, len, term) VALUES (?,?,?)";
    rc = sqlite3_prepare_v2(db, sql2, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    // bind parameters
    rc = sqlite3_bind_blob(stmt, 1, digest, 20, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind digest: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    rc = sqlite3_bind_int(stmt, 2, len);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind len: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    rc = sqlite3_bind_text(stmt, 3, term, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind term: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    // execute statement
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void deleteFromStorage(const char digest[HASH_LEN]) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *err_msg = 0;
    int rc = sqlite3_open("queue.db", &db);
    char buf[HASH_LEN] = "";
    memcpy(buf, digest, HASH_LEN);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    const char *sql = "DELETE FROM state WHERE digest = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    sqlite3_bind_text(stmt, 1, buf, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void readAllFromStorageToQueue(wQueue *const q) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc = sqlite3_open("queue.db", &db);
    char digest[HASH_LEN];
    size_t len;
    char *term;

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    const char *sql = "SELECT digest, len, term FROM state";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        memcpy(digest, sqlite3_column_blob(stmt, 0), HASH_LEN);
        len = sqlite3_column_int(stmt, 1);
        term = (char *)malloc(sizeof(char) * (len + 1));
        char *buf = (char *)sqlite3_column_text(stmt, 2);
        strncpy(term, buf, len);
        term[len] = '\0';
        enqueue(q, term, len, digest);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}