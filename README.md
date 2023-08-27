# Socket_Programming
Socket Programming Project Spring 2022

This is a UNIX socket programming project with a 2 client, 1 Main server and 3 Backend server setup.


Project Phases Completed:
BALANCE
TXCOINS
TXLIST
STATS

Order of execution:
Similar to what is mentioned in the pdf(Since select is implemented clientA or clientB can be selected. These do not have to be in order)
./serverM ./serverA ./serverB ./serverC ./clientA or ./clientB

Usage:
make all
./serverM
./serverA
./serverB
./serverC
./clientA Brad
./clientB Greg Brad 2
./clientB TXLIST
./clientA Brad stats

The different operations supported are given below
Balance:
Usage: 
./clientA <username> 
To obtain the balance of <username>. If <username> is not part of the network the same information is relayed to the client
Examples:
./clientA Racheal
./clientB Greg

TXCOINS:
Usage: 
./clientA <username1> <username2> <value> 
Transfer <value> amount of coins from <username1> to <username2>. Will only work if both <username1> and <username2> are part of the network and if <username1> has more than or equal to <value> amount of balance. If transaction is successful the remaining balance is shown on the terminal. If the transaction is unsuccessful then the same information is mentioned
Examples:
./clientA Racheal Greg 1
./clientB Brad Racheal 50

TXLIST
Usage:
./ClientA TXLIST
Show the list of transactions which are present in all the backend servers in a sorted fashion(sorted by transaction number). A file is created at the main server "Bradchain.txt" which contains the sorted transactions. The same information is also relayed to the clients
DIFFERENT BEHAVIOUR COMPARED TO REQUIREMENT:
The TXLIST transactions are printed on the client device along with a file being created at the main server. I did not read the requirements properly and have implemented it
Examples:
./clientA TXLIST
./clientB TXLIST

Stats
Usage:
./clientA <username> stats
Show the list of users who have had transactions with <username> in a sorted fashion(Sorted by total number of transactions with <username>). The same information is relayed to the client
Examples:
./clientA Brad stats
./clientB Greg stats

Files Present:
clientA.cpp:
Contains the complete client code in C++ for client A.Including connection to the main server and all communications with the main server.
clientA.h:
Contains the definitions and header file inclusions required for the client code execution. The definitions include the port numbers required for connection with main server
clientB.cpp:
Contains the complete client code in C++ for client B.Including connection to the main server and all communications with the main server.
clientB.h:
Contains the definitions and header file inclusions required for the client code execution. The definitions include the port numbers required for connection with main server
serverM.cpp:
Contains the complete main server code in C++. Including handling connections with both the clients as well as the three backend servers.
serverM.h:
Contains the definitions and header file inclusions required for the Main server code execution. The definitions include the port numbers required for connection with backend servers and the port numbers to connect to different clients
serverA.cpp
Contains the complete backend server code in C++ for server A. Including handling connections with the main server
serverA.h
Contains the definitions and header file inclusions required for the backend code execution. The definitions include the port numbers required for connection with main server
serverB.cpp
Contains the complete backend server code in C++ for server B. Including handling connections with the main server
serverB.h
Contains the definitions and header file inclusions required for the backend code execution. The definitions include the port numbers required for connection with main server
serverC.cpp
Contains the complete backend server code in C++ for server C. Including handling connections with the main server
serverC.h
Contains the definitions and header file inclusions required for the backend code execution. The definitions include the port numbers required for connection with main server
connection_helper.h (EXTRA FILE)
Contains the header inclusions used across the project and definitions for structures used in the project as well as helper functions used across different files


