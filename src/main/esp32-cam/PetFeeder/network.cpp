#include "secrets.h"
#include "utils.hpp"

#define _UPNP_LOGLEVEL_ 3
#include <UPnP_Generic.h>  // https://github.com/khoih-prog/UPnP_Generic
#include <WiFi.h>

constexpr u16_t BASE_ADDRESS = 5011;
constexpr int UPNP_TIMEOUT = 30000;    // ms
constexpr int LEASE_DURATION = 36000;  // seconds
constexpr int RETRY_TIMES = 4;

static const String PET_FEEDER_NAME{"PetFeeder_"};
static UPnP uPnP{UPNP_TIMEOUT};

int getDeviceIndex() {
    const auto& currentMac = WiFi.macAddress();
    if (MAC_1 == currentMac) {
        return 0;
    } else if (MAC_2 == currentMac) {
        return 1;
    } else if (MAC_3 == currentMac) {
        return 2;
    }
    return 3;
}

int getListenPort() {
    return BASE_ADDRESS + getDeviceIndex();
}

void setupNetwork() {
    Serial.print(F("Connecting to "));
    Serial.println(WIFI_SSID);
    Serial.println(WiFi.macAddress());
    auto deviceIndex = getDeviceIndex();

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    waitFor("Waiting on connection", []() { return WiFi.status() == WL_CONNECTED; });

    auto localIP = WiFi.localIP();
    Serial.print(F("IP address: "));
    Serial.println(localIP);
    auto mappingPort = BASE_ADDRESS + deviceIndex;
    Serial.print("Adding port mapping via UPnP: ");
    Serial.println(UPNP_GENERIC_VERSION);
    uPnP.addPortMappingConfig(localIP, mappingPort, RULE_PROTOCOL_TCP, LEASE_DURATION, PET_FEEDER_NAME + deviceIndex);
    bool portMappingAdded = false;
    int retries = 0;

    while (!portMappingAdded && (retries < RETRY_TIMES)) {
        Serial.println("Add Port Forwarding, Try # " + String(++retries));
        int result = uPnP.commitPortMappings();
        portMappingAdded = ((result == PORT_MAP_SUCCESS) || (result == ALREADY_MAPPED));
        Serial.println("commitPortMappings result =" + String(result));
        if (!portMappingAdded) {
            // for debugging, you can see this in your router too under forwarding or UPnP uPnP->printAllPortMappings();
            Serial.println(F("This was printed because adding the required port mapping failed "));
            if (retries < RETRY_TIMES)
                delay(10000);  // 10 seconds before trying again
        }
    }

    uPnP.printAllPortMappings();
    Serial.println(F("\nUPnP done"));
}

void loopNetwork() {
    uPnP.updatePortMappings(600000);  // 10 minutes
}
