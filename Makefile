all : serverM serverA serverB serverC clientA clientB 
serverM: serverM.cpp serverM.h connection_helper.h
	g++ serverM.cpp -o serverM
serverA: serverA.cpp serverA.h connection_helper.h
	g++ serverA.cpp -o serverA
serverB: serverB.cpp serverB.h connection_helper.h
	g++ serverB.cpp -o serverB
serverC: serverC.cpp serverC.h connection_helper.h
	g++ serverC.cpp -o serverC
clientA: clientA.cpp clientA.h connection_helper.h
	g++ clientA.cpp -o clientA
clientB: clientB.cpp clientB.h connection_helper.h
	g++ clientB.cpp -o clientB
clean:
	rm serverM
	rm serverA
	rm serverB
	rm serverC
	rm clientA
	rm clientB
