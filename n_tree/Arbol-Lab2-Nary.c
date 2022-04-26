#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct node {
    int id;
    struct node *sibling;
    struct node *child;
    struct node *nodeParent;
};

struct routeList
{
    int id;
    struct routeList *next;
};

typedef struct node node;
struct routeList* routes;

node * new_node(int);
node * add_sibling(node *, node *);
node * add_child(node *, node *);
void print_node_decendents(node *, bool);
void searchNode(node *, node *);
void printList(node *);



int main(int argc, char *argv[])
{
    node *root = new_node(0);
    node *node1 = new_node(1);
    node *node2 = new_node(2);
    node *node3 = new_node(3);
    node *node4 = new_node(4);
    node *node5 = new_node(5);
    node *node6 = new_node(6);
    node *node7 = new_node(7);
    node *node8 = new_node(8);
    node *node9 = new_node(9);
    node *node10 = new_node(10);

    add_child(root, node1);
    add_child(root, node2);

    add_child(node1, node3);
    add_child(node1, node4);
    add_child(node1, node5);

    add_child(node2, node6);
    add_child(node2, node7);
    add_child(node2, node8);

    add_child(node8, node9);
    add_child(node9, node10);

    print_node_decendents(root, false);
    printf("\n");
    searchNode(root, node10);

 }

node * new_node(int id){
    node *new_node = malloc(sizeof(node));
    if ( new_node ) {
        new_node->sibling = NULL;
        new_node->child = NULL;
        /* Se inicializa el padre del nodo como NULL */
        new_node->nodeParent = NULL;
        new_node->id = id;
    }
    return new_node;
}

node * add_sibling(node * eldest, node * silbing){
    /* Guardo la infomación el nodo hermano con su respectivo padre */
    node *new_node=silbing;
        new_node->nodeParent = eldest->nodeParent;
    if ( eldest == NULL )
        return NULL;

    while (eldest->sibling)
        eldest = eldest->sibling;
    return (eldest->sibling = new_node);
}

node * add_child(node * parent, node * child){
    /* Guardo la información del nodo hijo y de su padre */
    node *new_node=child;
        new_node->nodeParent = parent;

   /*  printf("->Parent id %d \n", parent->id);
    printf("->Child %d \n", child->id);
    printf("-------------\n"); */
    if ( parent == NULL )
        return NULL;

    if ( parent->child ){
        /* Si el nodo padre ya tiene un hijo, se le agrega un hermano */
        return add_sibling(parent->child, child);
    }
    else{
        /* Retorna la info del hijo */
        return (parent->child = new_node);
    }
}

void print_node_decendents(struct node* root, bool hasSilbing){
    if(root){
        if(!hasSilbing){
            printf("Decendents of: ");
        }
        printf(" %d ->", root->id);
        if(!hasSilbing){
            printf(" \n");
        }
        print_node_decendents(root->child, true);
        if(hasSilbing){ 
            print_node_decendents(root->sibling, true);
        }
    }
}

void searchNode(node* from, node* to){
    if(to->nodeParent){
    /* Si el nodo destino tiene un padre valido */
        if(to->nodeParent->id==from->id){
            printf(" Se debe enviar el paquete hacia el nodo %d  \n", to->id);
            return to->id;
        }else{
           searchNode(from, to->nodeParent);
        }
    }else{
        /* Sino indica que no se puede enviar */
        printf("No se pueden enviar los paquetes desde el node %d \n", from->id);
        return NULL;
    }
}


/* void searchNode(node* from, node* to){
    if(from){
        if(from->id == to->id){    
            printf("\n Send to %d successfully. \n", from->id);
            printList(from);
        } else {
            if(from->child){
                searchNode(from->child, to);
            }
            if(from->sibling){
                searchNode(from->sibling, to);
            }
        }
        
    }
} */


void printList(node* node){
    if(node){
        /* printf(" Route %d  \n", node->nodeParent->id); */
        if(node->nodeParent->nodeParent==NULL && node->nodeParent!=NULL){
        printf(" Routssse %d  \n", node->id);

            }else{
        printList(node->nodeParent);

            }
       /*  if(node->nodeParent->nodeParent == NULL  ){
        printList(node->nodeParent);

        }else{

        } */
    }else{
        printf("Empty linked list \n");
    }

}
