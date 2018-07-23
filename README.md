# Bellman-Ford-Router-Implementation
A C++ script written to simulate routers communicating between each other on a network and updating their forwarding tables using a distance-vector algorithm (Bellman Ford).

This was an assigment from my senior year at NJIT. It received a 6.0 out of 7.0.

Programming Language: C++
Version: C++ 11
Command lines: 
	Option 1 (What I used)
	1. Compile using g++ -std=c++0x -o run main.cpp
	2. Run using ./run router_num, where router_num is the number of the router
	   you want to simulate (i.e. ./run 1)
	
	Option 2
	1. Compile using g++ -std=c++0x main.cpp
	2. Run using ./a.out router_num, where router_num is the number of the router
	   you want to simulate (i.e. ./a.out 1)

References:
1. “Getting error code when creating a socket in Linux” - https://stackoverflow.com/questions/27096985/getting-error-code-when-creating-a-socket-in-linux
	a. Unseen in program, used when trying to test for errors
2. IEEE and The Open Group “setsockopt.html” - http://pubs.opengroup.org/onlinepubs/009695399/functions/setsockopt.html
3. Cplusplus.com “sleep_for” - http://www.cplusplus.com/reference/thread/this_thread/sleep_for/
4. Cplusplus.com “<chrono>” - http://www.cplusplus.com/reference/chrono/
5. “C++ compile issue” - https://stackoverflow.com/questions/16354058/c-compile-issue
	a. Unseen in program, used when trying to test for errors
6. Linux Manual “connect()” - https://linux.die.net/man/3/connect
	a. Unseen in program, used when troubleshooting connection to other routers
7. Linux Manual “recvfrom()” - https://linux.die.net/man/2/recvfrom
8. My own work from the first programming assignment (which makes up pretty much all of
   the work I got done, the rest was trivial things such as what a function's parameters
   mean or how to output errors for debugging)
