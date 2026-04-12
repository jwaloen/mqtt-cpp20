#include "MQTTClientCpp.h"
#include "MQTTClient.h"
#include <codecvt>
#include <functional>
#include <iostream>
#include <memory>
#include <functional>
#include <stdexcept>
#include <cstring>



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
                Impl(std::string_view uri, std::string_view clientId) : m_uri(uri), m_clientId(clientId) {}
                std::unique_ptr<void, MqttClientDestructor> m_client = nullptr;
                std::string m_uri;
                std::string m_clientId;
                bool m_connected = false;

                std::function<void(std::string, std::string)> m_msgHandler;
                std::function<void(void)> m_connLost;
                std::function<void(int)> m_deliveryComplete;

                static int m_msgarrvd_trampoline(void* context, char* topicName, int topicLen, MQTTClient_message* message){
                        auto* l_impl = static_cast<Impl*>(context);

                        size_t realTopicLen = (topicLen > 0) ? static_cast<size_t>(topicLen) : std::strlen(topicName);

                        std::string topic(topicName, realTopicLen);
                        std::string payload(static_cast<char*>(message->payload), message->payloadlen);

                        if(l_impl->m_msgHandler){
                                l_impl->m_msgHandler(std::move(topic), std::move(payload));
                        }

                        MQTTClient_freeMessage(&message);
                        MQTTClient_free(topicName);
                        return 1;
                }

                static void m_deliveryComplete_trampoline(void *context, MQTTClient_deliveryToken dt){
                        auto* l_impl = static_cast<Impl*>(context);

                        if(l_impl->m_deliveryComplete){
                                l_impl->m_deliveryComplete(dt);
                        }
                }

                static void m_connLost_trampoline(void *context, char *cause){
                        auto* l_impl = static_cast<Impl*>(context);

                        // From the MTTQClient.h documentation: Currently, <i>cause</i> is always set to NULL.
                        // Suppress warning unused parameter:
                        (void)cause;

                        if(l_impl->m_connLost){
                                l_impl->m_connLost();
                        }
                }
        };

        Client::Client(std::string_view uri, std::string_view clientId, ClientCallbacks callbacks) : pImpl(std::make_unique<Impl>(uri, clientId)){
                int rc = 0;
                MQTTClient rp = nullptr;

                if ((rc = MQTTClient_create(&rp, pImpl->m_uri.c_str(), pImpl->m_clientId.c_str(), MQTTCLIENT_PERSISTENCE_NONE, nullptr)) != MQTTCLIENT_SUCCESS){
                        throw std::runtime_error("Failed to create MQTT Client with C library.\n");
                }
                pImpl->m_client.reset(rp);
                if (pImpl->m_client == nullptr) {
                        throw std::runtime_error("Failed to store MQTT Client.\n");
                }

                pImpl->m_connLost = callbacks.connLost;
                pImpl->m_msgHandler = callbacks.msgHandler;
                pImpl->m_deliveryComplete = callbacks.deliveryComplete;


                if ((rc = MQTTClient_setCallbacks(rp, pImpl.get(), pImpl->m_connLost_trampoline, pImpl->m_msgarrvd_trampoline, pImpl->m_deliveryComplete_trampoline)) != MQTTCLIENT_SUCCESS){
                        throw std::runtime_error("Failed to set callbacks.");
                }
        }

        Client::~Client() = default;

        void Client::connect(){
                int rc = 0;
                MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

                conn_opts.keepAliveInterval = 20;
                conn_opts.cleansession = 1;

                if (pImpl->m_client){
                        MQTTClient rp = pImpl->m_client.get();
                        if ((rc = MQTTClient_connect(rp, &conn_opts)) != MQTTCLIENT_SUCCESS)
                        {
                                throw std::runtime_error("Failed to connect to MQTT Client with C library.");
                        }
                }
                else {
                        throw std::runtime_error("The saved client is lost.\n");
                }
                pImpl->m_connected = true;
        }

        void Client::publish(const std::string& topic, const std::string& msg){
                int rc = 0;
                MQTTClient_message pubmsg = MQTTClient_message_initializer;
                MQTTClient_deliveryToken token;

                pubmsg.payload = const_cast<char*>(msg.c_str());
                pubmsg.payloadlen = msg.length();
                pubmsg.qos = 1;
                pubmsg.retained = 0;

                if (pImpl->m_connected){
                        MQTTClient rp = pImpl->m_client.get();
                        if ((rc = MQTTClient_publishMessage(rp, topic.c_str(), &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
                        {
                                throw std::runtime_error("Failed to publish message, return code %d\n");
                        }

                        rc = MQTTClient_waitForCompletion(rp, token, 10000L);
                }
                else {
                        throw std::runtime_error("Cannot publish before we are connected to a client\n");
                }
        }       

        void Client::subscribe(const std::string& topic){
                int rc = 0;
                MQTTClient rp = pImpl->m_client.get();

                if (!pImpl->m_connected){
                        throw std::logic_error("Cannot subscribe before we are connected to a client.");
                } 

                if(!pImpl->m_msgHandler){
                        throw std::logic_error("Cannot subscribe: Client was initialized in publish-only mode (no callback provided).");
                }

                if ((rc = MQTTClient_subscribe(rp, topic.c_str(), 1)) != MQTTCLIENT_SUCCESS){
                        throw std::runtime_error("Failed to subscribe, return code.");
                }

        }

        void Client::disconnect(){
                int rc = 0;
                if (pImpl->m_connected){
                        MQTTClient rp = pImpl->m_client.get();

                        if ((rc = MQTTClient_disconnect(rp, 10000)) != MQTTCLIENT_SUCCESS){
                                throw std::runtime_error("Failed to disconnect, return code %d\n");
                        }
                }
                else {
                        throw std::runtime_error("Cannot disconnect from client because it is not connected.\n");
                }
                pImpl->m_connected = false;
        }

}
