#include "queue.h"
#include "hash.h"
#include <stdio.h>
#include <string.h>

// Queue Initialisierung
// Es wird Speicher reserviert für 2 Zellen und die Groesse der Queue
// Erste Zelle zeigt auf NULL
// Zweite Zelle zeigt auf NULL
// Groesse der Queue ist 0
wQueue *init_queue() {
    wQueue *q = (wQueue *)malloc(sizeof(wQueue));
    q->first = NULL;
    q->last = NULL;
    q->c = 0;
    q->in_mem = 0;
    q->not_in_mem = NULL;
    return q;
}

// Eine Zelle einfuegen
//  wQueue *const: q ist ein Konstanter Pointer auf mutable wQueue,  Zeiger auf die gesamte Queue
//  char *const:   term ist ein Konstanter Pointer auf ein char,     Zeiger auf den Text
// const size_t:   len ist Kontanter unsigned Integer,               Länge des Textes

void enqueue(wQueue *const q, char *const term, const size_t len, const char digest[HASH_LEN]) {

    // Ein Pointer auf eine neue Zelle wird erstellt und Speicher reserviert
    tCell *new = (tCell *)malloc(sizeof(tCell));

    // Die Groesse der Queue wird im 1 erhoeht
    q->c++;

    // Der Zeiger der Zelle wird auf den übergebenen Zeiger zum Text gesetzt
    new->term = term;
    // Die Länge des Textes wird auf die übergebene Länge gesetzt
    new->term_length = len;
    // Der Zeiger auf die nächste Zelle wird auf NULL gesetzt
    new->next = NULL;

    //Der Hashwert wird in die Zelle eingefügt
    memcpy(new->digest, digest, HASH_LEN);

    // Wenn die Queue leer ist, wird die erste Zelle eingefügt
    if (q->first == NULL) { // Warum nicht q->c == 0?
        q->first = new;
        q->last = new;
        // q->first->next = NULL;

        // Wenn die Queue nicht leer ist, wird der *next-zeiger der letzten Zelle der Queue der *new Zeiger
    } else {
        // tCell *temp = q->last; // for multithreading
        // q->last = new;
        // temp->next = new;
        q->last->next = new;
        q->last = new;
    }
    writeToBuffer(new);
}

tCell *dequeue(wQueue *const q) {
    if (q->c == 0) {
        return NULL;
    }
    q->c--;

    // Die erste Zelle ist das Ergebnis von Dequeue
    tCell *res = q->first;

    // Die erste Zelle der Queue wird auf die nächste Zelle gesetzt
    q->first = q->first->next;

    // Wenn die letzte Zelle ausgegeben wird, hat der *last-Zeiger auf diese letzte Zelle gezeigt und muss daher entfernt werden
    if (q->first == NULL) {
        q->last = NULL;
    }

    // Der Zeiger auf die nächste Zelle im Resultat wird entfernt
    res->next = NULL;

    free(res->term); // Selbst hinzugefügt. Wegen der Dummy-Daten.

    return res;
}

void teardown_queue(wQueue *q) {
    tCell *res = q->first;
    while (res != NULL) {
        res = dequeue(q);
        free(res);
    }
    free(q);
}

/* returns 1 if the queue is empty,
 * 0 otherwise */
int is_empty(wQueue const *const q) {
    return (q->first == NULL);
}

/* returns the current queue size */
size_t q_size(wQueue const *const q) {
    return q->c;
}

void printAllTermsOfCells(wQueue const *const q) {
    tCell *tmp = q->first;

    while (tmp != NULL) {
        printf("Length: %ld\n", tmp->term_length);
        printf("Text: %s\n", tmp->term);
        tmp = tmp->next;
    }
}