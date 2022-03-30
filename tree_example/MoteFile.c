#include "tree.h"

PROCESS(sendNodeInfo, "Enviar caracteristicas del nodo");
PROCESS(selectParent, "Seleccionar nodo padre");
AUTOSTART_PROCESSES(&sendNodeInfo, &selectParent);

struct neighbors currentNode;
struct node basicNode;
struct neighbors *prevParent = NULL;



static void handleBroadcast(struct broadcast_conn *c, const linkaddr_t *from) {

  struct neighbors *neighborNode, *n;

  signed int rssi = cc2420_last_rssi;
  signed int rssiOffset = -45;
  signed int rssiEnlace = rssi + rssiOffset;
  void *neighborPacket = packetbuf_dataptr();

  neighborNode = ((struct neighbors*) neighborPacket);

  neighborNode->rssi =  neighborNode->rssi + rssiEnlace;


  for(n = list_head(neighborsList); n != NULL; n = list_item_next(n)) {
    if(linkaddr_cmp(&n->addr, from)) {
      break;
    }
  }

  if(n == NULL) {
    n = memb_alloc(&neighborsMemb);
    if(n == NULL) {
      return;
  	}
    linkaddr_copy(&n->addr, from);
  	n->rssi = rssi;
  	list_add(neighborsList, n);
    process_post(&selectParent, PROCESS_EVENT_CONTINUE, NULL);
  }
}


static const struct broadcast_callbacks broadcastCallback = {handleBroadcast};
static struct broadcast_conn broadcastConnection;

/*Envio de las caracteristicas del nodo a neighbors*/
PROCESS_THREAD(sendNodeInfo, ev, data) {
  static struct etimer et;
  PROCESS_EXITHANDLER(broadcast_close(&broadcastConnection);)
  PROCESS_BEGIN();
  broadcast_open(&broadcastConnection, 129, &broadcastCallback);

  if(linkaddr_node_addr.u8[0] == 1) {
     basicNode.rssi= 0;
  } else {
     basicNode.rssi = INF_NEG;
  }

  while(1) {
    etimer_set(&et, CLOCK_SECOND*5);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    handleNode(&currentNode, linkaddr_node_addr, basicNode.rssi);
    packetbuf_copyfrom(&currentNode, sizeof(struct neighbors));
    broadcast_send(&broadcastConnection);
    /* printf(" Send info from node %d.%d in broadcast connection con rssi %d \n", linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1], basicNode.rssi); */
  }

  PROCESS_END();
}

/*Seleccion de padre de la lsita de neighbors*/
PROCESS_THREAD(selectParent, ev, data) {
  PROCESS_BEGIN();

  memb_init(&neighborsMemb);
  list_init(neighborsList);
  struct neighbors *n, *currentParent;

  while(1) {

    PROCESS_YIELD();

    if(linkaddr_node_addr.u8[0] == 1) {
       break;
    } else {
      printf("#L %d 0\n", prevParent->addr.u8[0]);
      currentParent = list_head(neighborsList);
      for(n = list_head(neighborsList); n != NULL; n = list_item_next(n)) {
        if(n->rssi > currentParent->rssi) {
          currentParent=n;
        }
      }
      printf("#L %d 1\n", currentParent->addr.u8[0]);
      basicNode.parent=currentParent->addr;
      basicNode.rssi=currentParent->rssi;
      prevParent=currentParent;
      }
  }
  
  PROCESS_END();
}