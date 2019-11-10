//Raymond Waidmann
//TigerHacks 2019
//11/9/2019

//Implementation of a war card game!!
//YouTube Instructions: https://www.youtube.com/watch?v=23QQ1Hz2-jY
//This implementation uses a triple pointer 'war' that points to an array of list nodes, each list node has a head and tail
//	dummy node that holds the linked list hand in the middle. 
//NOTE: In the event that there is a tie, discard the next card and play the following card; if you run out of cards you're out, if there is a tie again
//	the lower seeded player gets the win (it would've been near impossible to error check recursively with dynamic allocation) :(

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 50 //size of the deck of cards

typedef struct nodestruct {    
	int item;    
	struct nodestruct *next;
	} Node;
	
typedef struct {
    Node *head, *tail;
	} List;
	
int GetPlayers();
int GetCards(int);
int initGame(int, List ***, char *);
int runGame(int, List ***);

int main (void)
{
	srand(time(NULL)); // for random integer generator  
	int players = GetPlayers();
	List ** war = NULL;
	
	int error = initGame(players, &war, "deck.txt");
	if (error == -1) printf("\n\n*****FAILED MALLOC*****\n\n");
	if (error == -2) printf("\n\n*****UNSUCCESSFUL FILE OPENING*****\n\n");
	
	error = runGame(players, &war);
	if (error == -1) printf("\n\n*****FAILED MALLOC*****\n\n");
	
	free(war);
	printf("\nThanks for coming to TigerHacks 2019!!!\n\n");
	return(0);
}
	
/*GetPlayers: O(1);
	This function prompts the user to enter an integer number of players to play the game and returns 
	the number of players back to main
*/
int GetPlayers()
{
	int a = 0;
	printf("\n***** ART OF WAR *****\n\nEnter how many players: ");
	scanf("%d", &a);
	while (a < 2 || a >10)
	{
		printf("Invalid number of players (2-10); Enter how many players: ");
		scanf("%d", &a);
	}
	return(a);
}

/*GetCards: O(1);
	This function finds the appropriate number of cards to be used given the user entered amount of players
	and returns the number of cards back to main
*/
int GetCards(int a)
{
	return (MAX - (MAX % a)); //total number of cards in the deck
}

/*initGame: O(N) if success, O(N^3) if failure;
	Malloc an array of Lists
	Malloc a dummy node for the head and tail of each list
	Open the file containing the deck and store it in a malloced integer array
	Randomly select cards from the deck and deal them to each player making sure each has an even amount of cards
	Print the starting hands for each player to the screen
	
	Return Codes:
		 0: successful initialization
		-1: failed malloc at one point or another
		-2: failed file opening
*/
int initGame(int plyrs, List *** w, char * filename)
{
	int crds = GetCards(plyrs);
	int crds2 = crds;
	int crdspp = crds/plyrs;
	*w = malloc(plyrs * sizeof(List*)); //an array that will hold lists, return (0) if unsuccessful malloc of the array
	if (*w == NULL) return (-1);

	for(int i = 0; i < plyrs; i++)
	{
		((*w)[i]) = malloc(sizeof(List)); //mallocing each list in the List array
		if (((*w)[i]) == NULL) //if failed malloc, free all previously allocated memory
		{
			for (int j = 0; j < i; j++)
			{
				free(((*w)[j])->head);
				free(((*w)[j])->tail);
				free((*w)[j]);
			}
			free(*w); //freeing the list array
			*w = NULL;
			return(-1);
		}
		
		((*w)[i])->head = malloc(sizeof(Node)); //malloc of head dummy node
		if (((*w)[i])->head == NULL) //failed malloc
		{
			for (int j = 0; j < i; j++)
			{
				free(((*w)[j])->head);
				free(((*w)[j])->tail);
				free((*w)[j]);
			}
			free((*w)[i]); //freeing the list node of the most recent malloc
			free(*w);
			*w = NULL;
			return(-1);
		}
		((*w)[i])->head->item = -100;
		
		((*w)[i])->tail = malloc(sizeof(Node)); //malloc of a tail dummy
		if (((*w)[i])->tail == NULL) //failed malloc
		{
			for (int j = 0; j < i; j++)
			{
				free(((*w)[j])->head);
				free(((*w)[j])->tail);
				free((*w)[j]);
			}
			free(((*w)[i])->head); //freeing the head node pointed to by the most recent list node
			free((*w)[i]); //freeing the list node of the most recent malloc
			free(*w);
			*w = NULL;
			return(-1);
		}
		((*w)[i])->tail->item = -100;
		((*w)[i])->head->next = ((*w)[i])->tail;
		((*w)[i])->tail->next = NULL;
	}

	FILE* fptr = NULL;
	fptr = fopen(filename, "r"); //opening the random data file for reading, return (-2) if unsuccessful
	if (fptr == NULL) return (-2);
	
	int * data; 
	data = malloc(sizeof(int) * crds); //an array that holds all of the data from the input file (the deck of cards)
	if (data == NULL) return (-1);
	for (int i = 0; i < crds; i++) fscanf(fptr, "%d\n", &data[i]);
	
	for (int i = 0; i < plyrs; i++) //loop for each player
	{
		for (int j = 0; j < crdspp; j++) //loop for the number of cards per player
		{
			int index = (rand()%crds2); //random index
			int inp = data[index]; //random int 
			for (int k = index; k < crds2; k++) data[k] = data[k+1]; //shifting the array over to replace the removed int
			crds2--; //current size of data array
			
			Node* newNode = malloc(sizeof(Node));
			if(newNode == NULL) //if unsuccessful malloc
			{
				for (int m = 0; m < i; m++)
				{
					while (((*w)[m])->head->next->next != NULL) //freeing all of the cards in each persons hand
					{
						Node * temp = ((*w)[m])->head->next;
						((*w)[m])->head->next = ((*w)[m])->head->next->next;
						free(temp);
					}
					free(((*w)[m])->head); //freeing the dummies
					free(((*w)[m])->tail);
					free((*w)[m]); //freeing the list
				}
				free(*w);
				*w = NULL;
				return (-1); 
			}
			newNode->item = inp; 
			newNode->next =  ((*w)[i])->head->next; //dealing the cards into each players linked list
			((*w)[i])->head->next = newNode; 
		}
	}
	
	printf("\nStarting Hands:");
	for (int i = 0; i < plyrs; i++) //printing off the starting hands for each player
	{
		Node * temp = ((*w)[i])->head->next;
		printf("\nPlayer %d: ", (i+1));
		while (temp->next != NULL) 
		{
			printf("%d ", temp->item);
			temp = temp->next;
		}
	}
	free(data); //freeing the array that stored the input data from the file
	return (0);
}

