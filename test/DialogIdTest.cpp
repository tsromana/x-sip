#include "CppUTest/TestHarness.h"

TEST_GROUP(DialogIdTestGroup)
{
};

extern "C" {
#include <string.h>
#include "DialogId.h"
#include "TestingMessages.h"
#include "Messages.h"
}

TEST(DialogIdTestGroup, CreateDialogIdTest)
{
    char callid[] = "1";
    char localTag[] = "2";
    char remoteTag[] = "3";

    struct DialogId *dialogId = CreateFixedDialogId(callid, localTag, remoteTag);
    STRCMP_EQUAL("1", GetCallId(dialogId));
    STRCMP_EQUAL("2", GetLocalTag(dialogId));
    STRCMP_EQUAL("3", GetRemoteTag(dialogId));

    DestroyDialogId(&dialogId);
}

TEST(DialogIdTestGroup, DialogIdEqualTest)
{
    struct DialogId *id1 = CreateFixedDialogId((char *)"1", (char *)"2", (char *)"3");
    struct DialogId *id2 = CreateFixedDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_TRUE(DialogIdMatched(id1, id2));

    DestroyDialogId(&id1);
    DestroyDialogId(&id2);
}

TEST(DialogIdTestGroup, DialogIdUnequalTest)
{
    struct DialogId *id1 = CreateFixedDialogId((char *)"2", (char *)"2", (char *)"3");
    struct DialogId *id2 = CreateFixedDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_FALSE(DialogIdMatched(id1, id2));

    DestroyDialogId(&id1);
    DestroyDialogId(&id2);
}

TEST(DialogIdTestGroup, DialogIdUnequalTest1)
{
    struct DialogId *id1 = CreateFixedDialogId((char *)"4", (char *)"5", (char *)"6");
    struct DialogId *id2 = CreateFixedDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_FALSE(DialogIdMatched(id1, id2));

    DestroyDialogId(&id1);
    DestroyDialogId(&id2);
}

TEST(DialogIdTestGroup, DialogIdUnequalTest2)
{
    struct DialogId *id1 = CreateFixedDialogId((char *)"1", (char *)"1", (char *)"6");
    struct DialogId *id2 = CreateFixedDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_FALSE(DialogIdMatched(id1, id2));

    DestroyDialogId(&id1);
    DestroyDialogId(&id2);
}

TEST(DialogIdTestGroup, BuildDialIdFromResponseTest)
{
    MESSAGE *message = CreateMessage();
    struct DialogId *dialogid;

    ParseMessage(OK_MESSAGE_RECEIVED, message);
    dialogid = CreateDialogIdFromMessage(message);
    STRCMP_EQUAL(MessageGetCallId(message), GetCallId(dialogid));
    STRCMP_EQUAL(MessageGetFromTag(message), GetLocalTag(dialogid));
    STRCMP_EQUAL(MessageGetToTag(message), GetRemoteTag(dialogid));
    
    DestroyDialogId(&dialogid);
    DestroyMessage(&message);
}

TEST(DialogIdTestGroup, BuildDialogIdFromRequestTest)
{
    MESSAGE *message = CreateMessage();
    struct DialogId *dialogid;

    ParseMessage(INCOMMING_INVITE_MESSAGE, message);
    dialogid = CreateDialogIdFromMessage(message);
    STRCMP_EQUAL(MessageGetCallId(message), GetCallId(dialogid));
    STRCMP_EQUAL(MessageGetFromTag(message), GetRemoteTag(dialogid));
    
    DestroyDialogId(&dialogid);
    DestroyMessage(&message);
}

TEST(DialogIdTestGroup, GenerateCallIdTest)
{
    struct DialogId *dialogid = CreateEmptyDialogId();
    
    CHECK_TRUE(strlen(GetCallId(dialogid)) == 0);
    DestroyDialogId(&dialogid);
}

TEST(DialogIdTestGroup, GenerateLocalTagTest)
{
    struct DialogId *dialogid = CreateEmptyDialogId();
    
    CHECK_TRUE(strlen(GetLocalTag(dialogid)) == 0);
    DestroyDialogId(&dialogid);
}
