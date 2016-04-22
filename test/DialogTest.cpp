#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"
#include "TransportMock.h"

extern "C" {
#include <stdio.h>
#include "UserAgentManager.h"
#include "Transaction.h"
#include "UserAgent.h"
#include "Dialog.h"
#include "DialogId.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "TransactionManager.h"
#include "MessageTransport.h"
#include "RequestLine.h"
}

TEST_GROUP(DialogTestGroup)
{
    struct UserAgent *ua;
    struct Dialog *dialog;
    struct Message *invite;
    struct Message *ok;
    void setup()
    {
        UT_PTR_SET(Transporter, &MockTransporter);
        UT_PTR_SET(ReceiveMessageCallback, MessageReceived);

        ua = BuildUserAgent();
        dialog = CreateDialog(NULL_DIALOG_ID, ua);
        invite = BuildInviteMessage(dialog);
        ok = Build200OKMessage(invite);
        
    }

    void teardown()
    {
        EmptyTransactionManager();
        DestoryUserAgent(&ua);
        DestoryMessage(&ok);
        mock().checkExpectations();
        mock().clear();
    }
};

int SendOutAckMessageMock(char *message)
{
    struct Message *m = CreateMessage();
    char *remoteTag = NULL;

    ParseMessage(message, m);
    remoteTag = MessageGetToTag(m);    
    mock().actualCall(SEND_OUT_MESSAGE_MOCK).withParameter("RemoteTag", remoteTag).returnIntValue();

    CHECK_EQUAL(SIP_METHOD_ACK, RequestLineGetMethod(MessageGetRequestLine(m)));
    DestoryMessage(&m);

    return 0;
}

static struct MessageTransporter MockTransporterForAck = {
    SendOutAckMessageMock,
    ReceiveInMessageMock,
};

TEST(DialogTestGroup, AckRequestSendAfterInviteSuccessedTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("RemoteTag", "as6151ad25");
   
    AddClientInviteTransaction(invite, (struct TransactionUserNotifiers *)dialog);
    UT_PTR_SET(Transporter, &MockTransporterForAck);

    ReceiveInMessage(revMessage);
}

TEST(DialogTestGroup, AddTransactionTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    struct Transaction *transaction = DialogAddClientNonInviteTransaction(dialog, invite);

    POINTERS_EQUAL(transaction, GetTransaction(MessageGetViaBranch(invite), MessageGetCSeqMethod(invite)));
}

TEST(DialogTestGroup, UACDialogIdTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};
    char okString[MAX_MESSAGE_LENGTH] = {0};
    struct Message *originInvite = BuildInviteMessage(dialog);

    Message2String(okString, ok);
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(okString);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));

   
    DialogAddClientInviteTransaction(dialog, invite);
    ReceiveInMessage(revMessage);
    
    STRCMP_EQUAL(MessageGetFromTag(originInvite), DialogIdGetLocalTag(DialogGetId(dialog)));    
    STRCMP_EQUAL(MessageGetCallId(originInvite), DialogIdGetCallId(DialogGetId(dialog)));
    STRCMP_EQUAL(MessageGetToTag(ok), DialogIdGetRemoteTag(DialogGetId(dialog)));    

    DestoryMessage(&originInvite);
}

TEST(DialogTestGroup, UASDialogIdTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);

    DialogAddServerTransaction(dialog, invite);
    DialogSend200OKResponse(dialog);

    STRCMP_EQUAL(MessageGetToTag(ok), DialogIdGetLocalTag(DialogGetId(dialog)));
    STRCMP_EQUAL(MessageGetFromTag(invite), DialogIdGetRemoteTag(DialogGetId(dialog)));
    STRCMP_EQUAL(MessageGetCallId(invite), DialogIdGetCallId(DialogGetId(dialog)));
}

TEST(DialogTestGroup, UACDialogLocalSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    DialogAddClientInviteTransaction(dialog, invite);

    CHECK_EQUAL(MessageGetCSeqNumber(invite), DialogGetLocalSeqNumber(dialog));
}

TEST(DialogTestGroup, UACDialogRemoteSeqNumberTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));
   
    DialogAddClientInviteTransaction(dialog, invite);
    ReceiveInMessage(revMessage);

    CHECK_EQUAL(EMPTY_DIALOG_SEQNUMBER, DialogGetRemoteSeqNumber(dialog));
}

TEST(DialogTestGroup, UACDialogConfirmedTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_INVITE));
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_ACK));
   
    DialogAddClientInviteTransaction(dialog, invite);
    ReceiveInMessage(revMessage);

    CHECK_EQUAL(DIALOG_STATE_CONFIRMED, DialogGetState(dialog));

}

TEST(DialogTestGroup, UASDialogConfirmedTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    DialogAddServerTransaction(dialog, invite);
    DialogSend200OKResponse(dialog);

    CHECK_EQUAL(DIALOG_STATE_CONFIRMED, DialogGetState(dialog));
    DestoryMessage(&ok);
}

TEST(DialogTestGroup, UASDialogRemoteSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    DialogAddServerTransaction(dialog, invite);
    DialogSend200OKResponse(dialog);

    CHECK_EQUAL(MessageGetCSeqNumber(invite), DialogGetRemoteSeqNumber(dialog));
    DestoryMessage(&ok);
}

TEST(DialogTestGroup, UASDialogLocalSeqNumberTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    DialogAddServerTransaction(dialog, invite);
    DialogSend200OKResponse(dialog);

    CHECK_EQUAL(EMPTY_DIALOG_SEQNUMBER, DialogGetLocalSeqNumber(dialog));
    DestoryMessage(&ok);
}

TEST(DialogTestGroup, UACDialogTerminateTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_BYE));

    DialogAddServerTransaction(dialog, invite);
    DialogSend200OKResponse(dialog);
    DialogTerminate(dialog);

    CHECK_EQUAL(DIALOG_STATE_TERMINATED, DialogGetState(dialog));

    DestoryMessage(&ok);    
}

TEST(DialogTestGroup, UASDialogTerminateTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withIntParameter("StatusCode", 100);
    DialogAddServerTransaction(dialog, invite);    
}
