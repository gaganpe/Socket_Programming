/*
*** serverM.cpp - Main server code to receive requests from client and respond to it after contacting backend servers and performing calculations
*/

/***************** includes *******************************/
#include "serverM.h"


using namespace std;
using std::vector;
/***************** Function Declarations *******************************/
void tcp_parent_socket_setup(int  *parent_sock_desc_addr,struct sockaddr_in  connection_address);
void client_request_handler(int tcp_parent_sockdes,int udp_sockdes,bool is_clientA);
string balance_calculation(int udp_sockdes,string server_request,int suppress_print);
string new_transaction(int udp_sockdes,vector <string> server_request);
string generate_max_transaction_id(int udp_sockdes);
vector <transaction> txlist_op(int udp_sockdes);
vector <transaction> sort_transactions(vector<transaction> original,string maxvalue);
vector<stat> user_stat_calculation(int udp_sockdes,string server_request);
bool statcompare(struct stat a,struct stat b);
bool str_ends_with(string original_string,string substring);
int main(){
	fd_set multi_socket_fd,functional_multi_socket_fd;
	int tcp_parent_sockdes_A; //TCP socket Descriptors for parent and child socket for client A
	int tcp_parent_sockdes_B; //TCP socket Descriptors for parent and child socket for client B
	int udp_sockdes_server; //UDP socket descriptor to connect to Server A,B,C
	struct sockaddr_in address_connectA,address_connectB,address_connect_serv;
	address_connectA = map_local_address(TCP_PORT_NUM_CLIENTA);// Map localhost IP and Port Number for client A to an socket Address structure 
	address_connectB = map_local_address(TCP_PORT_NUM_CLIENTB);// Map localhost IP and Port Number for client B to an socket Address structure 
	address_connect_serv = map_local_address(UDP_PORT_NUM_LOCAL);// Map localhost IP and Port Number for connecting to server A,B,C 
	tcp_parent_socket_setup(&tcp_parent_sockdes_A,address_connectA);
	tcp_parent_socket_setup(&tcp_parent_sockdes_B,address_connectB);
	udp_socket_setup(&udp_sockdes_server,address_connect_serv);
	cout<<"The main server is up and running.\n";
	FD_ZERO(&multi_socket_fd);
	FD_SET(tcp_parent_sockdes_A,&multi_socket_fd);
	FD_SET(tcp_parent_sockdes_B,&multi_socket_fd); //select() api used to obtain requests from client A and B parallely
	for(;;){
		functional_multi_socket_fd = multi_socket_fd;
		errorchecker(select(FD_SETSIZE,&functional_multi_socket_fd,NULL,NULL,NULL),"Select_Socket");
		if(FD_ISSET(tcp_parent_sockdes_A, &functional_multi_socket_fd)){
			client_request_handler(tcp_parent_sockdes_A,udp_sockdes_server,true); // Client request handler with true as last parameter to indicate client A
		}
		if(FD_ISSET(tcp_parent_sockdes_B, &functional_multi_socket_fd)){
			client_request_handler(tcp_parent_sockdes_B,udp_sockdes_server,false);// Client request handler with false as last parameter to indicate client B
		}
	}

	return 0;
}
/************************
 *Function: tcp_parent_socket_setup
 *Function used to setup a tcp parent socket with a given port number and ip address
 *Parameters: 
 *int  *parent_sock_desc_addr: Socket descriptor to TCP parent socket. 
 *,struct sockaddr_in  connection_address: The address to be mapped to the parent socket
 *Return Value:None
 **********************/
void tcp_parent_socket_setup(int  *parent_sock_desc_addr,struct sockaddr_in  connection_address){
	int true_value = 1;
	errorchecker(*parent_sock_desc_addr = socket(AF_INET, SOCK_STREAM,0),"Socket_Creation");
        errorchecker(setsockopt(*parent_sock_desc_addr, SOL_SOCKET, SO_REUSEADDR, &true_value, sizeof(int)),"Socket_setsockopt") ;
	errorchecker(bind(*parent_sock_desc_addr,(struct sockaddr *)&connection_address,sizeof(connection_address)),"Socket_Bind");
	errorchecker(listen(*parent_sock_desc_addr,10),"Socket_Listen");
	return;
}


