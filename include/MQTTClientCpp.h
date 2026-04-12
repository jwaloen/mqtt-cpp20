#ifndef MQTTCLIENTCPP_h
#define MQTTCLIENTCPP_h



#include <memory>
#include <functional>
#include <string>


namespace mqtt{
    struct ClientCallbacks{
        std::function<void(void)> connLost = nullptr;
        std::function<void(std::string, std::string)> msgHandler = nullptr;
        std::function<void(int)> deliveryComplete = nullptr;
    };

    class Client{
    public:
        Client(std::string_view uri, std::string_view clientId, ClientCallbacks callbacks = {});
        ~Client();
        Client(const Client&) = delete;
        Client& operator=(const Client&) = delete;

        void connect();
        void publish(const std::string& topic, const std::string& msg);
        void subscribe(const std::string& topic);
        void disconnect();


    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

}

#endif