/*
 * connection_helper.h Header file with helper functions and all header file inclusions
 */


#ifndef CONNECTION_HELPER_H
#define CONNECTION_HELPER_H
/************************************* Header Inclusions ******************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

/*******************************Extra Header Files******************/
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>



/********************************Defines***************************/
#define localhost "127.0.0.1"
#define MAX_CHAR_SIZE 1040
#define NOMINAL_BUFFER_SIZE 256



using namespace std;
using std::vector;
/********************************Declarations*********************/
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
vector<string> listofstrings(string received_string,string seperator);
struct transaction stringtotransaction(string received_string);
void udp_socket_setup(int  *udp_socket_desc,struct sockaddr_in  connection_address);
struct sockaddr_in  map_remote_address(int  port_number,char * remote_ipaddress);
struct sockaddr_in  map_local_address(int  port_number);//Function to Map localhost IP and port number to a "sockaddr_in" structure
string removewhitespaces(string original);
vector<transaction> generate_transaction_list(string filename);
string transaction_to_string(struct transaction original);
string stat_to_string(struct stat original);
int stringtoint_com(string int_string);
string  inttostring_com(int received_int);
void errorchecker(int value,string printstring);
/********************************Helper Functions*****************/


/************************
 *Function: listofstrings
 *Function which returns a list of strings obtained from an original string. This list is obtained by splitting the original string based on the seperator provided 
 *Parameters: 
 *string received_string: The original string received
 *string seperator: The seperator character based on which the list has to be created
 *Return Value: 
 *vector<string>: Vector containing the list of strings seperated by seperator
 **********************/
vector<string> listofstrings(string received_string,string seperator){
	vector<string> seperated_substring_list;
	size_t position = 0;
	while ((position = received_string.find(seperator)) != string::npos) {
		seperated_substring_list.push_back(received_string.substr(0, position));
		received_string.erase(0, position + seperator.length());
	}
	if(!received_string.empty()){
		seperated_substring_list.push_back(received_string);
	}
	return seperated_substring_list;
}
/************************
 *Function: removewhitespaces
 *Function which removes all spaces and new line characters from a string
 *Parameters: 
 *string original: The original string received
 *Return Value: 
 *string: String in which all whitespaces and new line characters are removed
 **********************/
string removewhitespaces(string original){
	size_t position=0;
	while((position=original.find('\n'))!=string::npos){
		original.erase(position);
	}
	while((position=original.find(' '))!=string::npos){
		original.erase(position);
	}
	return original;
}
/************************
 *Function: stringtoint_com
 *Function which converts a received string into an integer
 *Parameters: 
 *string int_string: The original string which is an integer
 *Return Value: 
 *int: The interger which is obtained after conversion
 **********************/
int stringtoint_com(string int_string){
	static stringstream strstrm;
	static int retval;
	retval = 0;
	strstrm.clear();
	strstrm.str("");
	strstrm<<int_string;
	strstrm>>retval;
	return retval;
}
/************************
 *Function: inttostring_com
 *Function which converts a received integer into a string
 *Parameters: 
 *int int_string: The original integer
 *Return Value: 
 *string: The string which is obtained after conversion
 **********************/
string  inttostring_com(int received_int){
	static stringstream strstrm;
	strstrm.clear();
	strstrm.str("");
	strstrm<<received_int;
	return strstrm.str();
}
/************************
 *Function: stringtotransaction
 *Function which converts a received string into a transaction structure
 *Parameters: 
 *string received_string: The original string which has to be converted into a transaction structure
 *Return Value: 
 *struct transaction: The transaction structure which is obtained after conversion
 **********************/
struct transaction stringtotransaction(string received_string){
	struct transaction retval;
	string temporary;
	vector<string> seperated_substrings;
	if (!received_string.empty()){
		seperated_substrings = listofstrings(received_string,std::string(" "));//Seperate string based on spaces
		if(seperated_substrings.size()!=4){//If size is not 4 it means its not a transaction string. Return empty string
			return retval;
		}
		else{
			temporary = removewhitespaces(seperated_substrings.at(0));//Obtain the different members of the transaction structure and add it to a transaction
			retval.transaction_id = stringtoint_com(temporary);
			retval.transferor = removewhitespaces(seperated_substrings.at(1));
			retval.transferee = removewhitespaces(seperated_substrings.at(2));
			temporary = removewhitespaces(seperated_substrings.at(3));
			retval.transfer_amount = stringtoint_com(temporary);
			return retval;

		}
	}
	return retval;


}
/************************
 *Function: map_local_address
 *Function which maps the local IP address of the device and the port number to a sockaddr_in structure and returns the structure
 *Parameters: 
 *int  port_number: The port number which has to be mapped into the address structure
 *Return Value: 
 *struct sockaddr_in: The sockaddr_in structure which has to be returned
 **********************/