/************************
 *Function: balance_calculation
 *Function used to obtain the balance of a user from a network. Will return NO_USER if user is not part of the network
 *Parameters: 
 *int udp_sockdes: Socket descriptor to connect to backend server. 
 *string received_username: The username whose balance has to be checked which is sent by the client
 *int suppress_print: Used to suppress the print statements when connecting to backend servers
 *Return Value: 
 *string The result of the balance query from the backend servers
 **********************/
string balance_calculation(int udp_sockdes,string received_username,int suppress_print){
	string server_request = "BALANCE " +received_username+'\0'; //The request message if framed as BALANCE <username>
	if(suppress_print ==1){
		server_request = "BALANCENOPR "+received_username+'\0';
	}
	int received_bytes = 0;
	struct sockaddr_in server_address,address_incoming_serv_temp;
	int address_size_incoming_serv = sizeof(address_incoming_serv_temp);
	char  buffer_incoming_serv[MAX_CHAR_SIZE];
	memset(buffer_incoming_serv,0,MAX_CHAR_SIZE);
	string balance("");
	string intermediate_string;
	vector <string> list_of_transactions_string;
	vector <transaction>list_of_transactions_complete;
	int int_balance = 1000,temporary_transfer_amount;
	for(int i=0;i<3;i++){//Contact the three individual servers in a loop
		server_address = map_remote_address(server_port_array[i],server_ipaddress_array[i]);//Map the remote address of the respective backend server
		balance.clear();
		string contacted_server = (i==0)?"A":((i==1)?"B":"C");
		errorchecker((sendto(udp_sockdes,server_request.c_str(),server_request.length() ,0,(struct sockaddr *)&server_address,(socklen_t)(sizeof(server_address) ))),"Socket_sendto") ;
		if(suppress_print!=1){
			cout<<"The main server sent a request to server "<<contacted_server<<" .\n";
		}

		errorchecker((received_bytes = recvfrom(udp_sockdes, buffer_incoming_serv, sizeof buffer_incoming_serv, 0, (sockaddr *)&address_incoming_serv_temp, (socklen_t *)&address_size_incoming_serv)),"Socket_recvfrom");
		intermediate_string = buffer_incoming_serv;
		intermediate_string.resize(received_bytes);
		size_t total_mesg_length = stringtoint_com(intermediate_string);//Obtain the length of message
		while(balance.length()!=total_mesg_length){//Receive length equivalent to the message length
			memset(buffer_incoming_serv,0,MAX_CHAR_SIZE);
			errorchecker((received_bytes = recvfrom(udp_sockdes, buffer_incoming_serv, sizeof buffer_incoming_serv, 0, (sockaddr *)&address_incoming_serv_temp, (socklen_t *)&address_size_incoming_serv)),"Socket_recvfrom");
			intermediate_string = buffer_incoming_serv;
			intermediate_string.resize(received_bytes);
			balance+=intermediate_string;
		}
		if(suppress_print!=1){
			cout<<"The main server received transactions from Server "<<contacted_server<<" using UDP over port "<<ntohs(address_incoming_serv_temp.sin_port)<<" .\n";
		}
		list_of_transactions_string = listofstrings(balance,"\n");
		memset(buffer_incoming_serv,0,MAX_CHAR_SIZE);
		for(size_t j=0;j<list_of_transactions_string.size();j++){
			list_of_transactions_complete.push_back(stringtotransaction(list_of_transactions_string.at(j)));
		}

	}		
	if(list_of_transactions_complete.size()==0){//If no transactions are received the user is not present in the network
		return std::string("NO_USER"); 
	}
	else{//If transactions are present then find the balance with an offset of initial balance of 1000 
		for (size_t k=0;k<list_of_transactions_complete.size();k++){
			if(list_of_transactions_complete.at(k).transferor == received_username){
				temporary_transfer_amount = (list_of_transactions_complete.at(k).transfer_amount);
				int_balance -= temporary_transfer_amount;
			}
			if(list_of_transactions_complete.at(k).transferee == received_username){
				temporary_transfer_amount = (list_of_transactions_complete.at(k).transfer_amount);
				int_balance += temporary_transfer_amount;
			}
		}
	}
	return inttostring_com(int_balance)+"\0"; //Return the balance
}
/************************
 *Function: new_transaction
 *Function used to perform check and add new transaction to a random server if possible 
 *Parameters: 
 *int udp_sockdes: Socket descriptor to connect to backend server. 
 *vector <string> server_request: The request sent by the client
 *Return Value: 
 *string The result of trying to add the new transaction. Will return no balance,no user or success based on the successful nature of addition of transaction 
 **********************/
