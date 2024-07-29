#include <iostream>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <atomic>
#include <queue>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <condition_variable>
#include <fstream>
#include <thread>

using namespace std;

mutex mutexFilePaths;
mutex mutexWordFreq;
std::atomic<bool> isReceiveFinish{false};
std::condition_variable cv;

void SplitReceivedString(string recvBuf, queue<string> &filePaths);

void getWordsFrequencyInFile(queue<string> &filePaths, unordered_map<string, int> &wordFreq, int i);

int main(int argc, char *argv[]){
    if(argc != 3){
        cout << "Incorrect number of arguments.\n";
        cout << "Parameters: [number of threads] [Port] ex: 8 5005\n";
        return -1;
    }
    int numberOfThread = atoi(argv[1]);
    char* inputPort = argv[2];
    
    char recvBuf[1024] = {0}; // message buffer

    queue<string> filePaths; // tasks
    unordered_map<string, int> wordFreq; // word frequency
    string recvString; // receive message
    vector<std::thread> threads;

    // Create server socket 
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenFd == -1){
        cerr << "Fail to create server socket\n";
        return -1;
    }

    // Bind IP to socket
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(atoi(inputPort));
    if (bind(listenFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Bind failed\n";
        close(listenFd);
        return -1;
    }

    // Set the socket to listenable
    // backlog = 3: Accepts at most 3 connection requests.
    if (listen(listenFd, 3) < 0) {
        cerr << "Listen failed\n";
        close(listenFd);
        return -1;
    }

    // Handle client connection requests
    int clientFd = accept(listenFd, 0, 0);
    if(clientFd == -1){
        cerr << "client connection requests failed\n";
        close(listenFd);
        return -1;
    }
    
    // Generate mutiple threads wating for new tasks.
    for(int i = 0; i < numberOfThread; i++){
        threads.push_back(std::thread(
            getWordsFrequencyInFile,
            std::ref(filePaths),
            std::ref(wordFreq),
            i
        ));
    }

    // Communicate with client
    // Receive messages and notify thread to perform tasks.
    while(1){
        int iret = recv(clientFd, recvBuf, sizeof(recvBuf), 0);
        if(iret <= 0){
            // cout << "receive message from client failed.\n";
            break;
        }
        recvString = recvBuf;
        SplitReceivedString(recvString, filePaths);
        memset(recvBuf, 0, sizeof(recvBuf));
        string reply = "OK";
        iret = send(clientFd, reply.c_str(), reply.size(), 0);
        if(iret <= 0){
            cout << "send message to client failed.\n";
            break;
        }
    }
    // Notification that there are no more new tasks
    isReceiveFinish = 1;
    cv.notify_all();

    // Close socket
    close(listenFd);
    close(clientFd);
    for(auto &t: threads) t.join();

    // Print word frequency
    for(auto w: wordFreq){
        cout << w.first << " " << w.second << endl;
    }

    printf("\n%s success.\n", __FILE__);

    return 0;
}


void getWordsFrequencyInFile(queue<string> &filePaths, unordered_map<string, int> &wordFreq, int i){
    while(1){
        unique_lock<mutex> ugMutexFilePaths(mutexFilePaths);
        // waiting for new task or finish signal.
        cv.wait(ugMutexFilePaths, 
            [&](){
                return !filePaths.empty() || isReceiveFinish;
            }
        );
        // When there are no tasks and no new tasks, it end.
        if(isReceiveFinish && filePaths.empty()) return;

        std::ifstream file(filePaths.front());
        filePaths.pop();
        ugMutexFilePaths.unlock();

        string word;
        while(file >> word){
            if(ispunct(word[word.length()-1]))
                word = word.substr(0, word.length()-1);
            lock_guard<mutex> lg(mutexWordFreq);
            wordFreq[word]++;
        }
    }
}

// Split the received string with '\n' and notify one of the idle threads.
void SplitReceivedString(string recvBuf, queue<string> &filePaths){
    std::stringstream ss(recvBuf);
    string filePath;
    while(std::getline(ss, filePath, '\n')){
        if(filePath.length() > 0){
            lock_guard<mutex> lg(mutexFilePaths);        
            filePaths.push(filePath);
            cv.notify_one();
        }
    }
}
