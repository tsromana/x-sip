#define LOCAL_PORT  5060
#define PROXY_PORT  5060
#define REGISTRAR_PORT 5060

struct Message;
struct UserAgent;
struct Dialog;

struct Message *BuildBindingMessage(struct Dialog *dialog);
struct Message *BuildInviteMessage(struct Dialog *dialog);
struct Message *BuildAckMessage(struct Dialog *dialog);

struct Message *Build100TryingMessage(struct Message *invite);
