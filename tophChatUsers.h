#pragma once 

#define ui unsigned int
#define cp char*
#define MAX_FRIENDS 50
typedef struct tophChatUser TUSER;
typedef struct database TINFO;
static int numOfUsers = 0;
struct tophChatUser
{
	cp userName;
	ui hashedPassword;
	cp name;
	ui  hashID;
	cp email;
	cp phone;
	TUSER *friends[MAX_FRIENDS];
	ui friendCount;
	TUSER *next;
	TUSER *prev;
	TUSER *first;
	TUSER *last;

};


TUSER *addUser(cp _userName, cp _name, ui _hashedPassword);


ui hashID(cp userName);

//returns 0 if successful, 1 if not successful
//sends in both userName of the person who the inidivudal
//wants added, as well as the address of the person making
//the request
int addFriend(cp userName, TUSER* user);

//Check if user exists in the database
//traverses through linked list to check 
//if user is found, return address, 
//else null
TUSER *checkUser(cp userName, TUSER* user);

//Deltes user;
void deleteUser(TUSER *user);

//List friends
void showFriends(TUSER *user);
