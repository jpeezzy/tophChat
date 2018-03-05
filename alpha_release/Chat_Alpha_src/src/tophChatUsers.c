#include "tophChatUsers.h"
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

TUSER *addUser(cp _userName, cp _name, 
		ui _hashedPassword)
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
	if(temp->first == NULL)
	{
		temp ->prev = NULL;
		temp ->first = temp;
		temp ->last = temp;
	}
	else
	{
		temp->prev = temp->last;
	}
	temp ->next = NULL;
	//Increment Total user count
	numOfUsers++;
	return temp;
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



/*Function: addFriend
 * returns: nameOfFriend
 * input: friend's username
 * */
int addFriend(cp userName, TUSER *user)
{
	//Check if user exists
	TUSER *temp = user->first;
	temp->friendCount++;
	for(int i = 0; i < numOfUsers; i++)
	{
		if(strcmp(userName, temp->userName) == 0)
		{
			//add user to friendList
			user->friends[temp->friendCount] = temp;
		}
		temp = temp->next;
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
