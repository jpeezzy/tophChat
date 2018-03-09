#pragma once 

#define ui unsigned int
#define cp char*
#define MAX_FRIENDS 50
#define MAX_USERS 100
typedef struct tophChatUser TUSER;
typedef struct database TINFO;

/*For our data structure, we will use a 
 * graph and represent the users as nodes*/
struct tophChatUser
{
	cp userName;
	ui hashedPassword;
	cp name;
	ui  hashID;
	cp email;
	cp phone;
	TUSER *friends[MAX_FRIENDS]; //Edges to graph
	ui friendCount;
	int socket;
};

struct database{
	ui numOfUsers; 
	TUSER *Users[MAX_USERS];
};

/*Creates a dataBase for data collecting  */
TINFO *createTINFO();

TUSER *addUser(cp _userName, cp _name, 
		ui _hashedPassword, TINFO* userBase);

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
TUSER *checkUser(cp userName, TUSER* user);


//List friends
void showFriends(TUSER *user);
