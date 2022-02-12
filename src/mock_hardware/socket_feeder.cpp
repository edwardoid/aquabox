#include <iostream>

#include <arpa/inet.h>
#include <easy_slave.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unix_socket_proto_io.h>

#include <thread>

using namespace aquabox;
using namespace aquabox::proto;

void error(const char* msg)
{
    perror(msg);
    exit(1);
}

void calibrate(bool)
{
    fprintf(stderr, "#                                                 #");
    fprintf(stderr, "#                RUNNING CALLBACK                 #");
    fprintf(stderr, "#                                                 #");
}

CachedSlaveProperty<int32_t> m_times("times", 1);
CachedSlaveProperty<uint32_t> m_timesTimeout("_times", 100);
CachedSlaveProperty<char*> m_class("class", "step");
CachedSlaveProperty<char*> m_type("type", "feeder");
Method m_calibrate("+calibrate", &calibrate);
CachedSlaveProperty<bool> m_cache("_cache", true);
CachedSlaveProperty<int32_t> m_pingTimeout("_timeout", 5000);
SlaveProperty* allProperties[] = {&m_times, &m_timesTimeout, &m_class, &m_type, &m_calibrate, &m_cache, &m_pingTimeout};

aquabox::proto::byte_t* FEEDER_SERIAL = (aquabox::proto::byte_t*) ("tstrelay");

class MockFeeder : public EasySlave<7>
{
public:
    MockFeeder(int scfd)
        : EasySlave(allProperties, &m_io, FEEDER_SERIAL, (const byte_t*) "allowme")
        , m_io(scfd)
    {
        MockFeeder::SlavePropertyPtr props[] = {&m_times, &m_timesTimeout, &m_class, &m_calibrate, &m_type, &m_cache, &m_pingTimeout};
    }

private:
    UNIXSocketIO m_io;
};

// const byte_t MockFeeder::SERIAL = { 'f', 'e', 'e', 'd', 'e', 'r', 's', 'c' };
// byte_t MockFeeder::TOKEN = { 0, 0, 0, 0, 0, 0, 0, 0 };

void runClient(int scfd)
{
    MockFeeder feeder(scfd);

    if (!feeder.handshake()) {
        error("Handshake failed!");
    }

    std::thread decrementor([]() {
        while (1) {
            if (m_times.value > 0) {
                --m_times.value;
            }
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    });

    while (1) {
        feeder.loop();
    }

    int r = 5;
}

int main()
{
    const char* hostname = "localhost";
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    struct hostent* server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    struct sockaddr_in serveraddr;
    bzero((char*) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char*) server->h_addr, (char*) &serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(500503);

    /* connect: create a connection with the server */
    if (connect(sockfd, reinterpret_cast<sockaddr*>(&serveraddr), static_cast<socklen_t>(sizeof(serveraddr))) < 0)
        error("ERROR connecting");
    runClient(sockfd);
    return 0;
}