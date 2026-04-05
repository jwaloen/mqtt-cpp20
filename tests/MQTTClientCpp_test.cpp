#include "gmock/gmock.h"

#include <ostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include "MQTTClientCpp.h"


TEST(mqttclient, TestCreatingMqttClient){
    EXPECT_NO_THROW(mqtt::Client testClient2("tcp://test.mosquitto.org:1883", "ExampleClientPub"));
}

TEST(mqttclient, TestFailedCreatingMqttClient){
    EXPECT_THROW(mqtt::Client testClient2("http://test.mosquitto.org:1883", "ExampleClientPub"), std::runtime_error);
} 

TEST(mqttclient, TestConnectionToMosquittoOrg){
    mqtt::Client testClient{"tcp://test.mosquitto.org:1883", "ExampleClientPub"};
    EXPECT_NO_THROW(testClient.connect());
}

TEST(mqttclient, TestFailedConnectionToMosquittoOrg){
    mqtt::Client testClient{"tcp://test.mosquitto.org:1800", "ExampleClientPub"};
    EXPECT_THROW(testClient.connect(), std::runtime_error);
}

TEST(mqttclient, TestPublishToMosquittoOrg){
    mqtt::Client testClient{"tcp://test.mosquitto.org:1883", "ExampleClientPub"};
    testClient.connect();
    EXPECT_NO_THROW(testClient.publish("MQTT Examples", "Hello World!"));
}

TEST(mqttclient, TestFailingPublishBeforeConnectToMosquittoOrg){
    mqtt::Client testClient{"tcp://test.mosquitto.org:1883", "ExampleClientPub"};
    EXPECT_THROW(testClient.publish("MQTT Examples", "Hello World!"), std::runtime_error);
}

TEST(mqttclient, TestDestructorEarly){
    mqtt::Client *testClient = new mqtt::Client("tcp://test.mosquitto.org:1883", "ExampleClientPub");
    testClient->connect();
    EXPECT_NO_THROW(delete testClient);
}

TEST(mqttclient, ForceFailedTestToTestGithubActionsDeleteAfter){
    EXPECT_NO_THROW(mqtt::Client testClient2("http://test.mosquitto.org:1883", "ExampleClientPub"), std::runtime_error);
} 
