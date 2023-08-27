/*
clientB.h  Header file for Client B inclusions and definitions 
*/



#ifndef CLIENTB_H//Protection against multiple header inclusions
#define CLIENTB_H
/*********************************** Header Inclusions ********************/
#include "connection_helper.h"
/*********************************** Defines  ********************/

#define TCP_PORT_NUM_SERVER 26992  // 25000 + 992(last 3 digits of USC ID)
#define CLIENT_NAME "clientB"
#define CLIENT_SHORT_NAME "client B"

/***********************************Declarations*******************/
char  remote_address[] = localhost;

#endif