string new_transaction(int udp_sockdes,vector <string> server_request){
	srand (time(NULL));
	int random_num = rand()%3; //Generate random number between 0-2 in order to add transaction to a random server upon being successful
	struct sockaddr_in address_connect_serv,address_incoming_connection_temp;
	int address_size_incoming;
	int sender_no_user=0;
	char  buffer_incoming_serv[20];
	int received_bytes,sender_bal,transfer_amt;
	string contacted_server = (random_num==0)?"A":((random_num==1)?"B":"C");
	string returnvalue = balance_calculation(udp_sockdes, server_request.at(1),1),temp_returnvalue;//Check balance of sender
	if(returnvalue == "NO_USER"){//If sender does not exist then transaction is not possible
		temp_returnvalue =  "NO_USER "+server_request.at(1);
		sender_no_user = 1;
	}
	else{
		sender_bal = stringtoint_com(returnvalue);//If sender exists then obtain the sender balnace
	}
	transfer_amt = stringtoint_com(server_request.at(3));
	returnvalue = balance_calculation(udp_sockdes, server_request.at(2),1);//Check balance of receiver
	if(returnvalue == "NO_USER"){//If receiver does not exist then transaction is not possible
		if(sender_no_user==1){
			returnvalue= temp_returnvalue+std::string(" ")+server_request.at(2)+"\0";
		}
		else{
			returnvalue = "NO_USER "+server_request.at(2)+"\0";
		}
		return returnvalue;//Return if there is no sender and/or receiver
	}
	if(sender_no_user){
		returnvalue=temp_returnvalue+"\0";//Return if there is no sender 
		return returnvalue;
	}
	if(sender_bal<transfer_amt)
	{
		returnvalue =  std::string("NO_BAL")+ std::string(" ")+inttostring_com(sender_bal);//Return if the sender balance is less than the transaction amount
		return returnvalue;
	}
	string maxvalue = generate_max_transaction_id(udp_sockdes);//Obtain the maximum transaction id 
	string request;
	server_request.insert(server_request.begin() + 1, maxvalue);//Make the server request as TXCOINS <new_transaction_id> <sender> <receiver> <transfer_amount>
	for(size_t i = 0;i<server_request.size();i++){
		if(request.empty()){
			request += server_request.at(i);
		}
		else{
			request+=(" " + server_request.at(i));
		}
	}
	request+='\0';//Obtain the string of the request
	address_connect_serv = map_remote_address(server_port_array[random_num],server_ipaddress_array[random_num]);// Connect to a random server
	errorchecker((sendto(udp_sockdes,request.c_str(),request.length() ,0,(struct sockaddr *)&address_connect_serv,(socklen_t)(sizeof(address_connect_serv) ))),"Socket_sendto");//Send request to server
	cout<<"The main server sent a request to server "<<contacted_server<<" .\n";
	errorchecker((received_bytes = recvfrom(udp_sockdes, buffer_incoming_serv, sizeof buffer_incoming_serv, 0, (sockaddr *)&address_incoming_connection_temp, (socklen_t *)&address_size_incoming)),"Socket_recvfrom");//Obtain response from server
	cout<<"The main server received feedback from Server "<<contacted_server<<" using UDP over port "<<ntohs(address_incoming_connection_temp.sin_port)<<" .\n";
	returnvalue = buffer_incoming_serv;
	returnvalue.resize(received_bytes);
	returnvalue+=" ";
	returnvalue+= inttostring_com(sender_bal-transfer_amt);//Append the new updated balance of the user to the return value
	return returnvalue;//Return the status of transaction 
}
/************************
 *Function: generate_max_transaction_id
 *Function used to obtain maximum transaction id inorder to add new transaction or to sort the received transastions 
 *Parameters: 
 *int udp_sockdes: Socket descriptor to connect to backend server. 
 *Return Value: 
 *string Final maximum value of all the transaction ids accross all the 3 backend servers which is incremented by 1.
 **********************/
