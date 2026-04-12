#include <gtest/gtest.h>
#include <string>
#include <chrono>
#include <iostream>
#include "MQTTClientCpp.h"

// Guarantees the broker never drops our test connections:
std::string getUniqueId(const std::string& base) {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return base + "_" + std::to_string(ms);
}

// ==============================================================================
// CONNECTION TESTS
// ==============================================================================

TEST(MqttClientTest, TestCreatingMqttClient){
    EXPECT_NO_THROW(mqtt::Client client("tcp://test.mosquitto.org:1883", getUniqueId("Test1")));
}

TEST(MqttClientTest, TestFailedCreatingMqttClient){
    // Bad protocol scheme (http)
    EXPECT_THROW(mqtt::Client client("http://test.mosquitto.org:1883", getUniqueId("Test2")), std::runtime_error);
} 

TEST(MqttClientTest, TestConnectionToMosquittoOrg){
    mqtt::Client client("tcp://test.mosquitto.org:1883", getUniqueId("Test3"));
    EXPECT_NO_THROW(client.connect());
}

TEST(MqttClientTest, TestDestructorEarly){
    EXPECT_NO_THROW({
        mqtt::Client client("tcp://test.mosquitto.org:1883", getUniqueId("TestDestruct"));
        client.connect();
    });
}

// ==============================================================================
// PUBLISH TESTS
// ==============================================================================

TEST(MqttClientTest, TestPublishToMosquittoOrg){
    mqtt::Client client("tcp://test.mosquitto.org:1883", getUniqueId("TestPub1"));
    client.connect();
    EXPECT_NO_THROW(client.publish("TechTopic/Tests", "Hello World!"));
}

TEST(MqttClientTest, TestFailingPublishBeforeConnect){
    mqtt::Client client("tcp://test.mosquitto.org:1883", getUniqueId("TestPub2"));
    
    EXPECT_THROW(client.publish("TechTopic/Tests", "Hello World!"), std::logic_error);
}

TEST(MqttClientTest, TestConnectPublishDisconnectReconnect){
    mqtt::Client client("tcp://test.mosquitto.org:1883", getUniqueId("TestLifecycle"));
    
    client.connect();
    client.publish("TechTopic/Tests", "Message 1");
    client.disconnect();
    
    EXPECT_THROW(client.publish("TechTopic/Tests", "Should Fail"), std::logic_error);
    
    client.connect();
    EXPECT_NO_THROW(client.publish("TechTopic/Tests", "Message 2"));
}

// ==============================================================================
// SUBSCRIBE & CALLBACK TESTS
// ==============================================================================

TEST(MqttClientTest, TestSubscribeWithLambda){
    mqtt::ClientCallbacks callbacks;
    callbacks.msgHandler = [](std::string topic, std::string msg) {
        std::cout << "Received on " << topic << ": " << msg << std::endl;
    };

    mqtt::Client client("tcp://test.mosquitto.org:1883", getUniqueId("TestSub1"), std::move(callbacks));
    client.connect();
    
    EXPECT_NO_THROW(client.subscribe("TechTopic/Tests"));
}

TEST(MqttClientTest, TestFailingSubscribeBeforeConnect){
    mqtt::Client client("tcp://test.mosquitto.org:1883", getUniqueId("TestSub2"));
    EXPECT_THROW(client.subscribe("TechTopic/Tests"), std::logic_error);
}