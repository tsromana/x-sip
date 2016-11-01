#include <stdio.h>

#include "Utils.h"
#include "Dialog.h"
#include "DialogId.h"
#include "UserAgent.h"
#include "UserAgentManager.h"
#include "Accounts.h"
#include "TransactionManager.h"
#include "Messages.h"
#include "Transaction.h"
#include "MessageBuilder.h"
#include "Transporter.h"
#include "Header.h"
#include "URI.h"
#include "ContactHeader.h"
#include "Session.h"
#include "CallEvents.h"
#include "DialogManager.h"
#include "AccountManager.h"

struct Dialog {
    struct Transaction *transaction;
    struct DialogId *id;
    struct UserAgent *ua;
    struct Session *session;
    URI *remoteTarget;
    URI *remoteUri;
    
    SIP_METHOD requestMethod;
    unsigned int localSeqNumber;
    unsigned int remoteSeqNumber;
    enum DIALOG_STATE state;
};

struct TransactionEventAction {
    SIP_METHOD requestMethod;
    int event;
    void (*action)(struct Dialog *dialog, int event, MESSAGE *message);
};

URI *DialogGetRemoteUri(struct Dialog *dialog)
{
    assert(dialog != 0);
    
    return dialog->remoteUri;
}

char *DialogGetCallId(struct Dialog *dialog)
{
    return GetCallId(dialog->id);
}

char *DialogGetRemoteTag(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return GetRemoteTag(dialog->id);
}

char *DialogGetLocalTag(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return GetLocalTag(dialog->id);
}

URI *DialogGetRemoteTarget(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->remoteTarget;
}

struct DialogId *DialogGetId(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->id;
}

struct UserAgent *DialogGetUserAgent(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->ua;
}

void DialogSetRequestMethod(struct Dialog *dialog, SIP_METHOD method)
{
    assert(dialog != NULL);
    dialog->requestMethod = method;
}

SIP_METHOD DialogGetRequestMethod(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->requestMethod;
}

unsigned int DialogGetLocalSeqNumber(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->localSeqNumber;
}

void SetLocalSeqNumber(struct Dialog *dialog, int seq)
{
    assert(dialog != NULL);
    dialog->localSeqNumber = seq;
}

void DialogSetLocalTag(struct Dialog *dialog, const char *localTag)
{
    struct DialogId *id = DialogGetId(dialog);
    SetLocalTag(id, (char *)localTag);
}

unsigned int DialogGetRemoteSeqNumber(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->remoteSeqNumber;
}

enum DIALOG_STATE DialogGetState(struct Dialog *dialog)
{
    assert(dialog != NULL);
    return dialog->state;
}

void SetDialogState(struct Dialog *dialog, enum DIALOG_STATE state)
{
    assert(dialog != NULL);
    dialog->state = state;
}

void DialogSetRemoteUri(struct Dialog *dialog, URI *uri)
{
    assert(uri != NULL);
    assert(dialog != NULL);
    
    dialog->remoteUri = uri;
}

void ExtractRemoteTarget(struct Dialog *dialog, MESSAGE *message)
{
    CONTACT_HEADER *c = (CONTACT_HEADER *)MessageGetHeader(HEADER_NAME_CONTACT, message);
    URI *uri = ContactHeaderGetUri(c);
    dialog->remoteTarget = UriDup(uri);
}

void DialogAck(struct Dialog *dialog)
{
    MESSAGE *ack = BuildAckMessage(dialog);
    SendMessage(ack);
    DestroyMessage(&ack);
}

void DialogReceiveOk(struct Dialog *dialog, MESSAGE *message)
{
    struct DialogId *dialogid = DialogGetId(dialog);

    ExtractDialogId(dialogid, message);                    
    ExtractRemoteTarget(dialog, message);
    SetDialogState(dialog, DIALOG_STATE_CONFIRMED);
    
    DialogAck(dialog);

    if (NotifyCallManager != NULL)
        NotifyCallManager(CALL_ESTABLISHED, DialogGetUserAgent(dialog));

    dialog->session = CreateSession();
}

void DialogReceiveRinging(struct Dialog *dialog, MESSAGE *message)
{
    if (NotifyCallManager != NULL)
        NotifyCallManager(CALL_REMOTE_RINGING, DialogGetUserAgent(dialog));
}