string generate_max_transaction_id(int udp_sockdes){
	string server_request = "MAX_TRANSACTION_VALUE\0"; //Request MAX_TRANSACTION_VALUE in order to obtain the maximum transaction value from each backend server
	string maxval;
	int received_bytes = 0;
	struct sockaddr_in server_address,address_incoming_serv_temp;
	int address_size_incoming_serv = sizeof(address_incoming_serv_temp);
	char  buffer_incoming_serv[NOMINAL_BUFFER_SIZE];
	memset(buffer_incoming_serv,0,NOMINAL_BUFFER_SIZE);
	vector <string> list_of_maxes;
	int maxval_int=0;
	for(int i=0;i<3;i++){//Contact the three individual servers in a loop
		server_address = map_remote_address(server_port_array[i],server_ipaddress_array[i]);//Map the remote address of the respective backend server
		memset(buffer_incoming_serv,0,NOMINAL_BUFFER_SIZE);
		errorchecker((sendto(udp_sockdes,server_request.c_str(),server_request.length() ,0,(struct sockaddr *)&server_address,(socklen_t)(sizeof(server_address) ))),"Socket_sendto"); //Send request to server
		errorchecker((received_bytes = recvfrom(udp_sockdes, buffer_incoming_serv, sizeof buffer_incoming_serv, 0, (sockaddr *)&address_incoming_serv_temp, (socklen_t *)&address_size_incoming_serv)),"Socket_recvfrom");//Receive the maximum value of transaction id
		maxval = buffer_incoming_serv;
		list_of_maxes.push_back(maxval);
	}
	for(size_t j=0;j<list_of_maxes.size();j++){
		if(maxval_int<stringtoint_com(list_of_maxes.at(j))){//Loop through all maximums and find the global maximum
			maxval_int = 	stringtoint_com(list_of_maxes.at(j));
		}
	}
	return inttostring_com(maxval_int+1);//Add +1 to get the new transaction Id
}

/************************
 *Function: txlist_op
 *Function used to obtain list of transactions from backend servers and save it in a file and return the same to the client 
 *Parameters: 
 *int udp_sockdes: Socket descriptor to connect to backend server. 
 *Return Value: 
 *vector <transaction> List of all transactions stored in all the backend servers.
 **********************/
