/*
serverB.h  Header file for Server B inclusions and definitions 
*/



#ifndef SERVERB_H//Protection against multiple header inclusions
#define SERVERB_H
/*********************************** Header Inclusions ********************/
#include "connection_helper.h"
/*********************************** Defines  ********************/

#define UDP_PORT_NUM_LOCAL 22992  // 22000 + 992(last 3 digits of USC ID)
#define serverMaddress localhost // Communication is completely in the same device
#define TRANSACTION_FILE_NAME "block2.txt"
#define SERVER_NAME "ServerB"


#endif