Basic Code explanation and message formats:
Balance:
When balance is requested by a client (A or B) the request is sent to the main server(#$% is appended to request to indicate end of message). The main server contacts each of the backend server and the backend servers respond with the transactions which involve the username whose balance is being checked.If the size of total transactions receieved from all the backend servers is zero then the user is not present. If the user is present then the total balance is calculated by using balance = 1000 + total received coins - total sent coins.This information is then sent back to the client which displays the details
Message Formats Used:
clienti(i=A,B) -> serverM
<username>#$% (Type:String)
serverM -> serveri(i=A,B,C)
BALANCE <username> (Type:String)
serveri(i=A,B,C) -> serverM
<list_of_filtered_transactions>(Type:Vector<struct transaction>)
serverM -> clienti(i=A,B)
<calculated_balance> (Type:String)

Example:
When ./clientA Luke is executed:
clienti(A) -> serverM
Luke#$%
serverM -> serveri(i=A,B,C)
BALANCE Luke
serveri(i=A,B,C) -> serverM
<list_of_transactions_involving_Luke>
serverM -> client(A)
500

TXCOINS:
When a transaction addition is requested by the client (A or B) the request is first sent to the main server(#$% is appended to request to indicate end of message). The main server contacts each of the backend server and the backend servers respond with the transactions the sender is involved in and then the balance is calculated. The same process is repeated for the receiver in order to confirm whether the receiver is part of the network.If both are part of the network and the sender balance is higher than or equal to the transaction value then first the main server contacts each of the backend server in order to find the maximum transaction id and then the main server sends the transaction to a randomly selected backend server with transaction id one more than the maximum. The backend server adds it to the respective block file. The transaction success/failure is then relayed back to the client which displays it on the terminal
Message formats used:
clienti(i=A,B) -> serverM
TXCOINS <username1> <username2> <value>#$%(Type:String)
serverM -> serveri(i=A,B,C)
BALANCE <username1> (Type:String)
serveri(i=A,B,C) -> serverM
<list_of_filtered_transactions>(Type:Vector<struct transaction>)
serverM -> serveri(i=A,B,C)
BALANCE <username2> (Type:String)
serveri(i=A,B,C) -> serverM
<list_of_filtered_transactions>(Type:Vector<struct transaction>)
serverM -> serveri(i=A,B,C)
MAX_TRANSACTION_VALUE (Type:String)
serveri(i=A,B,C) -> serverM
<max_transaction_id>(Type:String)
serverM -> serveri(i=AorBorC)
TXCOINS <new_transaction_id> <username1> <username2> <value> (Type:String)
serveri(i=A,B,C) -> serverM
DONE(Type:String)
serverM -> clienti(i=A,B)
DONE <calculated_updated_balance> (Type:String)

Example:
When ./clientA Luke Chinmay 100 is executed:
clienti(A) -> serverM
TXCOINS Luke Chinmay 100#$%
serverM -> serveri(i=A,B,C)
BALANCE Luke 
serveri(i=A,B,C) -> serverM
<list_of_transactions_involving_Luke>
serverM -> serveri(i=A,B,C)
BALANCE Chinmay 
serveri(i=A,B,C) -> serverM
<list_of_transactions_involving_Chinmay>
serverM -> serveri(i=A,B,C)
MAX_TRANSACTION_VALUE
serveri(i=A,B,C) -> serverM
15(Type:String)
serverM -> server(B),Random
TXCOINS 16 Luke Chinmay 100 (Type:String)
serverM -> client(A)
DONE 820

TXLIST:
When a TXLIST is requested by the client (A or B) then the request is first sent to the main server(#$% is appended to request to indicate end of message). The main server contacts each of the backend server and the backend server respond by reading and sending the complete block file to the main server. The main server writes all the transactions received from each of the backend servers to Bradchain.txt first(unsorted). The main server reads the file after it contains all the transactions and then sorts all transactions(since the sort has to be done by transaction id we just create list of max_transaction_id size and insert to required positon based on the transaction id) this list of sorted transactions is sent to the client.
Message formats used:
clienti(i=A,B) -> serverM
TXLIST#$% (Type:String)
serverM -> serveri(i=A,B,C)
TXLIST(Type:String)
serveri(i=A,B,C) -> serverM
<list_of_complete_transactions>(Type:Vector<struct transaction>)
serverM -> clienti(i=A,B)
<list_of_sorted_complete_transactions>(Type:Vector<struct transaction>)

Example:
When ./clientB TXLIST is executed:
clienti(B) -> serverM
TXLIST#$%
serverM -> serveri(i=A,B,C)
TXLIST
serveri(i=A,B,C) -> serverM
<list_of_complete_transactions>
serverM -> clienti(B)
<list_of_sorted_complete_transactions>

STATS:
When statistics for a user is requested by a client (A or B) the request is first sent to the main server(#$% is appended to request to indicate end of message). The main server contacts each of the backend servers and the backend servers respond by sending the transactions in which the user is involved in. The main server then calculates a list of statistics which contains:No of transactions with another user, Total amount transferred and the other users name,then the resultant list is sorted based on number of transactions. This list of stats is sent to the client and the client displays it on the terminal
Message Formats Used:
clienti(i=A,B) -> serverM
STATS <username>#$% (Type:String)
serverM -> serveri(i=A,B,C)
STATS <username> (Type:String)
serveri(i=A,B,C) -> serverM
<list_of_filtered_transactions>(Type:Vector<struct transaction>)
serverM -> clienti(i=A,B)
<list_of_sorted_stats> (Type:Vector<struct stat>)

Example:
When ./clientB Chinmay stats is executed:
clienti(B) -> serverM
STATS Chinmay#$%
serverM -> serveri(i=A,B,C)
STATS Chinmay
serveri(i=A,B,C) -> serverM
<list_of_transactions_involving_Chinmay>
serverM -> clienti(B)
<list_of_sorted_stats_for_chinmay>

Structures Defined/Used:
struct transaction
{
    int transaction_id;
    std::string transferor;
    std::string transferee;
    int transfer_amount;

};
struct stat
{
	std::string username;
	int number_of_transactions;
	int transfer_amount;

};

Extra Header files included:
#include <iostream> //Basic io functions in c++
#include <vector> //Vector support for list creation and usage
#include <fstream> //File stream in c++
#include <sstream> //String stream support for conversions to different formats
#include <iomanip> //IO manipulation inorder to use Bradgnment for printing

No code is directly copied/reused.  References used:
https://beej.us/guide/bgnet/html/
https://github.com/nikhilroxtomar/File-Transfer-using-UDP-Socket-in-C/
https://www.cplusplus.com/reference/cstdlib/rand/
https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/

Idiosyncrasies:
The names have to be of the same case as given in the transaction file. For example: If Balance is requested for Brad then it will not match with Brad
The parent sockets are not closed in serverM and none of the UDP sockets are closed (In serverM or in serverA/B/C).
The extra print statements for operations which are not part of the requirement.(Balance calculation for TXCOINS) are not shown in the print statements. There are no printed data about balance calculation and obtaining the maximum transaction number(according to https://piazza.com/class/kyll7qbcetu155?cid=292_f1)
Currently all the transaction details for TXLIST is sent back to the client. Only the print statements are suppressed(commented).Due to misunderstanding of  the requirements the implementation is done like this.  
Balance and TXCOINS are tested for 512 character names and work fine
TXLIST tested with 1000 total transactions added and works fine. Have not tested at a higher number





