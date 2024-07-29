# Socket-Send-Data-Multi-Threaded-Producer-Consumer
I learned how to create multi-threaded programs, and learned how to use sockets to transmit and receive data.

# Description
## Client:
Collect all file paths modified after timestamp in the test folder, and send these file paths to the server.
You need to enter the timestamp, server IP, and port.
## Server:
Receive the file paths sent by the client, find these files through the consumer-producer model, calculate the frequency of occurrence of English words in the files, and output the results.
You need to enter the number of threads and ports.

# How to use
Compile:

`$ make`

Client command:

`$ ./client [Timestamp] [IP] [Port]`

![image](https://github.com/oh6400/Socket-Send-Data-Multi-Threaded-Producer-Consumer/blob/main/img/client.png)

Server command:

`$ ./server [Number of Threads] [Port]`

![image](https://github.com/oh6400/Socket-Send-Data-Multi-Threaded-Producer-Consumer/blob/main/img/server.png)
