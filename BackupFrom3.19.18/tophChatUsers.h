#pragma once

#include "constants.h"

#define ui unsigned int
#define uli unsigned long int
#define MAX_FRIENDS 5
#define MAX_USERS 6

typedef struct tophChatUser TUSER;
typedef struct database TINFO;

/*For our data structure, we will use a 
 * graph and represent the users as nodes*/
struct tophChatUser
{
	char* userName;
	char* password;
	ui hashedPassword;
	char* name;
	ui hashID;
	char* email;
	char* phone;
	TUSER *friends[MAX_FRIENDS]; //Edges to graph
	ui friendCount;
	uli publicKey;
	int numOfRoomUserIn;
	int listOfRooms[CHAT_ROOM_LIMIT];
	int socket;
	int isRegistered;
};

struct database
{
	ui numOfUsers;
	TUSER *Users[MAX_USERS];
	uli publicKey;
	uli privateKey;
};

/*Creates a dataBase for data collecting  */
TINFO *createTINFO();

TUSER *addUser(char* _userName, char* _name,
			   char* _Password, TINFO *userBase);

//Deletes the user
int deleteUser(TUSER *user);

ui hashID(char* userName);

//returns 0 if successful, 1 if not successful
//sends in both userName of the person who the inidivudal
//wants added, as well as the address of the person making
//the request
int addFriend(char* userName, TUSER *user, TINFO *userbase);

//Check if user exists in the database
//traverses through linked list to check
//if user is found, return address,
//else null
TUSER *checkUserByName(char* userName, TUSER *user);

//List friends
void showFriends(TUSER *user);

//Lists all the friends
/* returns a list of the friends like this
 * james/arvin/boostedgorilla/abel + '\0'
 */
int getFriends(char* username, TINFO *userbase, char* list);


//returns the list of its friends THat are online
//for this problem, first memset an initialize list to zero or calloc list
int getOnlineFriends(char *username, TINFO *userbase, char* list);

//Authentification gets the actual user struct
//and checks if the password is correct
//return 0 for true
//return 1 for false
int authentifyUser(char* username, char* _password, TINFO *userbase);

int checkIfFriends(TUSER *user1, TUSER *user2);

//Find a user based off username
TUSER *findUserByName(char* username, TINFO *userbase);

//Remove friend
int deleteFriend(TUSER *user1, char* username);

//chek if user is online,
int checkSocket(TUSER *user);

//Change socket based off if they are online
int changeSocket(TUSER *user, int socket);

//Saves the user to a text file (appends it to the end
int saveUser(TUSER *user);

//Loads the user
int loadUser(char* textFile, TINFO *userBase);