struct sockaddr_in  map_local_address(int  port_number){
	struct sockaddr_in address_local;
	address_local.sin_family = AF_INET;
	inet_pton(AF_INET,localhost,&address_local.sin_addr.s_addr);
	address_local.sin_port = htons(port_number);
	return address_local; 
}
/************************
 *Function: map_remote_address
 *Function which maps the given IP address of a device and the port number to a sockaddr_in structure and returns the structure
 *Parameters: 
 *int  port_number: The port number which has to be mapped into the address structure
 *char * remote_ipaddress: IP address which has to be mapped to the sockaddr_in structure
 *Return Value: 
 *struct sockaddr_in: The sockaddr_in structure which has to be returned
 **********************/
struct sockaddr_in  map_remote_address(int  port_number,char * remote_ipaddress){
	struct sockaddr_in address_local;
	address_local.sin_family = AF_INET;
	inet_pton(AF_INET,remote_ipaddress,&address_local.sin_addr.s_addr);
	address_local.sin_port = htons(port_number);
	return address_local; 
}
/************************
 *Function: udp_socket_setup
 *Function which sets up a UDP socket with a specific address and a socket descriptor
 *Parameters: 
 *int  *udp_socket_desc: The address to udp socket descriptor used to map a specific address to  
 *struct sockaddr_in connection_address: The address structure to be mapped to a udp socket
 *Return Value:None 
 **********************/
void udp_socket_setup(int  *udp_socket_desc,struct sockaddr_in connection_address){
	int true_value = 1;
	errorchecker((*udp_socket_desc = socket(AF_INET, SOCK_DGRAM,0)),"Socket_Creation");
        errorchecker(setsockopt(*udp_socket_desc, SOL_SOCKET, SO_REUSEADDR, &true_value, sizeof(int)),"Socket_setsockopt");
	errorchecker((bind(*udp_socket_desc,(struct sockaddr *)&connection_address,sizeof(connection_address))),"Socket_bind");
	return;
}
/************************
 *Function: generate_transaction_list
 *Function which returns a list of transactions which are read from a file 
 *Parameters: 
 *string filename: The name of the file which has to be read   
 *Return Value: 
*vector<transaction>: List of transactions read from the file
 **********************/
vector<transaction> generate_transaction_list(string filename){
	std::ifstream blockfilestream(filename.c_str());
	vector<transaction> transaction_list;
	struct transaction transaction_placeholder;
	while(blockfilestream >>transaction_placeholder.transaction_id>>transaction_placeholder.transferor>>transaction_placeholder.transferee>>transaction_placeholder.transfer_amount){
		transaction_list.push_back(transaction_placeholder);
	}
	blockfilestream.close();
	return transaction_list;
}
/************************
 *Function: transaction_to_string
 *Function which returns a string which is obtained after converting a transaction into a string(All parameters seperated by space)
 *Parameters: 
 *struct transaction original: The transaction which has to be converted  
 *Return Value: 
*string: String obtained after conversion 
 **********************/
string transaction_to_string(struct transaction original){
		return inttostring_com(original.transaction_id)+" "+original.transferor+" "+original.transferee+" "+inttostring_com(original.transfer_amount);


}
/************************
 *Function: stat_to_string
 *Function which returns a string which is obtained after converting a stat structure into a string(All parameters seperated by space)
 *Parameters: 
 *struct stat original: The stat structure of the file which has to be read   
 *Return Value: 
*string: String obtained after conversion 
 **********************/
string stat_to_string(struct stat original){
		return (original.username)+" "+inttostring_com(original.number_of_transactions)+" "+inttostring_com(original.transfer_amount);


}
/************************
 *Function: errorchecker
 *Function which checks if the return value of function is in error and prints the respective string to indicate the error
 *Parameters: 
 *int value: The value which has to be checked to be in error   
 *string printstring: The string which has to be printed in order to indicate the error 
 *Return Value:None
 **********************/
void errorchecker(int value,string printstring){
	if(value==-1){
		perror(printstring.c_str());
		exit(1);
	}
	return;
}
#endif /***CONNECTION_HELPER_H***/
