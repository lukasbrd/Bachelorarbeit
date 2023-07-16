#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include <czmq.h>
#include <czmq_library.h>

pthread_t thread;

#define ENQUEUE 1
#define DEQUEUE 2
bool runner = true;
bool loopend = true;

void *threaddi(void *args) {
    zsock_t *cmd_recv = zsock_new_pull("inproc://example-cmd");
    zsock_t *package_send = zsock_new_push("inproc://example-package");
    zsock_t *enqueue = zsock_new_push("inproc://queue");
    zsock_t *dequeue = zsock_new_pull("inproc://queue");

    while (runner && loopend) {
        loopend = false;
        int cmd;
        tCell *cell;

        int rc = zsock_recv(cmd_recv, "ip", &cmd, &cell);
        assert(rc == 0);

        printf("2222\n");
        if (cmd == ENQUEUE) {
            int rc = zsock_send(enqueue, "p", cell);
            assert(rc == 0);
            printf("33333\n");
        } else if (cmd == DEQUEUE) {
            tCell *cell2;
            int rc = zsock_recv(dequeue, "p", &cell2);
            assert(rc == 0);
            printf("Term: %s", cell2->term);
            rc = zsock_send(package_send, "p", cell2);
            printf("55555\n");
            assert(rc == 0);
        }
        loopend = true;
    }

    zsock_destroy(&cmd_recv);
    zsock_destroy(&package_send);
    zsock_destroy(&enqueue);
    zsock_destroy(&dequeue);
}

// main thread

void enqueue(zsock_t *cmd_send, char *term) {
    tCell *new = (tCell *)malloc(sizeof(tCell));
    size_t term_length = 0;
    char digest[HASH_LEN] = "";

    term_length = strlen(term);
    hash(term, term_length, digest);

    new->term_length = term_length;
    memcpy(new->digest, digest, HASH_LEN);
    int rc = zsock_send(cmd_send, "ip", ENQUEUE, new);
    assert(rc == 0);

    printf("Term:%s", term);
    writeToStorage(term, term_length, digest);
    free(term);
    free(new);
}

/*
tCell *dequeue(zsock_t *cmd_send, zsock_t *package_recv, wQueue *const q) {
    tCell *cell2;

    int rc = zsock_send(cmd_send, "ip", DEQUEUE, NULL);
    assert(rc == 0);
    rc = zsock_recv(package_recv, "p", &cell2);

    assert(rc == 0);

    cell2->term = readOneTermFromStorage(cell2->digest);
    atomic_fetch_sub(&q->length, 1);

    return cell2;
}*/

int main(void) {
    pthread_create(&thread, NULL, threaddi, NULL);
    zsock_t *cmd_send = zsock_new_push("inproc://example-cmd");
    zsock_t *package_recv = zsock_new_pull("inproc://example-package");
    char *term = NULL;
    q->length = ATOMIC_VAR_INIT(0);

    srand(time(NULL));

    int i;
    for (i = 0; i < 1; i++) {
        term = createRandomString(term);
        enqueue(cmd_send, term);
        printf("11111");
    }

    /*
        for (i = 0; i < 1; i++) {
            if (q->length > 0) {
                tCell *res = dequeue(cmd_send, package_recv, q);
                free(res->term);
                free(res);
                printf("44444");
            }
        }*/

    runner = false;

    zsock_destroy(&cmd_send);
    zsock_destroy(&package_recv);
    return 0;
}
