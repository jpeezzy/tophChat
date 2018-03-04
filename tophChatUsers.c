#include "tophChatUsers.h"
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h> 

TUSER *addUser(string _userName, string _name, 
		ui _hashedPassword)
{
	/*Making sure inputs set in are correct*/
	TUSER *temp = malloc(sizeof(TUSER));
	if(!temp)
	{
		perror("NOT ENOUGH REAM");
	}
	//Making sure the user sends in proper inputs
	assert(_userName);
	assert(_name);
	
	temp->userName = _userName;
	temp->name = _name;
	temp->hashedPassword = _hashedPassword;
	temp->hashID = hashID(_userName);
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
ui hashID(string userName)
{
	return ((ui) atoi(userName)) * 53 
		- (ui)(atoi(userName)) % 73;
}



/*Function: addFriend
 * returns: nameOfFriend
 * input: friend's username
 * */



