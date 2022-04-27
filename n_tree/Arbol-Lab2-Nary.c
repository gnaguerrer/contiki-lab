#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct node {
    int id;
    struct node *sibling;
    struct node *child;
    struct node *nodeParent;
};

typedef struct node node;

int fowardNode=-9999;
bool foundNode;

node * new_node(int);
node * add_sibling(node *, node *);
node * add_child(node *, node *);
void print_node_decendents(node *, bool);

int search_forwarder(node *, node *);
int search_forwarder_withParent(node *, node *);


void searchNode(node *, node *);
void search_forwarder_childs(node *, node *, bool);
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
    int nodeToWithParent=search_forwarder_withParent(node2, node10);
    int nodeTo=search_forwarder(node2, node10);
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


int search_forwarder_withParent(node* from, node* to){
    searchNode(from, to);
    return fowardNode;
}

void searchNode(node* from, node* to){
    if(to->nodeParent){
    /* Si el nodo destino tiene un padre valido */
        if(to->nodeParent->id==from->id){
            printf("Send from node %d using function with parent \n", to->id);
            fowardNode=to->id;
        }else{
           searchNode(from, to->nodeParent);
        }
    }else{
        /* Sino indica que no se puede enviar */
        printf("Unable to send packet from %d \n", from->id);
        fowardNode=-9999;
    }
}


int search_forwarder(node* from, node* to){
    node *childs=from->child;
    while(childs){
        search_forwarder_childs(childs, to, false);
        if(foundNode){ 
            break;    
        } else {
            if(childs->sibling){
                childs=childs->sibling;
            }else{
                childs=NULL;
            }
        }
    }
    if(childs->id){
         printf("Send from node %d \n", childs->id); 
        return childs->id;
    } else {
        printf("Unable to send packet from %d \n", from->id);
        return -9999;
    }
}

void search_forwarder_childs(node* from, node* to, bool checkSilbings){
    if(from){
        if(from->id==to->id){
            foundNode=true;
        }else{
            search_forwarder_childs(from->child, to, true);
            if(checkSilbings){
                search_forwarder_childs(from->sibling, to, true);
            }
        }
    }
}




/* void printList(node* node){
    if(node){
        printf(" Route %d  \n", node->nodeParent->id);
        if(node->nodeParent->nodeParent==NULL && node->nodeParent!=NULL){
        printf(" Routssse %d  \n", node->id);

            }else{
        printList(node->nodeParent);

            }
        if(node->nodeParent->nodeParent == NULL  ){
        printList(node->nodeParent);

        }else{

        }
    }else{
        printf("Empty linked list \n");
    }
} */

