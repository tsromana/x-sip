#include "CppUTest/TestHarness.h"

extern "C" {
#include "ContactHeader.h"
#include "Parser.h"
#include "stdio.h"
}

TEST_GROUP(ToTestGroup)
{
    void UriCheck(struct ContactHeader *header)
    {
        struct URI *uri = ContactHeaderGetUri(header);
        STRCMP_EQUAL("sip", UriGetScheme(uri));
        STRCMP_EQUAL("Martin.Yang", UriGetUser(uri));
        STRCMP_EQUAL("cs.columbia.edu",UriGetHost(uri));
    }
};

TEST(ToTestGroup, ContactHeaderParseTest)
{
    struct ContactHeader *toHeader = CreateContactHeader();

    char toString[]= "To:Martin Yang<sip:Martin.Yang@cs.columbia.edu>;tag=287447";

    Parse((char *)toString, toHeader, GetContactHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactHeaderGetName(toHeader));
    STRCMP_EQUAL("Martin Yang", ContactHeaderGetDisplayName(toHeader));

    UriCheck(toHeader);

    STRCMP_EQUAL("tag=287447", ContactHeaderGetParameters(toHeader));
    
    DestoryContactHeader((struct Header *)toHeader);
}

TEST(ToTestGroup, ContactHeaderWithSpaceParseTest)
{
    struct ContactHeader *toHeader = CreateContactHeader();
    char toString[]= "To:  Martin Yang  <sip:Martin.Yang@cs.columbia.edu>  ;  tag=287447";

    Parse((char *)toString, toHeader, GetContactHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactHeaderGetName(toHeader));
    STRCMP_EQUAL("Martin Yang", ContactHeaderGetDisplayName(toHeader));

    UriCheck(toHeader);

    STRCMP_EQUAL("tag=287447", ContactHeaderGetParameters(toHeader));
    
    DestoryContactHeader((struct Header *)toHeader);
}

TEST(ToTestGroup, ContactHeaderQuotedDisplayNameParseTest)
{
    struct ContactHeader *toHeader = CreateContactHeader();
    char toString[]= "To:  \"Martin Yang\"  <sip:Martin.Yang@cs.columbia.edu>  ;  tag=287447";

    Parse((char *)toString, toHeader, GetContactHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactHeaderGetName(toHeader));
    STRCMP_EQUAL("Martin Yang", ContactHeaderGetDisplayName(toHeader));

    UriCheck(toHeader);

    STRCMP_EQUAL("tag=287447", ContactHeaderGetParameters(toHeader));
    
    DestoryContactHeader((struct Header *)toHeader);
}

TEST(ToTestGroup, ContactHeaderNoDisplayNameParseTest)
{
    struct ContactHeader *toHeader = CreateContactHeader();
    char toString[]= "To:sip:Martin.Yang@cs.columbia.edu;tag=287447";

    Parse((char *)toString, toHeader, GetContactHeaderPattern(toString));    
    STRCMP_EQUAL("To", ContactHeaderGetName(toHeader));
    STRCMP_EQUAL("", ContactHeaderGetDisplayName(toHeader));

    UriCheck(toHeader);

    STRCMP_EQUAL("tag=287447", ContactHeaderGetParameters(toHeader));
    
    DestoryContactHeader((struct Header *)toHeader);
    
}

TEST(ToTestGroup, ContactParseTest)
{
    struct ContactHeader *contact = CreateContactHeader();
    char contactString[] = "Contact:<sip:Martin.Yang@cs.columbia.edu>";
   
    Parse((char*) contactString, contact, GetContactHeaderPattern(contactString));
    STRCMP_EQUAL("Contact", ContactHeaderGetName(contact));
    UriCheck(contact);

    DestoryContactHeader((struct Header *)contact);
}

TEST(ToTestGroup, Contact2StringTest)
{
    struct ContactHeader *contact = CreateContactHeader();
    char contactString[] = "Contact:<sip:alice@pc33.atlanta.com>";
    char result[128] = {0};

    Parse((char*) contactString, contact, GetContactHeaderPattern(contactString));
    ContactHeader2String(result,(struct Header *) contact);
    STRCMP_EQUAL("Contact:<sip:alice@pc33.atlanta.com>", result);

    DestoryContactHeader((struct Header *)contact);
}

TEST(ToTestGroup, ContactWithQuotedDisplayName2StringTest)
{
    struct ContactHeader *toHeader = CreateContactHeader();
    char toString[]= "To:\"Martin Yang\"<sip:Martin.Yang@cs.columbia.edu>;tag=287447";
    char result[128] = {0};

    Parse((char *)toString, toHeader, GetContactHeaderPattern(toString));    
    ContactHeader2String(result, (struct Header *)toHeader);
    STRCMP_EQUAL(toString, result);
    
    DestoryContactHeader((struct Header *)toHeader);
}

TEST(ToTestGroup, ContactStartWithSpaceParseTest)
{
    struct ContactHeader *toHeader = CreateContactHeader();
    char contactString[] = "Contact: <sip:bob@192.0.2.4>";

    Parse((char *)contactString, toHeader, GetContactHeaderPattern(contactString));
    DestoryContactHeader((struct Header *)toHeader);    
}
