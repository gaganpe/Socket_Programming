/*
serverM.h  Header file for Server M inclusions and definitions 
*/



#ifndef SERVERM_H//Protection against multiple header inclusions
#define SERVERM_H
/*********************************** Header Inclusions ********************/
#include "connection_helper.h"
/*********************************** Defines  ********************/

#define TCP_PORT_NUM_CLIENTA 25992  // 25000 + 992(last 3 digits of USC ID)
#define TCP_PORT_NUM_CLIENTB 26992  // 26000 + 992(last 3 digits of USC ID)
#define UDP_PORT_NUM_LOCAL 24992  // 24000 + 992(last 3 digits of USC ID)
#define UDP_PORT_NUM_SERVERA 21992  // 21000 + 992(last 3 digits of USC ID)
#define UDP_PORT_NUM_SERVERB 22992  // 22000 + 992(last 3 digits of USC ID)
#define UDP_PORT_NUM_SERVERC 23992  // 23000 + 992(last 3 digits of USC ID)
#define serverAaddress localhost // Communication is completely in the same device
#define serverBaddress localhost // Communication is completely in the same device
#define serverCaddress localhost // Communication is completely in the same device
#define GENERATED_TRANSACTION_FILE "alichain.txt"


/*********************************** Declarations  ********************/
int server_port_array[3] = {UDP_PORT_NUM_SERVERA,UDP_PORT_NUM_SERVERB,UDP_PORT_NUM_SERVERC};
char server_ipaddress_array[3][10] = {serverAaddress,serverBaddress,serverBaddress};




#endif
