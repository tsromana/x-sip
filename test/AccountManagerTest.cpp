#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "AccountMock.h"

extern "C" {
#include <stdio.h>
#include "Accounts.h"
#include "AccountManager.h"
}

void AddFirstAccount()
{
    struct Account *first;
    first = CreateAccount(
                          (char *)"88001", 
                          (char *)"88001", 
                          (char *)"192.168.10.62", 
                          (char *)"192.168.10.72");

    AddAccount(first);
}

int AddSecondAccount()
{
    struct Account *second;
    second = CreateAccount(
                           (char *)"88002", 
                           (char *)"88002", 
                           (char *)"192.168.10.62", 
                           (char *)"192.168.10.72");
    return AddAccount(second);
}

int AddThirdAccount()
{
    struct Account *third;
    third = CreateAccount(
                          (char *)"88003", 
                          (char *)"88003", 
                          (char *)"192.168.10.62", 
                          (char *)"192.168.10.72");

    return AddAccount(third);
}

void AccountInitMock()
{
    AddFirstAccount();
    AddSecondAccount();
    AddThirdAccount();
}

TEST_GROUP(AccountManagerTestGroup)
{
    void setup()
    {
        ClearAccount();
        ::AddFirstAccount();
    }

    void teardown()
    {
        ClearAccount();
    }
};

TEST(AccountManagerTestGroup, AddOneAccountTest)
{
    struct Account *retrived = GetAccount(0);

    STRCMP_EQUAL("88001", AccountGetUserName(retrived));
    STRCMP_EQUAL("88001", AccountGetAuthName(retrived));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxy(retrived));
    STRCMP_EQUAL("192.168.10.72", AccountGetRegistrar(retrived));
    CHECK_EQUAL(1, TotalAccount());

}

TEST(AccountManagerTestGroup, AddTwoAccountsTest)
{
    AddSecondAccount();
    CHECK_EQUAL(2, TotalAccount());
}

TEST(AccountManagerTestGroup, AddThreeAccountsTest)
{
    AddSecondAccount();
    AddThirdAccount();
    CHECK_EQUAL(3, TotalAccount());
}

TEST(AccountManagerTestGroup, ClearAccountTest)
{
    ClearAccount();
    CHECK_EQUAL(0, TotalAccount());
}

TEST(AccountManagerTestGroup, ClearMulitAccountTest)
{
    AddSecondAccount();
    AddThirdAccount();
    ClearAccount();
    CHECK_EQUAL(0, TotalAccount());
}

TEST(AccountManagerTestGroup, RemoveAccountTest)
{
    RemoveAccount(0);
    CHECK_EQUAL(0, TotalAccount());
}

TEST(AccountManagerTestGroup, RemoveMultiAccountTest)
{
    AddSecondAccount();
    AddThirdAccount();

    RemoveAccount(0);
    CHECK_EQUAL(2, TotalAccount());
    RemoveAccount(0);
    CHECK_EQUAL(1, TotalAccount());
    RemoveAccount(0);
    CHECK_EQUAL(0, TotalAccount());
}

TEST(AccountManagerTestGroup, InitTest)
{
    UT_PTR_SET(AccountInit, AccountInitMock);

    ClearAccount();

    AccountInit();
    CHECK_EQUAL(3, TotalAccount());

    struct Account *a = GetAccount( 0);
    STRCMP_EQUAL("88001", AccountGetUserName(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxy(a));
    STRCMP_EQUAL("192.168.10.72", AccountGetRegistrar(a));


    a = GetAccount(1);
    STRCMP_EQUAL("88002", AccountGetUserName(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxy(a));
    STRCMP_EQUAL("192.168.10.72", AccountGetRegistrar(a));

    a = GetAccount(2);
    STRCMP_EQUAL("88003", AccountGetUserName(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxy(a));
    STRCMP_EQUAL("192.168.10.72", AccountGetRegistrar(a));
}

TEST(AccountManagerTestGroup, GetOurOfRangeAccountTest)
{
    UT_PTR_SET(AccountInit, AccountInitMock);
    ClearAccount();

    AccountInit();
    CHECK_EQUAL(3, TotalAccount());

    struct Account *a = GetAccount( -100);
    POINTERS_EQUAL(NULL, a);
    
    a = GetAccount(100);
    POINTERS_EQUAL(NULL, a);
    
}

TEST(AccountManagerTestGroup, AddAccountReturnTest)
{
    CHECK_EQUAL(1,AddSecondAccount());
    CHECK_EQUAL(2,AddThirdAccount());
}

IGNORE_TEST(AccountManagerTestGroup, BindAccountTest)
{
    struct Account *account = GetAccount(0);

    BindAccount(0);
    
    CHECK_EQUAL(TRUE, AccountBinded(account));
}