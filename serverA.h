/*
serverA.h  Header file for Server A inclusions and definitions 
*/



#ifndef SERVERA_H//Protection against multiple header inclusions
#define SERVERA_H
/*********************************** Header Inclusions ********************/
#include "connection_helper.h"
/*********************************** Defines  ********************/

#define UDP_PORT_NUM_LOCAL 21992  // 21000 + 992(last 3 digits of USC ID)
#define serverMaddress localhost // Communication is completely in the same device
#define TRANSACTION_FILE_NAME "block1.txt"
#define SERVER_NAME "ServerA"


#endif
