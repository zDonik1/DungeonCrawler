#include "client.h"

Client::Client()
    : m_listenThread(&Client::listen, this)
{
    m_connected = false;
}

Client::~Client()
{
    m_socket.unbind();
}

bool Client::connect()
{
    if (m_connected) {
        return false;
    }

    m_socket.bind(sf::Socket::AnyPort);
    std::cout << "Bound client to port: " << m_socket.getLocalPort() << std::endl;
    sf::Packet p;
    stampPacket(PacketType::Connect, p);
    p << m_playerName;
    if (m_socket.send(p, m_serverIp, m_serverPort) != sf::Socket::Done) {
        m_socket.unbind();
        return false;
    }

    std::cout << "Connecting..." << std::endl;
    m_socket.setBlocking(false);
    p.clear();
    sf::IpAddress recvIP;
    PortNumber recvPORT;
    sf::Clock timer;
    timer.restart();
    while (timer.getElapsedTime().asMilliseconds() < CONNECT_TIMEOUT) {
        sf::Socket::Status s = m_socket.receive(p, recvIP, recvPORT);
        if (s != sf::Socket::Done) {
            continue;
        }

        if (recvIP != m_serverIp) {
            continue;
        }

        PacketID id;
        if (!(p >> id)) {
            break;
        }

        if (static_cast<PacketType>(id) != PacketType::Connect) {
            continue;
        }

        m_packetHandler(id, p, this);
        std::cout << "Connected to server!" << std::endl;
        m_connected = true;
        m_socket.setBlocking(true);
        m_lastHeartBeat = m_serverTime;
        m_listenThread.launch();
        return true;
    }

    std::cout << "Connection attempt failed! Server info: " << m_serverIp << ":" << m_serverPort << std::endl;
    m_socket.unbind();
    m_socket.setBlocking(true);
    return false;
}

bool Client::disconnect()
{
    if (!m_connected) {
        return false;
    }

    sf::Packet p;
    stampPacket(PacketType::Disconnect, p);
    sf::Socket::Status s = m_socket.send(p, m_serverIp, m_serverPort);
    m_connected = false;
    m_socket.unbind();
    if (s != sf::Socket::Done) {
        return false;
    }

    return true;
}

void Client::listen()
{
    sf::Packet packet;
    sf::IpAddress recvIP;
    PortNumber recvPORT;
    std::cout << "Beginning to listen..." << std::endl;
    while (m_connected) {
        packet.clear();
        sf::Socket::Status status = m_socket.receive(packet, recvIP, recvPORT);
        if (status != sf::Socket::Done) {
            if (m_connected) {
                std::cout << "Failed receiving a packet from " << recvIP << ":" << recvPORT << ". Status: " << status << std::endl;
                continue;
            }
            else {
                std::cout << "Socket unbound." << std::endl;
                break;
            }
        }

        if (recvIP != m_serverIp) {
            std::cout << "Invalid packet received: invalid origin." << std::endl;
            continue;
        }

        PacketID p_id;
        if (!(packet >> p_id)) {
            std::cout << "Invalid packet received: unable to extract id." << std::endl;
            continue;
        }

        PacketType id = static_cast<PacketType>(p_id);
        if (id < PacketType::Disconnect || id >= PacketType::OutOfBounds) {
            std::cout << "Invalid packet received: id is out of bounds." << std::endl;
            continue;
        }

        if (id == PacketType::Heartbeat) {
            sf::Packet p;
            stampPacket(PacketType::Heartbeat, p);
            if (m_socket.send(p, m_serverIp, m_serverPort) != sf::Socket::Done) {
                std::cout << "Failed sending a heartbeat!" << std::endl;
            }
            sf::Int32 timestamp;
            packet >> timestamp;
            setTime(sf::milliseconds(timestamp));
            m_lastHeartBeat = m_serverTime;
        }
        else if (m_packetHandler) {
            m_packetHandler(static_cast<PacketID>(id), packet, this);
        }
    }
}

bool Client::send(sf::Packet &l_packet)
{
    if (!m_connected) {
        return false;
    }

    if (m_socket.send(l_packet, m_serverIp, m_serverPort) != sf::Socket::Done) {
        return false;
    }

    return true;
}

const sf::Time &Client::getTime() const
{
    return m_serverTime;
}

const sf::Time &Client::getLastHeartbeat() const
{
    return m_lastHeartBeat;
}

void Client::setTime(const sf::Time &l_time)
{
    m_serverTime = l_time;
}

void Client::setServerInformation(const sf::IpAddress &l_ip, const PortNumber &l_port)
{
    m_serverIp = l_ip;
    m_serverPort = l_port;
}

void Client::setup(void (*l_handler)(const PacketID &, sf::Packet &, Client *))
{
    m_packetHandler = std::bind(l_handler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

void Client::unregisterPacketHandler()
{
    m_packetHandler = nullptr;
}

void Client::update(const sf::Time &l_time)
{
    if (!m_connected) {
        return;
    }

    m_serverTime += l_time;
    if (m_serverTime.asMilliseconds() < 0) {
        m_serverTime -= sf::milliseconds(sf::Int32(Network::HighestTimestamp));
        m_lastHeartBeat = m_serverTime;
        return;
    }

    if (m_serverTime.asMilliseconds() - m_lastHeartBeat.asMilliseconds() >= sf::Int32(Network::ClientTimeout)) {
        std::cout << "Server connection timed out!" << std::endl;
        disconnect();
    }
}

bool Client::isConnected() const
{
    return m_connected;
}

void Client::setPlayerName(const std::string &l_name)
{
    m_playerName = l_name;
}

sf::Mutex &Client::getMutex()
{
    return m_mutex;
}