vector<transaction> txlist_op(int udp_sockdes){
	string server_request = "TXLIST\0";//Create request message TXLIST which is sent to the backend servers
	string value,ending_data;
	int received_bytes = 0;
	struct sockaddr_in server_address,address_incoming_serv_temp;
	int address_size_incoming_serv = sizeof(address_incoming_serv_temp);
	char  buffer_incoming_serv[MAX_CHAR_SIZE];
	memset(buffer_incoming_serv,0,MAX_CHAR_SIZE);
	FILE* file_pointer = fopen(GENERATED_TRANSACTION_FILE,"w");//Open "alichain.txt" in write mode
	for(int i=0;i<3;i++){//Contact the three individual servers in a loop
		string contacted_server = (i==0)?"A":((i==1)?"B":"C");
		server_address = map_remote_address(server_port_array[i],server_ipaddress_array[i]);//Map the remote address of the respective backend server
		errorchecker((sendto(udp_sockdes,server_request.c_str(),server_request.length() ,0,(struct sockaddr *)&server_address,(socklen_t)(sizeof(server_address) ))),"Socket_sendto");//Send request to server
		cout<<"The main server sent a request to server "<<contacted_server<<" .\n";
		for(;;){
			memset(buffer_incoming_serv,0,MAX_CHAR_SIZE);
			errorchecker((received_bytes = recvfrom(udp_sockdes, buffer_incoming_serv, sizeof buffer_incoming_serv, 0, (sockaddr *)&address_incoming_serv_temp, (socklen_t *)&address_size_incoming_serv)),"Socket_recvfrom");
			value = buffer_incoming_serv; value.resize(received_bytes);
			ending_data += value;
			if(str_ends_with(ending_data,"DONE")){//If the string ends with DONE then the whole file is received,exit the loop
				ending_data.clear();
				break;
			}
			if(!(str_ends_with(ending_data,"D")||str_ends_with(ending_data,"DO")||str_ends_with(ending_data,"DON"))){//If the string does not end with any of substrings of DONE don't clear it and keep the data otherwise clear and check in the next received data 
				ending_data.clear();
			}
			fprintf(file_pointer,"%s",buffer_incoming_serv);//Write received data to file 
		}
		cout<<"The main server received transactions from Server "<<contacted_server<<" using UDP over port "<<ntohs(address_incoming_serv_temp.sin_port)<<" .\n";
	}
	fclose(file_pointer);//Close the file pointer
	vector <transaction> final_transaction_list = generate_transaction_list(GENERATED_TRANSACTION_FILE); //Read the transactions from file and write to a list of transactions 
	string maxval = generate_max_transaction_id(udp_sockdes);//Obtain the maximum transaction id from the servers 
	vector <transaction>final_transaction_list_sorted = sort_transactions(final_transaction_list,maxval);//Sort transactions which are present in the file based on transaction_id	
	int transaction_length = final_transaction_list_sorted.size();
	int removed_value=0;
	for(int i=0;i<transaction_length;i++){
		if(final_transaction_list_sorted.at(i-removed_value).transaction_id==0){//Remove invalid transactions from the list(Transactions  with transaction id =0)
			final_transaction_list_sorted.erase(final_transaction_list_sorted.begin()+i-removed_value);
			removed_value++;

		}
	}
	ofstream file_new;
	file_new.open(GENERATED_TRANSACTION_FILE);
				file_new<<setw(15)<<left<<"Serial No."
					<<" "<<setw(30)<<left<<"Sender"
					<<" "<<setw(30)<<left<<"Receiver"
					<<" "<<setw(10)<<left<<"Transfer Amount"<<endl;
	for(size_t i=0;i<final_transaction_list_sorted.size();i++){//Write the sorted list back to the file 
						file_new<<setw(15)<<left<<final_transaction_list_sorted.at(i).transaction_id
							<<" "<<setw(30)<<left<<final_transaction_list_sorted.at(i).transferor
							<<" "<<setw(30)<<left<<final_transaction_list_sorted.at(i).transferee
							<<" "<<setw(10)<<left<<final_transaction_list_sorted.at(i).transfer_amount<<endl;
					}
	file_new.close();
	return final_transaction_list_sorted;//Return the sorted list to send back to the client 
}

bool str_ends_with(string original_string,string substring){
	if(original_string.size() >= substring.size() && (original_string.compare(original_string.size() - substring.size(),substring.size(),substring)==0)){
		return true;
	}
	else{
		return false;
	}
}

/************************
 *Function: sort_transactions
 *Function used to sort list of transactions based on transaction id 
 *Parameters: 
 *vector<transaction> original: List of unsorted transactions. 
 *string maxvalue: The max value of the transaction id present in the list.
 *Return Value: 
 *vector <transaction> List of transactions sorted by transaction id.
 **********************/
vector <transaction> sort_transactions(vector<transaction> original,string maxvalue){
	int maxval = stringtoint_com(maxvalue);
	vector <transaction> returnvalue;
	struct transaction null_transaction;
	null_transaction.transaction_id = 0;
	returnvalue.resize(maxval-1,null_transaction); //Create list of transactions which is size of maximum transaction value and set all trnasaction ids to 0
	for(size_t i=0;i<original.size();i++){
		returnvalue.at(original.at(i).transaction_id-1)= original.at(i);//Place the transaction at the location of its transaction id 
	}
	return returnvalue;
}
/************************
 *Function: user_stat_calculation
 *Function used to calculate the statistics and return the list of statistics in which the user is involved in. Returns empty list if the user is not present in the network
 *Parameters: 
 *int udp_sockdes: Socket descriptor to connect to backend server. 
 *string server_request: The user whose statistics has to be checked.
 *Return Value: 
 *vector <stat> List of statistics involving the user.
 **********************/
