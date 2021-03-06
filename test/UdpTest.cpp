#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"

extern "C" {
#include "Transporter.h"
#include "SipUdp.h"
#include "Udp.h"
}

TEST_GROUP(TransporterUdpTestGroup)
{
    void setup()
    {
        UT_PTR_SET(xsocket, socket_mock);
        UT_PTR_SET(xbind, bind_mock);
        UT_PTR_SET(xsendto, sendto_mock);
        UT_PTR_SET(xrecvfrom, recvfrom_mock);
    }
    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(TransporterUdpTestGroup, BindFailedTest)
{
    mock().expectOneCall("socket").andReturnValue(0);
    mock().expectOneCall("bind").andReturnValue(-1);

    CHECK_EQUAL(-1, SipUdpInit(0));
}


TEST(TransporterUdpTestGroup, CreateSocketFailedTest)
{
    mock().expectOneCall("socket").andReturnValue(-1);
    CHECK_EQUAL(-1, SipUdpInit(0));
}

TEST(TransporterUdpTestGroup, InitSuccessedTest)
{
    mock().expectOneCall("socket").andReturnValue(0);
    mock().expectOneCall("bind").andReturnValue(0);

    CHECK_EQUAL(0, SipUdpInit(0));
}

int UdpSendMock(char *message, char *destaddr, int port, int fd)
{
    mock().actualCall("UdpSend").withStringParameter("Message", message).
        withStringParameter("addr", destaddr).
        withIntParameter("port", port).
        withIntParameter("fd", fd);
    return 0;
}

TEST(TransporterUdpTestGroup, SendMessageTest)
{
    char message[] = "Testing message";

    UT_PTR_SET(UdpSend, UdpSendMock);

    mock().expectOneCall("UdpSend").withStringParameter("Message", message).
        withStringParameter("addr", (char *)"192.168.10.62").
        withIntParameter("port", 5060).
        withIntParameter("fd", 201);
    
    SipUdpSendMessage(message, (char *)"192.168.10.62", 5060, 201);
}

TEST(TransporterUdpTestGroup, ReceiveMessageTest)
{
    char message[64] ={0};
    
    mock().expectOneCall("recvfrom");
    SipUdpReceiveMessage(message, 0);
}

