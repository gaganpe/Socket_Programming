/*
*** serverA.cpp - Backend server code for server A to respond to queries from main server 
*/

/***************** includes *******************************/
#include "serverA.h"


using namespace std;
using  std::vector;
/***************** Function Declarations *******************************/
void send_balance_transactions(int sockdes,struct sockaddr_in  send_address,int send_address_size,vector<transaction> transaction_list,string username);
void send_maxtransaction_value(int sockdes,struct sockaddr_in send_address,int send_address_size,vector<transaction>transaction_list);
void send_txcoins_response(int sockdes,struct sockaddr_in send_address,int send_address_size,vector<string>string_list);
void send_txlist_response(int sockdes,struct sockaddr_in send_address,int send_address_size);
void send_stats_transactions(int sockdes,struct sockaddr_in  send_address,int send_address_size,vector<transaction> transaction_list,string username);

int main(){
	int udp_sockdes_server; //UDP socket descriptor to connect to ServerM 
	struct sockaddr_in address_connect_serv,address_incoming_connection_temp,address_local_temp;
	address_connect_serv = map_local_address(UDP_PORT_NUM_LOCAL);// Map localhost IP and Port Number for connecting to server A,B,C 
	int address_size_incoming = sizeof(address_incoming_connection_temp);
	int address_size_local = sizeof(address_local_temp);
	char  buffer_incoming_M[MAX_CHAR_SIZE];
	string string_incoming_M;
	vector<string> incoming_string_list;
	udp_socket_setup(&udp_sockdes_server,address_connect_serv);
	errorchecker(getsockname(udp_sockdes_server,(struct sockaddr*)&address_local_temp, (socklen_t *)&address_size_local),"Socket_getsockname");
	cout<<"The "<<SERVER_NAME<<" is up and running using UDP on port "<<ntohs(address_local_temp.sin_port)<<" .\n";
	for(;;){
		memset(buffer_incoming_M, 0,MAX_CHAR_SIZE);

		recvfrom(udp_sockdes_server, buffer_incoming_M, sizeof buffer_incoming_M, 0, (sockaddr *)&address_incoming_connection_temp, (socklen_t *)&address_size_incoming);//Keep waiting for requests from main server
		string_incoming_M = buffer_incoming_M;
		incoming_string_list = listofstrings(string_incoming_M,std::string(" "));//Divide the received string into substrings which are seperated by spaces
		vector<transaction> transaction_list = generate_transaction_list(TRANSACTION_FILE_NAME);
		if(incoming_string_list.at(0) == "BALANCE"){//If the received string has BALANCE at the start then balance of a user is requested,call the respective function
			cout<<"The "<<SERVER_NAME<<" received a request from the Main Server.\n";
			send_balance_transactions(udp_sockdes_server,address_incoming_connection_temp,address_size_incoming,transaction_list,incoming_string_list.at(1));
			cout<<"The "<<SERVER_NAME<<" finished sending the response to the Main Server."<<endl;
		}
		if(incoming_string_list.at(0) == "BALANCENOPR"){//This is used to differentiate between printed and no print balance queries. If BALANCENOPR is called no print statements will appear in the console on the main server. The response is same. Currently not used
			send_balance_transactions(udp_sockdes_server,address_incoming_connection_temp,address_size_incoming,transaction_list,incoming_string_list.at(1));
		}
		if(incoming_string_list.at(0) == "STATS"){//If the received string has STATS at the start then statistics of a user is requested,call the respective function
			cout<<"The "<<SERVER_NAME<<" received a request from the Main Server.\n";
			send_stats_transactions(udp_sockdes_server,address_incoming_connection_temp,address_size_incoming,transaction_list,incoming_string_list.at(1));
			cout<<"The "<<SERVER_NAME<<" finished sending the response to the Main Server."<<endl;
		}
		if(incoming_string_list.at(0)== "MAX_TRANSACTION_VALUE"){//If the received string is MAX_TRANSACTION_VALUE then the max transaction id is requested,Call the respective function
			send_maxtransaction_value(udp_sockdes_server,address_incoming_connection_temp,address_size_incoming,transaction_list);
		}
		if(incoming_string_list.at(0)== "TXCOINS"){//If the received string has TXCOINS at the start then a transaction has to be added,call the respective function
			cout<<"The "<<SERVER_NAME<<" received a request from the Main Server.\n";
			send_txcoins_response(udp_sockdes_server,address_incoming_connection_temp,address_size_incoming,incoming_string_list);
			cout<<"The "<<SERVER_NAME<<" finished sending the response to the Main Server."<<endl;
		}
		if(incoming_string_list.at(0)== "TXLIST"){//If the received string has TXCOINS at the start then a transaction has to be added,call the respective function
			cout<<"The "<<SERVER_NAME<<" received a request from the Main Server.\n";
			send_txlist_response(udp_sockdes_server,address_incoming_connection_temp,address_size_incoming);
			cout<<"The "<<SERVER_NAME<<" finished sending the response to the Main Server."<<endl;
		}
		
	}
	return 0;
}