vector <stat> user_stat_calculation(int udp_sockdes, string server_request){
	string username = server_request;
	server_request  ="STATS " +server_request+'\0'; //Create request message STATS <username> which is sent to the backend servers
	struct sockaddr_in server_address,address_incoming_serv_temp;
	string stats,intermediate_string;
	int user_found = 0;
	int address_size_incoming_serv = sizeof(address_incoming_serv_temp);
	char  buffer_incoming_serv[MAX_CHAR_SIZE];
	int received_bytes;
	vector <stat> returnstatlist;
	vector <string> list_of_transactions_string;
	vector <transaction>list_of_transactions_complete;
	for(int i=0;i<3;i++){//Contact the three individual servers in a loop
		server_address = map_remote_address(server_port_array[i],server_ipaddress_array[i]);//Map the remote address of the respective backend server
		stats.clear();
		string contacted_server = (i==0)?"A":((i==1)?"B":"C");
		errorchecker((sendto(udp_sockdes,server_request.c_str(),server_request.length() ,0,(struct sockaddr *)&server_address,(socklen_t)(sizeof(server_address) ))) ,"Socket_sendto"); //Send request to server
		cout<<"The main server sent a request to server "<<contacted_server<<" .\n";
		errorchecker((received_bytes = recvfrom(udp_sockdes, buffer_incoming_serv, sizeof buffer_incoming_serv, 0, (sockaddr *)&address_incoming_serv_temp, (socklen_t *)&address_size_incoming_serv)),"Socket_recvfrom");//The server sends the size of the list of transactions in bytes first
		intermediate_string = buffer_incoming_serv;
		intermediate_string.resize(received_bytes);
		size_t total_mesg_length = stringtoint_com(intermediate_string);//The message length to be received is stored in an integer
		while(stats.length()!=total_mesg_length){
			memset(buffer_incoming_serv,0,MAX_CHAR_SIZE);
			errorchecker((received_bytes = recvfrom(udp_sockdes, buffer_incoming_serv, sizeof buffer_incoming_serv, 0, (sockaddr *)&address_incoming_serv_temp, (socklen_t *)&address_size_incoming_serv)),"Socket_recvfrom");//Start receiving the transactions involving the user 
			intermediate_string = buffer_incoming_serv;
			intermediate_string.resize(received_bytes);
			stats+=intermediate_string;//Add received data to the string of statistics
			//End the loop if received data length is equivalent to the message length
		}
		cout<<"The main server received transactions from Server "<<contacted_server<<" using UDP over port "<<ntohs(address_incoming_serv_temp.sin_port)<<" .\n";
		list_of_transactions_string = listofstrings(stats,"\n");//Divide the received data into substrings based on new line character 
		memset(buffer_incoming_serv,0,MAX_CHAR_SIZE);
		for(size_t j=0;j<list_of_transactions_string.size();j++){
			list_of_transactions_complete.push_back(stringtotransaction(list_of_transactions_string.at(j)));//Add individual transactions to complete list of transactions
		}
	}
	if(list_of_transactions_complete.size()==0){//If size of received transactions is 0 return empty list of stats
		return vector<stat>();
	}	
	for(size_t i = 0;i<list_of_transactions_complete.size();i++){
		if(list_of_transactions_complete.at(i).transferor == username){//Loop through list of transactions and check if username is the transferor
			string user = list_of_transactions_complete.at(i).transferee;//Obtain the name of the other user and the transfer amount
			int amount = list_of_transactions_complete.at(i).transfer_amount;
			user_found = 0;
			for(size_t j = 0; j<returnstatlist.size();j++){//Loop through the list of stats and check if the other user is already present 
				if(user == returnstatlist.at(j).username){//If present add the amount and increment the number of transactions 
					returnstatlist.at(j).transfer_amount-=amount;
					returnstatlist.at(j).number_of_transactions++;
					user_found = 1;
				}
			}
			if(user_found ==0){//If Not found create new stat structure and add the other user along with the amount
				struct stat new_stat;
				new_stat.username = user;
				new_stat.number_of_transactions = 1;
				new_stat.transfer_amount = -1*amount;
				returnstatlist.push_back(new_stat);
			}
		}
		else{//If username is not the transferor then username is the transferee
			string user = list_of_transactions_complete.at(i).transferor;//Obtain the name of the other user and the transfer amount
			int amount = list_of_transactions_complete.at(i).transfer_amount;
			user_found = 0;
			for(size_t j = 0; j<returnstatlist.size();j++){//Loop through the list of stats and check if the other user is already present
				if(user == returnstatlist.at(j).username){//If present add the amount and increment the number of transactions 
					returnstatlist.at(j).transfer_amount+=amount;
					returnstatlist.at(j).number_of_transactions++;
					user_found = 1;
				}
			}
			if(user_found ==0){//If Not found create new stat structure and add the other user along with the amount
				struct stat new_stat;
				new_stat.username = user;
				new_stat.number_of_transactions = 1;
				new_stat.transfer_amount = amount;
				returnstatlist.push_back(new_stat);
			}
		}
	}

	for(size_t i=0;i<returnstatlist.size()-1;i++){ //Sort the final list of stats using bubble sort
		for(size_t j=0;j<returnstatlist.size()-i-1;j++){
			if(statcompare(returnstatlist.at(j),returnstatlist.at(j+1))){//Function to check whether number of transactions of current stat is higher than the next stat
				struct stat temp = returnstatlist.at(j);
				returnstatlist.at(j) = returnstatlist.at(j+1);
				returnstatlist.at(j+1) = temp;
			}
		}
	}
	return returnstatlist; //Return sorted list
}
/************************
 *Function: statcompare
 *Function used to check whether we need to sort elements or not based on the number of transactions. Returns true if first structure has less transactions than the second
 *Parameters: 
 *struct stat a: The first stat structure for comparison. 
 *struct stat b: The second stat structure for comparison.
 *Return Value: 
 *bool which is true if we need to change positions of the elements.
 **********************/
