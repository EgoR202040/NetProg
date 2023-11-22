#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <memory>//unique_ptr
#include <string>
#include <time.h>
#include <chrono>
#include <unistd.h>// close
#define buff_size 1024
std::string getCurrentDateTime( std::string s ){
    time_t now = time(0);
    struct tm  tstruct;
    char  buf[80];
    tstruct = *localtime(&now);
    if(s=="now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if(s=="date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return std::string(buf);
};
std::unique_ptr<char[]> buffer(new char[buff_size]);
class Net_exp
{
    std::string message;
public:
    Net_exp(std::string s):message(s) {}
    std::string what()
    {
        return message;
    }
};

int main()
{
    try {
        int sock = socket(AF_INET,SOCK_DGRAM,0); 
        if(sock==-1){
            throw Net_exp("Ошибка создание сокета");
            }
        struct sockaddr_in serverAddr {
        };
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(13);
        inet_aton("172.16.40.1",&serverAddr.sin_addr);
        int s = sendto(sock,nullptr,0,0,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
        if(s<0){
        	throw Net_exp("Ошибка отправки данных серверу для теста");
        }
        int rc = recvfrom(sock,buffer.get(),buff_size-1,0,nullptr,nullptr);
        if(rc==-1){
            throw Net_exp("Ошибка приёма данных от сервера");
            }
        buffer[rc]=0;
        std::string recv_message(buffer.get(),rc);
        std::cout << "Current time: " << recv_message << std::endl;
        close(sock);
    } catch(Net_exp& e) {
        std::cerr<< e.what() << std::endl;
    }
    return 0;
}
