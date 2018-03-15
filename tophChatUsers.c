#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <sys/types.h>

#include "tophChatUsers.h"
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
	temp->numOfRoomUserIn = 0;
	temp->socket = -1; 

	/*Null everythin gwe don't know */
	temp->email = NULL;
	temp->phone = NULL;
	//add user to database
	userBase->Users[userBase->numOfUsers] = temp;
	//Increment Total user count
	userBase->numOfUsers++;

	//Settin the chat rooms to negative
	for(int i = 0; i < CHAT_ROOM_LIMIT; i++)
	{
		temp->listOfRooms[i] = -1;
	}

	for(int i = 0; i < MAX_FRIENDS; i++)
	{
		temp->friends[i] = NULL;
	}
	return temp;
}

int deleteUser(TUSER *user)
{
	free(user->userName);
	user->userName = NULL;
	free(user->name);
	user->name = NULL;
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
	int i = 0;
	while(userBase->Users[i] != NULL)
	{
		deleteUser(userBase->Users[i]);
		userBase->Users[i] = NULL;
		i++;
	}
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
			TUSER *potentialFriend = userBase->Users[i];
			//add user to friendList
			user->friends[user->friendCount] = potentialFriend;
			user->friendCount++;

			//add friend from the other end
			potentialFriend->friends[potentialFriend->friendCount] = user;
			potentialFriend->friendCount++;
		}
#ifdef DEBUG
		else
			printf("User does not exist!\n");
#endif
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

//Returns 0 if the user matches, 1 if it doesn't
int authentifyUser(cp username, ui hashpassword, TINFO *userbase)
{
	TUSER *actualUser = findUserByName(username, userbase);
	return (strcmp(username, actualUser->userName) == 0 && 
			hashpassword == actualUser->hashedPassword) ? 0: 1;
}

int checkIfFriends(TUSER *user1, TUSER *user2)
{
	int user1friend = 0;
	int user2friend = 0;
	for(int i = 0; i < MAX_FRIENDS; i++)
	{
		if(user1->friends[i] == user2)
		{
			user1friend = 1;
		}
		if(user2->friends[i] == user1)
		{
			user2friend = 1;
		}
	}
	return user2friend && user1friend;
}
//Check Sockets, -1 offline, returns1 online 
int checkSocket(TUSER *user)
{
	return (user->socket == -1) ? -1: 1;
}
//returns 0 if sucessful
int changeSocket(TUSER *user, int socket)
{
	user->socket = socket;
	return 0;
}

