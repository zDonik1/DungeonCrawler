#pragma once

#include <SFML/Network.hpp>
#include <iostream>
#include <functional>
#include "networkdefinitions.h"
#include "packettypes.h"
#include "netsettings.h"

#define CONNECT_TIMEOUT 5000

class Client;

using PacketHandler = std::function<void(const PacketID&, sf::Packet&, Client*)>;

class Client
{
public:
    Client();
    ~Client();

    bool connect();
    bool disconnect();

    void listen();
    bool send(sf::Packet &l_packet);

    const sf::Time &getTime() const;
    const sf::Time &getLastHeartbeat() const;
    void setTime(const sf::Time &l_time);
    void setServerInformation(const sf::IpAddress &l_ip, const PortNumber &l_port);

    template<class T>
    void setup(void(T::*l_handler)(const PacketID&, sf::Packet&, Client*), T *l_instance)
    {
        m_packetHandler = std::bind(l_handler, l_instance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    void setup(void(*l_handler)(const PacketID&, sf::Packet&, Client*));
    void unregisterPacketHandler();

    void update(const sf::Time &l_time);

    bool isConnected() const;
    void setPlayerName(const std::string &l_name);

    sf::Mutex &getMutex();

private:
    std::string m_playerName;

    sf::UdpSocket m_socket;
    sf::IpAddress m_serverIp;
    PortNumber m_serverPort;
    PacketHandler m_packetHandler;
    bool m_connected;
    sf::Time m_serverTime;
    sf::Time m_lastHeartBeat;

    sf::Thread m_listenThread;
    sf::Mutex m_mutex;
};
