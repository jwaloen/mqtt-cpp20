#ifndef MQTTCLIENTCPP_h
#define MQTTCLIENTCPP_h



#include <memory>
#include <string>
namespace mqtt{

    class Client{
    public:
        Client(const std::string& uri, const std::string& clientId);
        ~Client();
        Client(const Client&) = delete;
        Client& operator=(const Client&) = delete;

        void connect();
        void publish(const std::string& topic, const std::string& msg);
        void disconnect();


    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
        
        


    };

}

#endif