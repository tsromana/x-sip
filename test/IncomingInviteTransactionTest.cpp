#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TransportMock.h"
#include "TestingMessages.h"

extern "C" {
#include <stdio.h>
#include "Messages.h"
#include "MessageTransport.h"
#include "TransactionManager.h"
}

TEST_GROUP(IncomingInviteTransactionTestGroup)
{
    void setup(){
        UT_PTR_SET(ReceiveMessageCallback, MessageReceived);
        UT_PTR_SET(Transporter, &MockTransporter);
   }

    void teardown() {
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(IncomingInviteTransactionTestGroup, ReceiveInvitedCreateTransactionTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INCOMMING_INVITE_MESSAGE);
    ReceiveInMessage(stringReceived);    
    CHECK_EQUAL(1, CountTransaction());
    CHECK_TRUE(GetTransactionBy((char *)"z9hG4bK27dc30b4",(char *)"INVITE") != NULL);

    EmptyTransactionManager();
}

TEST(IncomingInviteTransactionTestGroup, Send100TryingTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall("SendOutMessageMock");
    ReceiveInMessage(stringReceived);    

    EmptyTransactionManager();
}







