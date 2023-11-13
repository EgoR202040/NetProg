#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <memory>//unique_ptr
#include <string>
#include <unistd.h>// close
#define buff_size 1024
#define port 7777
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
        int queue_len = 100;
        sockaddr_in * addr = new (sockaddr_in);
        addr->sin_family = AF_INET;
        addr->sin_port = htons(port);
        inet_aton("127.0.0.1", &addr->sin_addr);
        int s = socket(AF_INET, SOCK_STREAM, 0); // TCP
        if (s ==-1) {
            throw std::invalid_argument("Критическая ошибка: ошибка создание сокета сервера");
        }
        int rc =bind(s,(const sockaddr*)addr,sizeof(sockaddr_in));
        if(rc==-1) {
            throw std::invalid_argument("Критическая ошибка: ошибка привязки сокета сервера");
        }
        rc = listen(s,queue_len);
        if (rc ==-1) {
            throw std::invalid_argument("Критическая ошибка: ошибка listen");
        }
        while (true) {
            try {
                sockaddr_in * client_addr = new sockaddr_in;
                socklen_t len = sizeof (sockaddr_in);
                int work_sock = accept(s, (sockaddr*)(client_addr), &len);
                if(work_sock==-1){
                    throw Net_exp("Ошибка создания сокета клиента");
                    }
                rc = recv(work_sock, buffer.get(), 1024, 0);
                if (rc ==-1) {
                    throw Net_exp("Ошибка принятие сообщения");
                }
                buffer[rc]=0;
                std::string message_client(buffer.get(),rc);
                std::cout << "Получено сообщение от клиента: " << message_client;
                rc = send(work_sock,buffer.get(),message_client.length(),0);
                if(rc==-1){
                	throw Net_exp("Ошибка отправки сообщения");
                }
                close(work_sock);
            } catch(Net_exp& err) {
                std::cerr << "Ошибка: "<<err.what()<<std::endl;
            }
        }
    } catch(std::invalid_argument& e) {
        std::cerr <<e.what() << std::endl;
    }
    return 0;
}
