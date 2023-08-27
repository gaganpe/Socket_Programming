/*
clientA.h - client A header file for all header inclusions and client specific inclusions
*/




#ifndef CLIENTA_H//Protection against multiple header inclusions
#define CLIENTA_H
/*********************************** Header Inclusions ********************/
#include "connection_helper.h"
/*********************************** Defines  ********************/

#define TCP_PORT_NUM_SERVER 25992  // 25000 + 992(last 3 digits of USC ID)
#define CLIENT_NAME "client A"
#define CLIENT_SHORT_NAME "clientA"

/************************************Declarations******************/
char remote_address[] = localhost; // The server address is the same as the localhost address(127.0.0.1)
#endif