/************************
 *Function: send_balance_transactions
 *Function which sends response for BALANCE operation from backend server (All transactions invovling specific user)
 *Parameters: 
 *int sockdes: UDP socket descriptor to connect to the main server
 *struct sockaddr_in send_address: The address to which the data has to be sent to
 *int send_address_size: Size of the sockaddr_in structure 
 *vector<transaction>transaction_list: Vector containing the complete transactions in the block file corresponding to this backend server
 *string username:Username whose transactions have to be sent
 *Return Value:None 
 **********************/
void send_balance_transactions(int sockdes,struct sockaddr_in send_address,int send_address_size,vector<transaction> transaction_list,string username){
	string response_string;
	for (size_t i = 0; i < transaction_list.size(); i++) {
		if((transaction_list[i].transferor==username)||(transaction_list[i].transferee==username)){
			response_string += transaction_to_string(transaction_list[i])+"\n";  //Append all transactions from list into a string variable
		}
	}
	int response_string_length = response_string.length();
	errorchecker(sendto(sockdes,inttostring_com(response_string_length).c_str(),inttostring_com(response_string_length).length(),0,(struct sockaddr *)&send_address,(socklen_t)(send_address_size)),"Socket_sendto"); //Send the size of the string first in order to tell the main server how much data it will be receiving 
	while(!response_string.empty()){
		string response_substring;
		if(response_string.length()<1000){//Send the string 1000 characters at a time 
			response_substring = response_string;
			response_string.clear();
		}	
		else{
			response_substring = response_string.substr(0,1000);
			response_string.erase(0,1000);
		}
		errorchecker(sendto(sockdes,response_substring.c_str(),response_substring.length(),0,(struct sockaddr *)&send_address,(socklen_t)(send_address_size)),"Socket_sendto");
	}
	return;

}
/************************
 *Function: send_stats_transactions
 *Function which sends response for STATS operation from backend server (All transactions invovling specific user)
 *Parameters: 
 *int sockdes: UDP socket descriptor to connect to the main server
 *struct sockaddr_in send_address: The address to which the data has to be sent to
 *int send_address_size: Size of the sockaddr_in structure 
 *vector<transaction>transaction_list: Vector containing the complete transactions in the block file corresponding to this backend server
 *string username:Username whose transactions have to be sent
 *Return Value:None 
 **********************/
void send_stats_transactions(int sockdes,struct sockaddr_in send_address,int send_address_size,vector<transaction> transaction_list,string username){
			send_balance_transactions(sockdes,send_address,send_address_size,transaction_list,username);//The operation is same as balance calculation. So Balance calculation function is called
			return;
}
/************************
 *Function: send_maxtransaction_value
 *Function which sends response with the maximum transaction id present in the file 
 *Parameters: 
 *int sockdes: UDP socket descriptor to connect to the main server
 *struct sockaddr_in send_address: The address to which the data has to be sent to
 *int send_address_size: Size of the sockaddr_in structure 
 *vector<transaction>transaction_list: Vector containing the complete transactions in the block file corresponding to this backend server
 *Return Value:None 
 **********************/