/*runGame: O(N);
	Malloc an array that stores the players currently still in the game
	Find the player that has the winning card for that turn
		- if two players have equal cards, look at the next next card (winner takes all) ***********
	Place the used cards at the bottom of the winners hand
	Print off the hand of each player
	Anytime one or more players is eliminated, stop automation, display users have been eliminated, adjust currentplyrs array, and prompt user to resume program
	Free all allocated memory once a winner has been determined. 
		
		Return Codes:
			-1: failed malloc
			 0: success
*/
int runGame(int plyrs, List *** w)
{
	int * currentplyrs; 
	currentplyrs = malloc(sizeof(int) * (plyrs + 1)); //an array that holds all of the players currently in the game
	if (currentplyrs == NULL) return (-1);
	for (int i = 0; i < plyrs; i++) currentplyrs[i] = i;
	currentplyrs[-1] = plyrs; //current size of the array stored in front of the array
		
	int sentinel = 1;
	int turn = 1;
	int sentinel2 = 1;
	int maxs[5]; //4 slots for identical values, 5th spot for current number of indeces in array
	maxs[4] = 0;

	while (sentinel != 0)
	{
		int max = -1; //max value (winning card)
		int maxplyr = 0; //player with the winning card
		for (int i = 0; i < 4; i++) maxs[i] = -1;
		int z = 0; //index for the array that stores the cards in the event of a tie
		
		for(int i = 0; i < currentplyrs[-1]; i++) //iterating through each players hand and finding the player who has the winning card for that turn
		{
			if (((*w)[(currentplyrs[i])])->head->next->item == max) //if there is more than one maximum, store the indexes in the maxs array
			{
				maxs[z] = (currentplyrs[i]);
				z++;
				maxs[4]++;
			}
			
			if (((*w)[(currentplyrs[i])])->head->next->item > max) //if a new max is found
			{
				max = ((*w)[(currentplyrs[i])])->head->next->item; //max value is the new max card
				maxplyr = (currentplyrs[i]); //array index of the person with the winning card
				maxs[0] = maxplyr;
				for (int i = 1; i < 4; i++) maxs[i] = -1; //reinitializing the maxs array
				z = 1;
				maxs[4] = 1;
			}
			
		}	
			
			
		if (maxs[4] != 1) //while there are multiple max values
		{
			Node * tempwin = ((*w)[0])->tail; //for the time being, store the "discard" at the end of the first hand
			for(int i = 0; i < maxs[4]; i++) //inserting the cards from that round into the temporary stack
			{
				Node * temp = ((*w)[(maxs[i])])->head->next; //temp is the card played in that round
				((*w)[(maxs[i])])->head->next = ((*w)[(maxs[i])])->head->next->next; //pointing dummy to card 2 in everyones hand
				temp->next = NULL; //pointing the discarded cards to NULL
				tempwin->next = temp; //pointing the end of the old hand to the newly inserted card
				tempwin = tempwin->next; //moving tempwin to the new end of the hand
				
				temp = ((*w)[(maxs[i])])->head->next; //another dequeue to the temporary stack (burn card)
				if (temp != ((*w)[(maxs[i])])->tail) //if the person only had one card, they're out (this loop doesn't execute)
				{
					((*w)[(maxs[i])])->head->next = ((*w)[(maxs[i])])->head->next->next; //removing a second card 
					temp->next = NULL; 
					tempwin->next = temp; 
					tempwin = tempwin->next; 
				}
			}
				
			for(int i = 0; i < maxs[4]; i++) //iterating through each prelinimary winners players new hand
			{
				if (((*w)[(maxs[i])])->head->next == ((*w)[(maxs[i])])->tail) continue; //if player only had 1 or 2 cards
						
				if (((*w)[(maxs[i])])->head->next->item > max) //if a new max is found (since this is not >=, in the event of a double tie the earliest player gets the win)
					{
						max = ((*w)[(maxs[i])])->head->next->item; //max value is the new max card
						maxplyr = (maxs[i]); //array index of the person with the winning card
					}
			}
			
			tempwin = ((*w)[maxplyr])->head->next; //tempwin is the winning card
			while (tempwin->next != ((*w)[maxplyr])->tail) tempwin = tempwin->next; //tempwin points to the last card in the winning persons hand
			tempwin->next = ((*w)[0])->tail->next; //pushing the temp stack to the winning hand
			((*w)[0])->tail->next = NULL; //pointing tail to NULL
			while (tempwin->next != NULL) tempwin = tempwin->next;
			tempwin->next = ((*w)[maxplyr])->tail; //making sure the tail is at the end of the list
			((*w)[maxplyr])->tail->next = NULL;
			printf("\nThere was a tie!!! Winner of the drawdown is Player %d", (maxplyr + 1));
		}
			
		Node * tempwin = ((*w)[maxplyr])->head->next; 
		while (tempwin->next->next != NULL) tempwin = tempwin->next; //tempwin points to the last card in the winning persons hand
		
		for(int i = 0; i < currentplyrs[-1]; i++) //inserting the cards from that round into the winners hand
		{
			Node * temp = ((*w)[(currentplyrs[i])])->head->next; //temp is the card played in that round
			if (temp == ((*w)[(currentplyrs[i])])->tail) continue;
			((*w)[(currentplyrs[i])])->head->next = ((*w)[(currentplyrs[i])])->head->next->next; //pointing dummy to card 2 in everyones hand
			temp->next = tempwin->next; //pointing the discarded cards to the tail pointer in the winners hand
			tempwin->next = temp; //pointing the end of the old hand to the newly inserted card
			tempwin = tempwin->next; //moving tempwin to the new end of the hand
		}
		
		printf("\n\nTurn %d; Winning player = %d", turn, (maxplyr+1)); //printing the current turn and the winning player of that turn
		turn++;
		
		for (int i = 0; i < currentplyrs[-1]; i++) //printing off the hands for each player
		{
			Node * temp = ((*w)[(currentplyrs[i])])->head->next;
			printf("\nPlayer %d: ", ((currentplyrs[i])+1));
			while (temp != ((*w)[(currentplyrs[i])])->tail) 
			{
				printf("%d ", temp->item);
				temp = temp->next;
			}
			
		}

		int tally = 0; //tally of number of eliminated players
		for (int i = 0; i < currentplyrs[-1]; i++) //counting the number of players that have been eliminated on a given turn
		{
			if (((*w)[(currentplyrs[i])])->head->next->next == NULL) //If a specific player is out of cards
			{
				printf("\nPlayer %d Eliminated!!!", (currentplyrs[i]+1)); 
				currentplyrs[i] = -1;
				tally++;
				sentinel2 = 0; //sentinel 2 stops automation so user knows a player has been eliminated
			}
		}
		
		int index = 0; //index of the new array containing the players still in the game
		for (int i = 0; i < currentplyrs[-1]; i++) //this array removed all eliminated players from the currentplyrs array
		{
			if (currentplyrs[i] == -1) continue; //skipping the eliminated players
			currentplyrs[index] = currentplyrs[i]; //if there are 5 players, and player 3 is eliminated; the new array is: 0, 1, 3, 4
			index++;
		}
		currentplyrs[-1] -= tally; //adjusting the size of the array
		
		if (sentinel2 == 0) //if someone has been elinimated on that turn
		{
			if (currentplyrs[-1] == 1) //if there's only one person left
			{
				printf("\nPlayer %d is the Winner!!!", (currentplyrs[0]+1));
				sentinel = 0;
			}
			
			else //2 or more people left in the game
			{
				printf("\nPress 1 to continue: "); //prompting user to resume automation
				scanf("%d", &sentinel);
				sentinel2 = 1;
			}
		}
	}
	
	for (int i = 0; i < plyrs; i++) //freeing all malloced memory
	{
		while (((*w)[i])->head->next->next != NULL) //freeing all of the cards in each persons hand (this loop only executed on the winners hand)
		{
			Node * temp = ((*w)[i])->head->next;
			((*w)[i])->head->next = ((*w)[i])->head->next->next;
			free(temp);
		}
		free(((*w)[i])->head); //freeing the dummies
		free(((*w)[i])->tail);
		free((*w)[i]); //freeing the list
	}
	free(*w); //freeing the array
	*w = NULL;	
	free(currentplyrs); //freeing the currentplyrs array
	return(0);
}