void HandleRegisterTransactionEvent (struct Dialog *dialog, int event, MESSAGE *message)
{
    struct UserAgent *ua = DialogGetUserAgent(dialog);
    
    if (event == TRANSACTION_EVENT_200OK) {    
        if (MessageGetExpires(message) != 0) {
            UaSetBinded(ua);
        } else {
            UaSetUnbinded(ua);
        }
    } else if (event == TRANSACTION_EVENT_401UNAUTHORIZED) {
        MESSAGE *authMessage = BuildAuthorizationMessage(dialog, message);
        DialogNewTransaction(dialog, authMessage, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    }
}

void HandleByeTransactionEvent(struct Dialog *dialog, int event)
{
}

void HandleInviteTransactionEvent(struct Dialog *dialog, int event, struct Message *message)
{
    if (event == TRANSACTION_EVENT_200OK) {
        DialogReceiveOk(dialog, message);
    } else if (event == TRANSACTION_EVENT_180RINGING) {
        DialogReceiveRinging(dialog, message);
    }
}

void HandleNewTransactionEvent(struct Dialog *dialog, int event, MESSAGE *message)
{
    int account = FindMessageDestAccount(message);
    struct UserAgent *ua = AddUserAgent(account);
    struct Dialog *newDialog = AddDialog(NULL, ua);

    DialogNewTransaction(newDialog, message, TRANSACTION_TYPE_SERVER_INVITE);
}

struct TransactionEventAction TransactionEventActions[] = {
    {SIP_METHOD_REGISTER, TRANSACTION_EVENT_200OK, HandleRegisterTransactionEvent},
    {SIP_METHOD_REGISTER, TRANSACTION_EVENT_401UNAUTHORIZED, HandleRegisterTransactionEvent},
    {SIP_METHOD_INVITE, TRANSACTION_EVENT_200OK, HandleInviteTransactionEvent},
    {SIP_METHOD_INVITE, TRANSACTION_EVENT_180RINGING, HandleInviteTransactionEvent},
    {SIP_METHOD_INVITE, TRANSACTION_EVENT_NEW, HandleNewTransactionEvent},
    {-1, -1, 0},
};

void OnTransactionEventImpl(struct Dialog *dialog,  int event, MESSAGE *message)
{
    struct Transaction *t;
    SIP_METHOD requestMethod;
    struct TransactionEventAction *action;

    if (dialog == NULL) {
        requestMethod = MessageGetMethod(message);
    } else {
        t = dialog->transaction;
        requestMethod = MessageGetMethod(GetTransactionRequest(t));
    }

    action = TransactionEventActions;    
    for (; action->requestMethod != -1; action ++)
    {
        if (requestMethod == action->requestMethod && event == action->event) {
            action->action(dialog, event, message);
        }
    }    
}

void (*OnTransactionEvent)(struct Dialog *dialog, int event, MESSAGE *message) =
    OnTransactionEventImpl;

struct Transaction *DialogNewTransaction(struct Dialog *dialog, MESSAGE *message, int type)
{
    struct DialogId *id = DialogGetId(dialog);
    struct Transaction *t = AddTransaction(message, (struct TransactionUser *)dialog, type);
    dialog->transaction = t;

    if (type == TRANSACTION_TYPE_CLIENT_INVITE) {
        SetLocalTag(id, MessageGetFromTag(message));
        SetCallId(id, MessageGetCallId(message));
        SetLocalSeqNumber(dialog, MessageGetCSeqNumber(message));
    } else if (type == TRANSACTION_TYPE_SERVER_INVITE) {
        SetRemoteTag(id, MessageGetFromTag(message));
        SetCallId(id, MessageGetCallId(message));
        ExtractRemoteTarget(dialog, message);

        if (NotifyCallManager != NULL) {
            NotifyCallManager(CALL_INCOMING, DialogGetUserAgent(dialog));
        }
    }
    
    return t;
}

void DialogOk(struct Dialog *dialog)
{
    struct DialogId *id = DialogGetId(dialog);
    MESSAGE *message = Build200OkMessage(NULL, GetTransactionRequest(dialog->transaction));

    dialog->remoteSeqNumber = MessageGetCSeqNumber(GetTransactionRequest(dialog->transaction));     
    ResponseWith200OK(dialog->transaction);

    if (DialogGetState(dialog) == DIALOG_STATE_NON_EXIST) {        
        SetLocalTag(id, MessageGetToTag(message));
        SetDialogState(dialog, DIALOG_STATE_CONFIRMED);
        
        dialog->session = CreateSession();

    } else if (DialogGetState(dialog) == DIALOG_STATE_CONFIRMED) {
        if (DialogGetRequestMethod(dialog) == SIP_METHOD_BYE) {
            SetDialogState(dialog, DIALOG_STATE_TERMINATED);
        }
    }

    DestroyMessage(&message);
}

void DialogReceiveBye(struct Dialog *dialog, MESSAGE *bye)
{
    DialogNewTransaction(dialog, bye, TRANSACTION_TYPE_SERVER_NON_INVITE);
    DialogOk(dialog);
}

void DialogTerminate(struct Dialog *dialog)
{
    MESSAGE *bye = BuildByeMessage(dialog);
    DialogNewTransaction(dialog, bye, TRANSACTION_TYPE_CLIENT_NON_INVITE);
    dialog->state = DIALOG_STATE_TERMINATED;
}

void DialogInvite(struct Dialog *dialog)
{
    MESSAGE *invite = BuildInviteMessage(dialog, (char *)"88002");
    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);
}

void DialogBye(struct Dialog *dialog)
{
    MESSAGE *bye = BuildByeMessage(dialog);
    DialogNewTransaction(dialog, bye, TRANSACTION_TYPE_CLIENT_NON_INVITE);
}

struct Dialog *CreateDialog(struct DialogId *dialogid, struct UserAgent *ua)
{
    struct Dialog *dialog = calloc(1, sizeof(struct Dialog));

    if (dialogid == NULL) {
        dialog->id = CreateEmptyDialogId();
    } else {
        dialog->id = dialogid;
    }

    dialog->ua = ua;

    return dialog;
}

void DestroyDialog(struct Dialog **dialog)
{
    struct Dialog *d = *dialog;

    if (d != NULL) {
        DestroyDialogId(&d->id);
        DestroyUri(&d->remoteTarget);
        DestroyUri(&(d->remoteUri));
        DestroySession(&d->session);
        free(d);
        *dialog = NULL;
    }
}
