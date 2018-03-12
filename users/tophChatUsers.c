#include "tophChatUsers.h"
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

TUSER *addUser(cp _userName, cp _name, 
		ui _hashedPassword, TINFO* userBase)
{
	/*Making sure inputs set in are correct*/
	TUSER *temp = malloc(sizeof(TUSER));
	if(!temp)
	{
		perror("NOT ENOUGH RAM");
	}
	//Making sure the user sends in proper inputs
	assert(_userName);
	assert(_name);

	temp->userName = _userName;
	temp->name = _name;
	temp->hashedPassword = _hashedPassword;
	temp->friendCount = 0;
	temp->hashID = hashID(_userName);

	/*Null everythin gwe don't know */
	temp->email = NULL;
	temp->phone = NULL;
	//add user to database
	userBase->Users[userBase->numOfUsers] = temp;
	//Increment Total user count
	userBase->numOfUsers++;
	
	return temp;
}

int deleteUser(TUSER *user)
{
	//free(user->userName);
	//user->userName = NULL;
	//user->hashedPassword = 0;
	//user->name = 0;
	//if (user->email != NULL) free(user->email);
	//if (user->phone != NULL) free(user->phone);
	//free(user->friends);
	free(user);
	return 0;
	
}
/*Function: hashID
 * returns: unsigned int 
 * input: userName
 * The reasoning is because no two people 
 * can have the same username, the chance of it 
 * falling to collision is lowered
 * it will use double hashing 
 * */
ui hashID(cp userName)
{
	return ((ui) atoi(userName)) * 53 
		- (ui)(atoi(userName)) % 73;
}

/*Function: createTINFO*/
TINFO *createTINFO()
{
	TINFO *temp = malloc(sizeof(TINFO));
	if(!temp) perror("OUT OF RAM");
	temp->numOfUsers = 0;
	return temp;
}
int deleteTINFO(TINFO *userBase)
{
	free(userBase);
	return 0;
}
/*Function: addFriend
 * returns: nameOfFriend
 * input: friend's username
 * */
int addFriend(cp userName, TUSER *user, TINFO *userBase)
{
	//Check if user exists
	for(ui i = 0; i < userBase->numOfUsers; i++)
	{
		if(strcmp(userName, userBase->Users[i]->userName) == 0)
		{
			//add user to friendList
			user->friends[user->friendCount] = userBase->Users[i];
			user->friendCount++;
		}
		else
			printf("User does not exist!\n");
	}
	return 0;
}

/*Function: showFriends
 * input, TUSER user 
 * output: prints out the friends TUSER has 
*/
void showFriends(TUSER *user)
{
	if(user->friends[0] == NULL)
	{
		printf("He doesn't have friends");
		return;
	}

	for(ui i = 0; i < user->friendCount; i++)
	{
		printf("%s \n", user->friends[i]->userName);
	}
	return;
}

//Function: findUserByName()
//iterates through the user database array and compares the username
//in each struct
//returns the user if true, returns null if false
TUSER *findUserByName(cp username, TINFO *userbase)
{
	for(ui i = 0; i<userbase->numOfUsers; i++)
	{
		if (strcmp(username, userbase->Users[i]->userName) == 0) 
			return userbase->Users[i];
	}
	return NULL;
}
#ifdef DEBUG
int main()
{
	/*creating a user */
	TINFO *dataBase = createTINFO();
	addUser("Justindlee","Justin", 1234, dataBase);
	findUserByName("Justindlee", dataBase);
	deleteUser(dataBase->Users[0]);
	free(dataBase);

	printf("Finished running!\n");
	return 0;
}
#endif
