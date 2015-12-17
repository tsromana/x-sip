#include "Bool.h"

struct Parameter;
struct Parameters;
/*
 *Function for parameter struct.
 */
struct Parameter *CreateEmptyParameter();
struct Parameter *CreateParameter(char *name, char *value);
void DestoryParameter(struct Parameter *p);
BOOL ParameterMatched(struct Parameter *p1, struct Parameter *p2);
/*
 *Function for parameters struct.
 */
struct Parameters *CreateParameters();
void DestoryParameters(struct Parameters *ps);
int ParseParameters(char *string, void *target);
int ParseParametersExt(char *string, void *target);
char *GetParameter(struct Parameters *p, char *name);
int AddParameter(struct Parameters *ps, char *name, char *value);
BOOL ParametersMatched(struct Parameters *ps1, struct Parameters *ps2);
char *Parameters2String(char *pos, void *ps, struct HeaderPattern *pattern);
char *Parameters2StringExt(char *pos, void *ps, struct HeaderPattern *pattern);