void send_maxtransaction_value(int sockdes,struct sockaddr_in send_address,int send_address_size,vector<transaction>transaction_list){
	int maxvalue = 0;
	string response_string;
	for(size_t i=0;i<transaction_list.size();i++){
		if(maxvalue<transaction_list.at(i).transaction_id){
			maxvalue = (transaction_list.at(i).transaction_id);//Loop through all transactions and find the max value
		}
	}
	response_string = inttostring_com(maxvalue)+"\0";
	errorchecker(sendto(sockdes,response_string.c_str(),response_string.length(),0,(struct sockaddr *)&send_address,(socklen_t)(send_address_size)),"Socket_sendto");
	return;
}
/************************
 *Function: send_txcoins_response
 *Function which sends response for TXCOINS operation from backend server after addition to the block file
 *Parameters: 
 *int sockdes: UDP socket descriptor to connect to the main server
 *struct sockaddr_in send_address: The address to which the data has to be sent to
 *int send_address_size: Size of the sockaddr_in structure 
 *vector<string>string_list: Vector containing the transaction details
 *Return Value:None 
 **********************/
void send_txcoins_response(int sockdes,struct sockaddr_in send_address,int send_address_size,vector<string>string_list){
	const char * response_string="DONE" ;
	string writestring = string_list.at(1)+" "+string_list.at(2)+" "+string_list.at(3)+" "+string_list.at(4)+"\n";//Convert the received list into string 
	std::ifstream in(TRANSACTION_FILE_NAME);
	std::string line, text;
	while (std::getline(in, line)){
		if ((!line.empty()) && (line.find_first_not_of(' ') != std::string::npos))//Read the file to remove all empty lines
			text += line + "\n";
	}
	in.close();
	text+=writestring;
	std::ofstream out(TRANSACTION_FILE_NAME);
	out << text;//Write to file with the new transaction added
	out.close();
	errorchecker(sendto(sockdes,response_string,strlen(response_string),0,(struct sockaddr *)&send_address,(socklen_t)(send_address_size)),"Socket_sendto") ;//Send DONE once the transaction is added
	return;
}
/************************
 *Function: send_txlist_response
 *Function which sends response for TXLIST operation from backend server
 *Parameters: 
 *int sockdes: UDP socket descriptor to connect to the main server
 *struct sockaddr_in send_address: The address to which the data has to be sent to
 *int send_address_size: Size of the sockaddr_in structure 
 *Return Value:None 
 **********************/
void send_txlist_response(int sockdes,struct sockaddr_in send_address,int send_address_size){
	char file_buf[MAX_CHAR_SIZE];
	FILE * file_pointer = fopen(TRANSACTION_FILE_NAME,"r");
	if(file_pointer==NULL){
		perror("FileOpen:");
		exit(1);
	}
	memset(file_buf, 0,MAX_CHAR_SIZE);
	for(;fgets(file_buf,MAX_CHAR_SIZE,file_pointer)!=NULL;){//Read from file and send the complete file
		errorchecker((sendto(sockdes,file_buf,1024,0,(struct sockaddr*)&send_address,(socklen_t)(send_address_size))),"Socket_sendto");
		usleep(2);//Delay of 2 microseconds added in order to not overwhelm the receiver since the transmission is not happening properly for >1000 transactions
		memset(file_buf, 0,MAX_CHAR_SIZE);
	}
	fclose(file_pointer);
	errorchecker(sendto(sockdes,"DONE",strlen("DONE"),0,(struct sockaddr*)&send_address,(socklen_t)(send_address_size)),"Socket_sendto");//Send DONE once the complete file is sent 
	return;

}
