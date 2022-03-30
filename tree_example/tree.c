#include "tree.h"

/*
 *  Copia la dirección y rssi suministrados en nodo
 *  @params node  variable donde se va guardar
 *  @params addr dirección a guardar 
 *  @params rssi rssi a guardar 
*/
void handleNode(struct neighbors *node, linkaddr_t addr, signed int rssi) {
    linkaddr_copy(&node->addr, &addr);
    node->rssi = rssi;
}
