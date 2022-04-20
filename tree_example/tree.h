#include "contiki.h"
#include "random.h"
#include <stdio.h>
#include "dev/leds.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "net/rime/rime.h"
#include "dev/button-sensor.h"
/*Cambiar linea de abajo por la que corresponda*/
#include "/home/gnaguerrer/contiki/dev/cc2420/cc2420.h" 

#define INF_NEG -999
#define MAX_NEIGHBORS 16


/*Definición de estructuras*/
struct neighbors {
  struct neighbors *next;
  linkaddr_t addr;
  signed int rssi;
};

struct node {
  linkaddr_t parent;
  signed int rssi;
};


void handleNode(struct neighbors *node, linkaddr_t addr, signed int rssi);