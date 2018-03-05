#pragma once 

#define TUSER struct tophChatUsers
#define ui unsigned int
#define string char*
#define MAX_FRIENDS 50
typedef struct tophChatUsers
{
	string userName;
	ui hashedPassword;
	string name;
	ui  hashID;
	string email;
	string phone;
	TUSER *friends[MAX_FRIENDS];
} tophChatUsers;

TUSER *addUser(string _userName, string _name, ui _hashedPassword);

ui hashID(string userName);

