#include "MQTTClientCpp.h"
#include "MQTTClient.h"
#include <codecvt>
#include <iostream>
#include <memory>
#include <stdexcept>



namespace mqtt{
        struct MqttClientDestructor{
                void operator()(void* p) const {
                        if(p) {
                                MQTTClient_disconnect(p, 10000);
                                MQTTClient_destroy(&p);
                        }
                }
        };

        struct Client::Impl{
                Impl(const std::string& uri, const std::string& clientId) : m_uri(uri), m_clientId(clientId) {}
                std::unique_ptr<void, MqttClientDestructor> client = nullptr;
                MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
                MQTTClient_message pubmsg = MQTTClient_message_initializer;
                MQTTClient_deliveryToken token;
                std::string m_uri;
                std::string m_clientId;
        };

        Client::Client(const std::string& uri, const std::string& clientId) : pImpl(std::make_unique<Impl>(uri, clientId)){
                int rc = 0;
                MQTTClient rp = nullptr;
                if ((rc = MQTTClient_create(&rp, pImpl->m_uri.c_str(), pImpl->m_clientId.c_str(), MQTTCLIENT_PERSISTENCE_NONE, nullptr)) != MQTTCLIENT_SUCCESS){
                        throw std::runtime_error("Failed to create MQTT Client with C library.\n");
                }
                pImpl->client.reset(rp);
                if (pImpl->client == nullptr) {
                        throw std::runtime_error("Failed to store MQTT Client.\n");
                }
        }

        Client::~Client() = default;

        void Client::connect(){
                int rc = 0;
                pImpl->conn_opts.keepAliveInterval = 20;
                pImpl->conn_opts.cleansession = 1;

                if (pImpl->client){
                        MQTTClient rp = pImpl->client.get();
                        if ((rc = MQTTClient_connect(rp, &pImpl->conn_opts)) != MQTTCLIENT_SUCCESS)
                        {
                                throw std::runtime_error("Failed to connect to MQTT Client with C library.\n");
                        }
                }
                else {
                        throw std::runtime_error("The saved client is lost.\n");
                }
        }

        void Client::publish(const std::string& topic, const std::string& msg){
                int rc = 0;

                pImpl->pubmsg.payload = const_cast<char*>(msg.c_str());
                pImpl->pubmsg.payloadlen = msg.length();
                pImpl->pubmsg.qos = 1;
                pImpl->pubmsg.retained = 0;

                if (pImpl->client){
                        MQTTClient rp = pImpl->client.get();
                        if ((rc = MQTTClient_publishMessage(rp, topic.c_str(), &pImpl->pubmsg, &pImpl->token)) != MQTTCLIENT_SUCCESS)
                        {
                                throw std::runtime_error("Failed to publish message, return code %d\n");
                        }

                        rc = MQTTClient_waitForCompletion(rp, pImpl->token, 10000L);
                }
                else {
                        throw std::runtime_error("The saved client is lost.\n");
                }
        }

        void Client::disconnect(){
                int rc = 0;
                if (pImpl->client){
                        MQTTClient rp = pImpl->client.get();

                        if ((rc = MQTTClient_disconnect(rp, 10000)) != MQTTCLIENT_SUCCESS){
                                throw std::runtime_error("Failed to disconnect, return code %d\n");
                        }
                }
                else {
                        throw std::runtime_error("The saved client is lost.\n");
                }
        }

}
