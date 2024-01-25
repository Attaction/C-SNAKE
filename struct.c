#include <stdlib.h>
#include "struct.h"

//Minden egyes negyzetnek, ami a jatek alapjaul szolgalo negyzetes palyat alkotja lefoglalunk helyet a memoriaban.
Node* newNode() {
    Node *node = (Node*) malloc(sizeof(Node));
    node->next = NULL;
    node->prev = NULL;
    node->size = 10;
    return node;
}