bool statcompare(struct stat a,struct stat b){
	 if(a.number_of_transactions<b.number_of_transactions){ // If first structure has less transactions immediately return true
		 return true;
	 }
	 else if(a.number_of_transactions==b.number_of_transactions){// If the number of transactions are same then return the string comparison of username in order to sort alphabetically
		 return a.username.compare(b.username)>0;
	 }
	 else{
		 return false;// If both the previous conditions are false then return false. Positions are kept the same
	 }
	}
/************************
 *Function: client_request_handler
 *Function which handles client side requests coming from client A or client B. Depending on the received string data the required operation is performed
 *Parameters: 
 *int tcp_parent_sockdes: TCP parent socket descriptor. 
 *int udp_sockdes: UDP socket descriptor to connect to the backend servers
 *bool is_clientA: Boolean to distinguish between client A and client B. True for client A and false for client B
 *Return Value: none
 * ********************/
void client_request_handler(int tcp_parent_sockdes,int udp_sockdes,bool is_clientA){
	char buffer_client_request[MAX_CHAR_SIZE];
	struct sockaddr_in address_incoming_connection_temp,address_connection_local;
	string string_incoming_data,intermediate_string;
	string client_value;
	int current_mesg_size;
	client_value = (is_clientA)?"client A":"client B"; //The request is received based on the boolean passed to the function
	vector<string> received_substrings;
	int tcp_child_socket,address_size_local = sizeof(address_connection_local),address_size_incoming = sizeof(address_incoming_connection_temp);
	errorchecker((tcp_child_socket = accept(tcp_parent_sockdes,(struct sockaddr *) &address_incoming_connection_temp,(socklen_t*)&(address_size_incoming))),"Socket_accept");//Accept is called for the parent socket. Here the parent socket could be the parent for client A connections or client B connections. Using select the correct parent socket descriptor is passed
	memset(buffer_client_request, 0,MAX_CHAR_SIZE);
	string_incoming_data.clear();
	intermediate_string.clear();
	while(!str_ends_with(string_incoming_data,"#$%")){//If the string ends with DONE then the whole file is received,exit the loop
	memset(buffer_client_request, 0,MAX_CHAR_SIZE);
	errorchecker((current_mesg_size = recv(tcp_child_socket,buffer_client_request,MAX_CHAR_SIZE,0)),"Socket_recv"); // Receive the size of data sent by the client 
	intermediate_string = buffer_client_request;
	intermediate_string.resize(current_mesg_size);
	string_incoming_data += intermediate_string;
	}
	string_incoming_data = string_incoming_data.substr(0,string_incoming_data.size()-3);
	received_substrings = listofstrings(string_incoming_data,std::string(" "));// Split string into substrings seperated by space
	errorchecker(getsockname(tcp_child_socket,(struct sockaddr*)&address_connection_local, (socklen_t *)&address_size_local),"Socket_getsockname");// Retreive the local IP and port number in order to print
	if(received_substrings.size()==0){ // If no data is received the client closed because of some issue. Close the child socket and return
		close(tcp_child_socket);
	}
	else if(received_substrings.at(0) == "TXCOINS" && received_substrings.size()==4){// If the first substring is TXCOINS then new transaction has to be added. Call the required function
		cout<<"The main server received from "<<received_substrings.at(1)<<" to transfer "<<received_substrings.at(3)<<" coins to "<<received_substrings.at(2)<<" using TCP over port "<<ntohs(address_connection_local.sin_port)<<" .\n";
		string returnvalue = new_transaction(udp_sockdes, received_substrings); // Call the new_transaction function and pass the udp socket descriptor and the received string list
		errorchecker(send(tcp_child_socket, returnvalue.c_str(), returnvalue.length(), 0),"Socket_send"); // Directly send the return value of the function
		cout<<"The main server sent the result of the transaction to "<<client_value<<".\n";
		close(tcp_child_socket);//Close the child socket
	}
	else if (received_substrings.at(0) != "TXLIST"&&received_substrings.size() == 1){//If only one substring is received and it is not TXLIST then balance is requested
		cout<<"The main server received input = "<<received_substrings.at(0) <<" from the client using TCP over port "<<ntohs(address_connection_local.sin_port)<<endl; 
		string returnvalue = balance_calculation(udp_sockdes, received_substrings.at(0),0);// Call the balance function and pass the udp socket descriptor and the received string list and 0 as parameters Here 0 indicates that there is no suppression in the print statements
		errorchecker(send(tcp_child_socket, returnvalue.c_str(), returnvalue.length(), 0),"Socket_send"); // Directly send the return value to the client of the function
		cout<<"The main server sent the current balance to "<<client_value<<".\n";
		close(tcp_child_socket);//Close the child socket
	}
	else if (received_substrings.at(0) == "TXLIST"&&received_substrings.size() == 1){//If only one substring is received and it is TXLIST then Transaction List is requested
		cout<<"The main server received input = "<<received_substrings.at(0) <<" from the "<<client_value<<" using TCP over port "<<ntohs(address_connection_local.sin_port)<<endl; 
		vector<transaction> returnvalue = txlist_op(udp_sockdes);//Call the txlist_op function which returns the sorted list of transactions 
		for (size_t i=0;i<returnvalue.size();i++){
			string sendvalue = transaction_to_string(returnvalue.at(i))+"\n\0";
			errorchecker(send(tcp_child_socket, sendvalue.c_str(), sendvalue.length(), 0),"Socket_send");// Send the returned transaction list one by one to the client 
		}
		cout<<"The sorted file is up and ready\n";

		close(tcp_child_socket);// Once all data is sent the child socket is closed. This has to be done in order to make sure the client exits recv 
	}
	else if (received_substrings.at(0) == "STATS"&&received_substrings.size() == 2){// If the first substring is STATS then stats is requested. Call the required function
		cout<<"The main server received input = "<<received_substrings.at(0) << " from the client "<<received_substrings.at(1)<<" using TCP over port "<<ntohs(address_connection_local.sin_port)<<endl; 
		vector<stat> returnvalue = user_stat_calculation(udp_sockdes,received_substrings.at(1));// Call the user_stat_calculation which returns sorted stat list
		if(returnvalue.size()==0){
			errorchecker(send(tcp_child_socket, "NO_USER", strlen("NO_USER"), 0),"Socket_send"); // If the size of returned list is 0 then no user is present. Return NO_USER to client 
		}
		for (size_t i=0;i<returnvalue.size();i++){
			string sendvalue = inttostring_com(i+1)+" "+stat_to_string(returnvalue.at(i))+"\n\0";
			errorchecker(send(tcp_child_socket, sendvalue.c_str(), sendvalue.length(), 0),"Socket_send"); // If the size of returned list is not 0 then return the list one by one to the client 
		}
		cout<<"The main server sent the STATS to "<<client_value<<".\n";
		close(tcp_child_socket);// Once all data is sent the child socket is closed. This has to be done in order to make sure the client exits recv 
	}
	else{
	}
	return;
}
