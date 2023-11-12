#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <memory>//unique_ptr
#include <string>
#include <unistd.h>// close
#define buff_size 1024
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
        int s;
        socklen_t clientLength;
        struct sockaddr_in serverAddr {
        }, clientAddr{};
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s ==-1) {
            throw std::invalid_argument("Критическая ошибка: ошибка создание сокета сервера");
        }
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(7777);
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        if (bind(s, (sockaddr *)(&serverAddr), sizeof(serverAddr)) ==-1) {
            throw std::invalid_argument("Критическая ошибка: ошибка привязки сокета сервера");
        }
        clientLength = sizeof(clientAddr);
        while (true) {
            try {
                int rc = recvfrom(s, buffer.get(), 1024, 0, (sockaddr*)(&clientAddr),&clientLength);
                if (rc ==-1) {
                    throw Net_exp("Ошибка принятие сообщения");
                }
                buffer[rc]=0;
                std::string message_client(buffer.get(),rc);
                std::cout << "Получено сообщение от клиента: " << message_client<<std::endl;
            } catch(Net_exp& err) {
                std::cerr <<err.what()<<std::endl;
            }
        }
    }catch(std::invalid_argument& e){
        std::cerr <<e.what() << std::endl;
        }
    return 0;
}
