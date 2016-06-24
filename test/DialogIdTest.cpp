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
    STRCMP_EQUAL("1", DialogIdGetCallId(dialogId));
    STRCMP_EQUAL("2", DialogIdGetLocalTag(dialogId));
    STRCMP_EQUAL("3", DialogIdGetRemoteTag(dialogId));

    DestoryDialogId(&dialogId);
}

TEST(DialogIdTestGroup, DialogIdEqualTest)
{
    struct DialogId *id1 = CreateFixedDialogId((char *)"1", (char *)"2", (char *)"3");
    struct DialogId *id2 = CreateFixedDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_TRUE(DialogIdMatched(id1, id2));

    DestoryDialogId(&id1);
    DestoryDialogId(&id2);
}

TEST(DialogIdTestGroup, DialogIdUnequalTest)
{
    struct DialogId *id1 = CreateFixedDialogId((char *)"2", (char *)"2", (char *)"3");
    struct DialogId *id2 = CreateFixedDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_FALSE(DialogIdMatched(id1, id2));

    DestoryDialogId(&id1);
    DestoryDialogId(&id2);
}

TEST(DialogIdTestGroup, DialogIdUnequalTest1)
{
    struct DialogId *id1 = CreateFixedDialogId((char *)"4", (char *)"5", (char *)"6");
    struct DialogId *id2 = CreateFixedDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_FALSE(DialogIdMatched(id1, id2));

    DestoryDialogId(&id1);
    DestoryDialogId(&id2);
}

TEST(DialogIdTestGroup, DialogIdUnequalTest2)
{
    struct DialogId *id1 = CreateFixedDialogId((char *)"1", (char *)"1", (char *)"6");
    struct DialogId *id2 = CreateFixedDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_FALSE(DialogIdMatched(id1, id2));

    DestoryDialogId(&id1);
    DestoryDialogId(&id2);
}

TEST(DialogIdTestGroup, BuildDialIdFromResponseTest)
{
    struct Message *message = CreateMessage();
    struct DialogId *dialogid;

    ParseMessage(INVITE_200OK_MESSAGE, message);
    dialogid = CreateDialogIdFromMessage(message);
    STRCMP_EQUAL(MessageGetCallId(message), DialogIdGetCallId(dialogid));
    STRCMP_EQUAL(MessageGetFromTag(message), DialogIdGetLocalTag(dialogid));
    STRCMP_EQUAL(MessageGetToTag(message), DialogIdGetRemoteTag(dialogid));
    
    DestoryDialogId(&dialogid);
    DestoryMessage(&message);
}

TEST(DialogIdTestGroup, BuildDialogIdFromRequestTest)
{
    struct Message *message = CreateMessage();
    struct DialogId *dialogid;

    ParseMessage(INCOMMING_INVITE_MESSAGE, message);
    dialogid = CreateDialogIdFromMessage(message);
    STRCMP_EQUAL(MessageGetCallId(message), DialogIdGetCallId(dialogid));
    STRCMP_EQUAL(MessageGetFromTag(message), DialogIdGetRemoteTag(dialogid));
    
    DestoryDialogId(&dialogid);
    DestoryMessage(&message);
}

TEST(DialogIdTestGroup, GenerateCallIdTest)
{
    struct DialogId *dialogid = CreateDialogId();
    
    CHECK_TRUE(strlen(DialogIdGetCallId(dialogid)) != 0);
    DestoryDialogId(&dialogid);
}

TEST(DialogIdTestGroup, GenerateLocalTagTest)
{
    struct DialogId *dialogid = CreateDialogId();
    
    CHECK_TRUE(strlen(DialogIdGetLocalTag(dialogid)) != 0);
    DestoryDialogId(&dialogid);
}
