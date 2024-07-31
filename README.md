# Socket-Send-Data-Multi-Threaded-Producer-Consumer
This project is based on the producer-consumer model. Throughout the project, I employed C++ to implement multi-execution continuation programs and utilized sockets for data exchange. I undertook this exercise to deepen my understanding of these concepts and their practical application.

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
