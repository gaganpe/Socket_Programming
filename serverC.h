/*
serverC.h  Header file for Server C inclusions and definitions 
*/



#ifndef SERVERC_H//Protection against multiple header inclusions
#define SERVERC_H
/*********************************** Header Inclusions ********************/
#include "connection_helper.h"
/*********************************** Defines  ********************/

#define UDP_PORT_NUM_LOCAL 23992  // 23000 + 992(last 3 digits of USC ID)
#define serverMaddress localhost // Communication is completely in the same device
#define TRANSACTION_FILE_NAME "block3.txt"
#define SERVER_NAME "ServerC"


#endif
