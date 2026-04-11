#ifndef MQTTCLIENTCPP_h
#define MQTTCLIENTCPP_h



#include <memory>
#include <functional>
#include <string>


namespace mqtt{

    class Client{
    public:
        Client(std::string_view uri, std::string_view clientId);
        ~Client();
        Client(const Client&) = delete;
        Client& operator=(const Client&) = delete;

        void connect();
        void publish(const std::string& topic, const std::string& msg);
        void setCallbacks(std::function<void(std::string_view, std::string_view)> msgarrvd);
        void subscribe(const std::string& topic);
        void disconnect();


    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

}

#endif