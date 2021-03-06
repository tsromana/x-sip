#include "Bool.h"
#include "SipMethod.h"

struct URI;
struct RequestLine;

void DestroyRequestLine(struct RequestLine *requestLine);
struct RequestLine *CreateEmptyRequestLine();
struct RequestLine *CreateRequestLine(SIP_METHOD m, struct URI *u);
struct RequestLine *RequestLineDup(struct RequestLine *rl);

char *RequestLineGetMethodName (struct RequestLine *r);
SIP_METHOD RequestLineGetMethod(struct RequestLine *r);
char *RequestLineGetSipVersion(struct RequestLine *r);
struct URI *RequestLineGetUri(struct RequestLine *r);

int RequestLineSetMethod(struct RequestLine *r, char *method);
int RequestLineSetSipVersion(struct RequestLine *r, char *version);
int RequestLineSetUri(struct RequestLine *r, struct URI *u);

char *RequestLine2String(char *string, struct RequestLine *r);
int ParseRequestLine(char *string, struct RequestLine *r);

BOOL RequestLineMatched(struct RequestLine *r1, struct RequestLine *r2);
BOOL RequestLineMethodMatched(struct RequestLine *r1, struct RequestLine *r2);
