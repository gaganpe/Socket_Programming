/*
clientB.cpp - client B code for TCP connection with the main server
*/


/***************************** Header Inclusions ********************/
#include "clientB.h"

string usage_string();
using namespace std;
int main(int argc,char * argv[]){
/***************************** Variable Declarations ******************/
	int tcp_client_socket,recv_bytes_len;
	char incoming_data_buf[NOMINAL_BUFFER_SIZE];
	char sending_string[MAX_CHAR_SIZE];
	string received_data;
	string txlist_string;
	vector <string> received_data_list;
	struct sockaddr_in server_Address;


	server_Address = map_remote_address(TCP_PORT_NUM_SERVER,remote_address);   // Map the server side address and port to a sockaddr_in structure 
	
	if(argc<2){
		//If no command line arguments are passed then the usage is invalid. Display the same and close the program
		cout<<"No operation specified:\n"<<usage_string();
		exit(1);
	}
	else{

		cout<<"The "<<CLIENT_NAME<<" is up and running.\n";
		errorchecker(tcp_client_socket = socket(AF_INET,SOCK_STREAM,0),"Socket_Creation");      //Create a TCP socket
		errorchecker(connect(tcp_client_socket,(struct sockaddr *)&server_Address,sizeof(server_Address)),"Socket_Connection");   //Connect to the main server at the specified port and IP address
		if(argc==2){
			if(strcmp(argv[1],"TXLIST")==0){
				sending_string[0] = '\0';
				strcat(sending_string,argv[1]);
				errorchecker(send(tcp_client_socket,sending_string,strlen(sending_string),0),"Socket_Send"); //If client is started with TXLIST command line argument. Request the same from Main server
			errorchecker(send(tcp_client_socket,"#$%",strlen("#$%"),0),"Socket_Send");// Indicates end of Message Request
				cout<<CLIENT_NAME<<" sent a sorted list request to the main server.\n";
				for(;;){
					memset(incoming_data_buf, 0,NOMINAL_BUFFER_SIZE);
					errorchecker(recv_bytes_len = recv(tcp_client_socket,incoming_data_buf,NOMINAL_BUFFER_SIZE,0),"Socket_Recv");//Keep receiving data in an infinite loop
					if(recv_bytes_len==0){//If number of bytes received is 0 that means that the server side child socket is closed. Stop receiving
						break;
					}
					received_data = incoming_data_buf;
					received_data.resize(recv_bytes_len);
					txlist_string+=received_data;
				}
				vector<string> output_lines = listofstrings(txlist_string,"\n");//Seperate the strings into substrings which are seperated by new line character and print in a left aligned format 
				/*cout<<setw(15)<<left<<"Serial No."
					<<" "<<setw(30)<<left<<"Sender"
					<<" "<<setw(30)<<left<<"Receiver"
					<<" "<<setw(10)<<left<<"Transfer Amount"<<endl;
				for(size_t i = 0;i<output_lines.size();i++){
					vector<string>output_transactions = listofstrings(output_lines.at(i)," ");
					if(output_transactions.size()>=4){
						cout<<setw(15)<<left<<output_transactions.at(0)
							<<" "<<setw(30)<<left<<output_transactions.at(1)
							<<" "<<setw(30)<<left<<output_transactions.at(2)
							<<" "<<setw(10)<<left<<output_transactions.at(3)<<endl;
					}
				}*/
			}
			else{
				//If total command line arguments is 1 and if its not TXLIST then balance is what is requested 
				sending_string[0] = '\0';
				strcat(sending_string,argv[1]);
				errorchecker(send(tcp_client_socket,sending_string,strlen(sending_string),0),"Socket_Send"); //Send the name of the user whose balance is requested to Main server
				errorchecker(send(tcp_client_socket,"#$%",strlen("#$%"),0),"Socket_Send");// Indicates end of Message Request

				cout<<argv[1]<<" sent a balance enquiry request to the main server.\n";
				errorchecker(recv_bytes_len = recv(tcp_client_socket,incoming_data_buf,NOMINAL_BUFFER_SIZE,0),"Socket_Recv");//Receive data from main server
				received_data = incoming_data_buf;
				received_data.resize(recv_bytes_len);
				if(received_data == "NO_USER"){ //Received data is either NO_USER or the calculated balance. Display the message based on receieved data
					cout<<"Unable to proceed with the request as "<<argv[1]<<" is not part of the network\n";
				}
				else{
					cout<<"The current balance of "<<argv[1]<<" is :\n"<<received_data<< " alicoins.\n";
				}
			}

		}
		else if(argc==3){
			if(strcmp(argv[2],"stats")==0){
				//If total command line arguments are 3 and if if third argument is stats then statistics is requested 
				sending_string[0]='\0';
				strcat(sending_string,"STATS");
				strcat(sending_string," ");
				strcat(sending_string,argv[1]);
				errorchecker(send(tcp_client_socket,sending_string,strlen(sending_string),0),"Socket_Send"); //Send STATS <username> to Main server
				errorchecker(send(tcp_client_socket,"#$%",strlen("#$%"),0),"Socket_Send");// Indicates end of Message Request
				cout<<argv[1]<<" sent a statistics enquiry request to the main server.\n";
				for(;;){
					memset(incoming_data_buf, 0,NOMINAL_BUFFER_SIZE);
					errorchecker(recv_bytes_len = recv(tcp_client_socket,incoming_data_buf,NOMINAL_BUFFER_SIZE,0),"Socket_Recv");//Keep receiving data in an infinite loop
					if(recv_bytes_len==0){//If number of bytes received is 0 that means that the server side child socket is closed. Stop receiving
						break;
					}
					received_data = incoming_data_buf;
					received_data.resize(recv_bytes_len);
					txlist_string+=received_data;
				}
				if(txlist_string == "NO_USER"){//Received data is either NO_USER or the statistics. If statistics are received print in a left aligned format
					cout<<"Unable to proceed with the request as "<<argv[1]<<" is not part of the network\n";
				}
				else{
					vector<string> output_lines = listofstrings(txlist_string,"\n");
					cout<<argv[1]<<" statistics are the following:\n";
					cout<<setw(15)<<left<<"Rank"
						<<" "<<setw(30)<<left<<"Username"
						<<" "<<setw(50)<<left<<"Number of Transactions Made with User"
						<<" "<<setw(10)<<left<<"Transfer Amount"<<endl;
					for(size_t i = 0;i<output_lines.size();i++){
						vector<string>output_stats = listofstrings(output_lines.at(i)," ");
						if(output_stats.size()>=4){
							cout<<setw(15)<<left<<output_stats.at(0)
								<<" "<<setw(30)<<left<<output_stats.at(1)
								<<" "<<setw(50)<<left<<output_stats.at(2)
								<<" "<<setw(10)<<left<<output_stats.at(3)<<endl;
						}
					}
				}
			}
			else{
				cout<<"Invalid operation specified:\n"<<usage_string();//If 3 arguments are passed and the third is not stats then the execution is invalid and the program ends
				close(tcp_client_socket);
				exit(1);

			}
		}
		else if(argc==4){
			sending_string[0] = '\0';
			strcat(sending_string,"TXCOINS");
			strcat(sending_string," ");
			strcat(sending_string,argv[1]);
			strcat(sending_string," ");
			strcat(sending_string,argv[2]);
			strcat(sending_string," ");
			strcat(sending_string,argv[3]);
			errorchecker(send(tcp_client_socket,sending_string,strlen(sending_string),0),"Socket_Send");// Send TXCOINS <username1> <username2> <value> to Main server
			errorchecker(send(tcp_client_socket,"#$%",strlen("#$%"),0),"Socket_Send");// Indicates end of Message Request
			cout<<argv[1]<<" has requested to transfer "<<argv[3]<<" coins to "<<argv[2]<<"."<<endl;
			errorchecker(recv_bytes_len = recv(tcp_client_socket,incoming_data_buf,NOMINAL_BUFFER_SIZE,0),"Socket_Recv");
			received_data = incoming_data_buf;
			received_data.resize(recv_bytes_len);
			received_data_list = listofstrings(received_data," ");
			if(received_data_list.at(0) == "NO_USER"){// If NO_USER is received check whether only one user or both users are not present in the network based on the size of the received data
				if(received_data_list.size()==2){
					cout<<"Unable to proceed with the transaction as "<<received_data_list.at(1)<<" is not part of the network.\n";

				}
				else{
					cout<<"Unable to proceed with the transaction as "<<received_data_list.at(1)<<" and "<<received_data_list.at(2)<<" are not part of the network.\n";
				}
			}
			if(received_data_list.at(0) == "NO_BAL"){// If NO_BAL is received then this indicates the sender does not have enough balance and the current balance is displayed
				cout<<argv[1]<<" was unable to transfer "<<argv[3]<<" alicoins to "<<argv[2]<<" because of insufficient balance.\n";
				cout<<"The current balance of "<<argv[1]<<" is : "<<received_data_list.at(1)<<" alicoins.\n";
			}
			if(received_data_list.at(0) == "DONE"){//If done is received then this indicates the transaction went through and the new balance after transaction is displayed
				cout<<argv[1]<<" successfully transferred "<<argv[3]<<" alicoins to "<<argv[2]<<" .\n";
				cout<<"The current balance of "<<argv[1]<<" is : "<<received_data_list.at(1)<<" alicoins.\n";
			}

		}
	}
	close(tcp_client_socket);  // Close the client socket and end the program
	return 0;
}



/************************
 *Function: usage_string
 *Function which returns the string for the correct way to run the client side code. This is printed if the client side code is executed with the wrong command line arguments
 *Parameters: None
 *Return Value: 
 *string Return the string containing the correct usage of client code
 * ********************/
string  usage_string(){
	string returnvalue = "Usage:\n"+
		string("./")+CLIENT_SHORT_NAME+string(" username -Print Balance of username\n")+
		"./"+CLIENT_SHORT_NAME+" username1 username2 value-Transfer value coins from username1 to username2\n"+
		"./"+CLIENT_SHORT_NAME+" TXLIST -Print Transaction list\n"+
		"./"+CLIENT_SHORT_NAME+" username stats -Print stats of username\n";
	return returnvalue;
}

