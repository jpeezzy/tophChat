#pragma once

#include "constants.h"

#define ui unsigned int
#define uli unsigned long int
#define cp char *
#define MAX_FRIENDS 50
#define MAX_USERS 100

typedef struct tophChatUser TUSER;
typedef struct database TINFO;

/*For our data structure, we will use a 
 * graph and represent the users as nodes*/
struct tophChatUser
{
	cp userName;
	ui password;
	ui hashedPassword;
	cp name;
	ui hashID;
	cp email;
	cp phone;
	TUSER *friends[MAX_FRIENDS]; //Edges to graph
	ui friendCount;
	uli publicKey;
	int numOfRoomUserIn;
	int listOfRooms[CHAT_ROOM_LIMIT];
	int socket;
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

TUSER *addUser(cp _userName, cp _name,
			   ui _hashedPassword, TINFO *userBase);

//Deletes the user
int deleteUser(TUSER *user);

ui hashID(cp userName);

//returns 0 if successful, 1 if not successful
//sends in both userName of the person who the inidivudal
//wants added, as well as the address of the person making
//the request
int addFriend(cp userName, TUSER *user, TINFO *userbase);

//Check if user exists in the database
//traverses through linked list to check
//if user is found, return address,
//else null
TUSER *checkUserByName(cp userName, TUSER *user);

//List friends
void showFriends(TUSER *user);

//Lists all the friends 
/* returns a list of the friends like this
 * james/arvin/boostedgorilla/abel + '\0'
 */
int getFriends(cp username, TINFO *userbase, cp list);

//Authentification gets the actual user struct
//and checks if the password is correct
//return 0 for true 
//return 1 for false
int authentifyUser(cp username, ui hashpassword, TINFO *userbase);

int checkIfFriends(TUSER *user1, TUSER *user2);

//Find a user based off username
TUSER *findUserByName(cp username, TINFO *userbase);

//Remove friend
int deleteFriend(TUSER *user1, cp username);

//chek if user is online,
int checkSocket(TUSER *user);

//Change socket based off if they are online
int changeSocket(TUSER *user, int socket);

//Saves the user to a text file (appends it to the end 
int saveUser(TUSER *user);

//Loads the user
int loadUser(cp textFile, TINFO *userBase);
