#include "tree.h"

/* Esta función guarda la info de los neighbors */
/* see: http://contiki.sourceforge.net/docs/2.6/a01684.html   */
MEMB(neighborsMemb, struct neighbors, MAX_NEIGHBORS);

/* Función/libreria que crea la lista de neighbors */
/* see: http://contiki.sourceforge.net/docs/2.6/a01682.html  */
LIST(neighborsList);

PROCESS(sendNodeInfo, "Enviar caracteristicas del nodo");
PROCESS(selectParent, "Seleccionar nodo padre");
PROCESS(sendUnicastToParent, "Establecer unicast con el padre");
AUTOSTART_PROCESSES(&sendNodeInfo, &selectParent, &sendUnicastToParent);

struct neighbors currentNode;
struct node basicNode;
struct neighbors *prevParent = NULL;


/*Callback para la recepcion de mensajes broadcast*/
static void handleBroadcast(struct broadcast_conn *c, const linkaddr_t *from) {

  struct neighbors *neighborNode, *n;

  /*Calculo de RSSI*/
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

/*Envio de las caracteristicas del nodo a neighbors mediante broadcast*/
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
      printf("#L %d 0 \n", prevParent->addr.u8[0]);
      currentParent = list_head(neighborsList);
      for(n = list_head(neighborsList); n != NULL; n = list_item_next(n)) {
        if(n->rssi > currentParent->rssi) {
          currentParent=n;
        }
      }
      printf("#L %d 1 \n", currentParent->addr.u8[0]);
      basicNode.parent=currentParent->addr;
      basicNode.rssi=currentParent->rssi;
      prevParent=currentParent;
      }
  }
  
  PROCESS_END();
}



/*Callback para recepcion de mensajes unicast*/
static void handleReceiveUnicast(struct unicast_conn *c, const linkaddr_t *from)
{
  printf("Unicast message received from %d.%d, '%s'\n",
	 from->u8[0], from->u8[1], (char *)packetbuf_dataptr());

}

/*Callback para envio de mensajes unicast*/
static void hanldeSendUnicast(struct unicast_conn *c, int status, int num_tx)
{
  const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
  if(linkaddr_cmp(dest, &linkaddr_null)) {
    return;
  }
}

static const struct unicast_callbacks unicastCallback = {handleReceiveUnicast, hanldeSendUnicast};
static struct unicast_conn unicastConnection;

/*Establecer una conexión unicast con el padre*/
PROCESS_THREAD(sendUnicastToParent, ev, data)  {

  PROCESS_EXITHANDLER(unicast_close(&unicastConnection);)
  PROCESS_BEGIN();
  unicast_open(&unicastConnection, 146, &unicastCallback);

  while(1) {
    static struct etimer et;
    linkaddr_t addr;
    etimer_set(&et, CLOCK_SECOND*5);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    char message[] = "Mensaje unicast enviado desde el nodo ";
    char unicastNode[3];
    sprintf(unicastNode, "%d", linkaddr_node_addr.u8[0]);
    strcat(message, unicastNode);

    packetbuf_copyfrom(message, sizeof(message)+sizeof(unicastNode));
    addr.u8[0] = basicNode.parent.u8[0];
    addr.u8[1] = 0;
    if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      unicast_send(&unicastConnection, &addr);
    }
  }

  PROCESS_END();
}