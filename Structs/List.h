

typedef struct List_Node List_Node;
typedef struct List List;

struct List_Node{
        void *content;
        List_Node *next;
};

struct List{
        List_Node *front;
        int size;
};


List *List_init();

void List_destroy(List *list);

int List_isEmpty(List *list);

void List_append(List *list, void *element);

void *List_pop(List *list);

void *List_get(List *list, int index);