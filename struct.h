typedef struct Node {
    int x, y;
    int size;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct Snake {
    Node *head;
    Node *tail;
    int length;
    int direction;
} Snake;

typedef struct Food {
    int x,y;
}Food;

Node* newNode();