int saveUser(TUSER *user)
{
	FILE *file = fopen("Users.txt", "a");
	if (file == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	//The format in the file is 
	/*userName | hashedPassword | friendCount | all the friends space |  */
	fprintf(file, "%s|%s|%u|%u|", user->userName, user->name, user->hashedPassword, user->friendCount);
	//now to dynamically save all the friend user has saved
	for(ui i = 0; i < user->friendCount; i++)
	{
		fprintf(file, "%s|", user->friends[i]->userName);
	}
	fprintf(file, " |\n");
	fclose(file);
	return 0;
}

int loadUser(cp textFile, TINFO *userBase)
{
	FILE *file = fopen(textFile, "r");
	char line[512];
	if (file == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	//Load the users based off information
	while (fgets(line, sizeof(line), file)) 
	{
		//printf("%s", line);
		char* _userName = malloc(256*sizeof(char));
		char* _name = malloc(256*sizeof(char));
		ui _hashPassword;
		ui _numFriends;
		//read tolkenized input
		//Get all the inputs to their perspective places
		char *split = strtok(line, "|");
		strcpy(_userName, split);
		split = strtok(NULL, "|");
		strcpy(_name, split);
		split = strtok(NULL, "|");
		_hashPassword = (ui)atoi(split);
		split = strtok(NULL, "|");
		_numFriends = (ui)atoi(split);
		TUSER *user1 = addUser(_userName, _name, _hashPassword, userBase);
		user1->friendCount = _numFriends;
		//printf("%s|\n", findUserByName(_userName,userBase)->userName);
	}
	fclose(file);

	//now that all the users are loaded, its time to add them all as friends
	FILE *file2 = fopen(textFile, "r");
	while(fgets(line, sizeof(line), file2))
	{
		char *split = strtok(line, "|");
		char* _userName = split;
		//printf("Here the name is %s\n", _userName);
		split = strtok(NULL, "|");
		split = strtok(NULL, "|");
		split = strtok(NULL, "|");
		TUSER *temp = findUserByName(_userName, userBase);
		//printf("%u\n", temp->friendCount);
		for(ui i = 0; i < temp->friendCount; i++)
		{
			split = strtok(NULL, "|");
#ifdef DEBUG
			printf("%s \n", split);
			printf("%s \n", "JUSTIN");
#endif
			temp->friends[i] = findUserByName(split, userBase);
			//printf("Here the name is %s\n", temp->friends[i]->userName);
		}
		//now we start looking for friends
		//int position = 0;
	}
	fclose(file2);
	return 0;
}

/*Function: deleteFriend
 * Gets a particular user, finds the username of the person they want
 * unfriended, and unfriends */
int deleteFriend(TUSER *user, cp username)
{
	for(ui i = 0; i<user->friendCount; i++)
	{
		if (strcmp(username, user->friends[i]->userName) == 0)
		{
			user->friends[i] = NULL;
			for(ui x = 0; x < user->friendCount - i; x++)
			{
				user->friends[i+x] = user->friends[i+x+1];
				//sift all the users back
			}
			user->friendCount--;
			return 0;
			//BRINGS THE OTHER FRIENDS BACK 1 step
		}
	}
	return 1;
}
//returns the list of its friends
int getFriends(char* username, TINFO* userbase, cp list)
{
	TUSER *temp = findUserByName(username, userbase);
	if(temp->friendCount > 0)
	{
		strcat(list, temp->friends[0]->userName);
	}
	for(ui i = 1; i < temp->friendCount; i++)
	{
		strcat(list, "/");
		printf("in getFirnesd, the username is %s \n", temp->friends[i]->userName);
		strcat(list, temp->friends[i]->userName);
	}
#ifdef DEBUG
	printf("in getFriends, the list is %s \n", list);
#endif
	return 0;
}
#ifdef DEBUG
int main()
{
	/*creating a user */
	TINFO *dataBase = createTINFO();
/*	addUser("Justindlee","Justin", 1234, dataBase);
	addUser("BoostedGorilla","asdf", 1234, dataBase);

	printf("I was able to find %s \n", findUserByName("Justindlee", dataBase)->userName);

	printf("We authentify the user with the password 123,"" result is %d \n", 
			authentifyUser("Justindlee", 1234, dataBase));

	addFriend("BoostedGorilla", findUserByName("Justindlee", dataBase), dataBase);
	printf("%d\n", checkIfFriends(findUserByName("Justindlee", dataBase), 
				findUserByName("BoostedGorilla", dataBase)));
*/
//	saveUser(findUserByName("Justindlee", dataBase));
//	printf("saved successfully!\n");

	loadUser("Users.txt", dataBase);
	printf("loaded successfully?\n");
	//printf("%s\n", dataBase->Users[0]->userName);
	printf("can we see ADMIN: %s \n", findUserByName("ADMIN", dataBase)->userName);
	printf("can we see USER: %s \n", findUserByName("USER", dataBase)->userName);
	printf("can we see USER: %s \n", findUserByName("ADMIN", dataBase)->friends[0]->userName);
	printf("can we see JUSTIN: %s \n", findUserByName("JUSTIN", dataBase)->userName);
	printf("loaded successfully! are ADMIN and USER friends? %d\n", 
			checkIfFriends(findUserByName("ADMIN", dataBase), findUserByName("USER", dataBase)));
	char friends[500];
	getFriends("ADMIN", dataBase, friends);
//	printf("frineds are %s \n", friends);

	deleteTINFO(dataBase);

	printf("Finished running!\n");

	return 0;
}
#endif
