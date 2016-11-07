#include <stdio.h>
#include <assert.h>

#include "UserAgentManager.h"
#include "UserAgent.h"
#include "CallEvents.h"
#include "UserAgentManager.h"
#include "ClientUdp.h"
#include "Transporter.h"
#include "Maps.h"
#include "Parameter.h"
#include "utils/StringExt.h"

struct IntStringMap CallEventMap[] = {
    INT_STRING_MAP(CALL_INCOMING),
    INT_STRING_MAP(CALL_ESTABLISHED),
    INT_STRING_MAP(CALL_REMOTE_RINGING),
    INT_STRING_MAP(CALL_FINISHED),
    
    INT_STRING_MAP(ACCEPT_CALL),
    {-1, ""},
};

struct UserAgent *CallOut(int account, char *dest)
{
    struct UserAgent *ua = AddUa(account);
    UaMakeCall(ua);

    return ua;
}

void EndCall(struct UserAgent *ua)
{
    UaEndCall(ua);
}

void AcceptCall(struct UserAgent *ua)
{
    UaAcceptCall(ua);
}

BOOL HandleClientMessage(char *string)
{
    struct UserAgent *ua = GetUa(3);
    AcceptCall(ua);
    
    return 1;
}

void BuildClientMessage(char *msg, int ua, enum CALL_EVENT event)
{
    assert(msg != NULL);
    
    sprintf(msg, "ua=%d;event=%s\r\n", ua, IntMap2String(event, CallEventMap));
}

void ParseClientMessage(char *msg, struct ClientEvent *event)
{
    assert (msg != NULL);
    assert (event != NULL);

    RemoveTailingCRLF(msg);
    struct Parameters *ps = CreateParameters();
    ParseParametersExt(msg, ps);
    

    event->event = StringMap2Int(GetParameter(ps, "event"), CallEventMap);
    event->ua = atoi(GetParameter(ps, "ua"));    

    DestroyParameters(&ps);
}

void NotifyCmImpl(int event, struct UserAgent *ua)
{
    char msg[CLIENT_MESSAGE_MAX_LENGTH] = {0};

    BuildClientMessage(msg, GetUaPosition(ua), event);
    ClientTransporter->send(msg, "192.168.10.1", 5556, ClientTransporter->fd);
}

void (*NotifyCm)(int event, struct UserAgent *ua) = NotifyCmImpl;
