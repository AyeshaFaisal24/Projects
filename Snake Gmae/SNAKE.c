#include <stdio.h> //used this to print stuff
#include <stdlib.h> //used to get character from input by getch()
#include <conio.h> //used kbhit() to control snake movement(Important)
#include <ctype.h> //used to make sure the character is lowercase 
#include <windows.h> //sleep() used to allow user to be able to react to snake movement, used Beep() for sounds, cursor control for smoother printing

#define HEIGHT 20
#define WIDTH 60
#define FILENAME "Highscores.txt"
#define TOP10 10

int snaketailx[100], snaketaily[100]; //snake cords array (snake can have a maximum length 100)
int snaketaillen; //stores snake's current length
// Score and flags
int gameover, key, score, startflag = 0, count, quit;
//coords of snakehead and food
int x, y, foodx, foody;

typedef struct {
	char name[50];
	int score;
} highscore;//datatype for highscore
highscore scores[TOP10];//array to save highscores

//function prototypes for c99 or later versions of C (case of implicit declaration)
int start();
int menu(int choice);
int instructions();
int displaytop10(highscore scores[], int count);
int loadhighscores(highscore scores[]);
void savehighscores(highscore scores[], int count);
void updatehighscores(highscore scores[], int *count, char name[], int score);
void gameoversound();
void eatsound();
void init();
void draw();
void rules();
void input();


int start() {
	int menulocate;  //Tracks the current menu option (1 to 4)
	char menukey;    //Holds the user's keyboard input

	menulocate = 1;      //Initializes the cursor position to the first menu option
	menu(menulocate);    //start cursor in menu at position 1
	while (1) {
		menukey = getch();     //Captures the key pressed by the user
		if (menukey == 80) {           //Moves the cursor down to the next menu option
			menulocate++;
			if (menulocate > 4) {
				menulocate = 1;
			}
			menu(menulocate);
		}
		else if (menukey == 72) {    //cursor up
			menulocate--;
			if (menulocate < 1) {
				menulocate = 4;
			}
			menu(menulocate);
		}
		if (menukey == 27) {       //escape pressed
			system("cls");
			printf("Exited Game");
			return 0;
		}
		if (menukey == 13 && menulocate == 1){//enter pressed on start game
			gameover = 0;       //to prepare for a new game
			return 1;              //to indicate the game should start
		}
		if (menukey == 13 && menulocate == 2){   //enter pressed on highscores
			system("cls");
			displaytop10(scores,count);      //calling function to show the leaderboard
			menu(menulocate);                //calling function to redisplay the menu afterward
		}
		if (menukey == 13 && menulocate == 3){   //enter pressed on instructions
			system("cls");
			instructions();
			menu(menulocate);
		}
		if (menukey == 13 && menulocate == 4){//enter pressed on quit game
			return 0;
		}
	}
}
int menu(int choice) {
	switch (choice) {
		case 1:
			system("cls");
			printf("\t\tSNAKE GAME\n");
			printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
			printf("\nSTART GAME  <");
			printf("\nTOP 10 LEADERBOARD");
			printf("\nINSTRUCTIONS");
			printf("\nQUIT GAME");
			printf("\n\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
			printf("\n\nUP and DOWN arrow keys to scroll");
			printf("\nENTER key to select\n");
			break;

		case 2:
			system("cls");
			printf("\t\tSNAKE GAME\n");
			printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
			printf("\nSTART GAME");
			printf("\nTOP 10 LEADERBOARD  <");
			printf("\nINSTRUCTIONS");
			printf("\nQUIT GAME");
			printf("\n\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
			printf("\n\nUP and DOWN arrow keys to scroll");
			printf("\nENTER key to select\n");
			break;

		case 3:
			system("cls");
			printf("\t\tSNAKE GAME\n");
			printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
			printf("\nSTART GAME");
			printf("\nTOP 10 LEADERBOARD");
			printf("\nINSTRUCTIONS  <");
			printf("\nQUIT GAME");
			printf("\n\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
			printf("\n\nUP and DOWN arrow keys to scroll");
			printf("\nENTER key to select\n");
			break;

		case 4:
			system("cls");
			printf("\t\tSNAKE GAME\n");
			printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
			printf("\nSTART GAME");
			printf("\nTOP 10 LEADERBOARD");
			printf("\nINSTRUCTIONS");
			printf("\nQUIT GAME  <");
			printf("\n\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
			printf("\n\nUP and DOWN arrow keys to scroll");
			printf("\nENTER key to select\n");
			break;

		default:
			system("cls");
			printf("\nError");
	}
}
int instructions() {
    printf("\n=-=-=-=-=-=-=- INSTRUCTIONS =-=-=-=-=-=-=-=\n\n");
    printf("1. Use W, A, S, D keys to control the snake's movement.\n");
    printf("2. Eat food to grow the snake.\n");
    printf("3. Avoid colliding with walls or the snake's body.\n");
    printf("4. The game ends if the snake collides with itself or the wall.\n");
    printf("\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");
    printf("Press any key to return to the menu...");
    getch();
    system("cls");
}
int displaytop10(highscore scores[], int count) {
	int i;
	printf("\n=-=-=-=-=-= Top 10 High Scores -=-=-=-=-=-=\n\n");
	FILE *file = fopen(FILENAME, "r");//file opened
	if (!file) {
		printf("No Highscores available.\n");
	}
	for ( i = 0; i < count; i++) {
		printf("%d. %s - %d\n", i + 1, scores[i].name, scores[i].score);
	}
	printf("\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
	printf("Press any key to return to the menu...");
	getch();
	system("cls");
}
//loads highscores from file
int loadhighscores(highscore scores[]) {
	FILE *file = fopen(FILENAME, "r");//file opened
	if (!file) {
		printf("Error: Could not open file to count scores.\n");
		return 0;
	}
	count = 0;
	while (fscanf(file, "%49s %d", scores[count].name, &scores[count].score) == 2 && count < TOP10) {//counts scores saved , == 2 check ensures both the name and score are successfully read
		count++;
	}
	fclose(file);
	return count;  //Returns the total number of valid scores successfully loaded (count).
}
//saves highscores to file
void savehighscores(highscore scores[], int count) {
	int i;
	FILE *file = fopen(FILENAME, "w");
	if (!file) {
		printf("Error: Could not open file to save scores.\n");
		return;
	}
	for ( i = 0; i < count; i++) {
		fprintf(file, "%s %d\n", scores[i].name, scores[i].score);
	}
	fclose(file);
}
//updates top10 list
void updatehighscores(highscore scores[], int *count, char name[], int score) {
	highscore newscore;
	strncpy(newscore.name, name, sizeof(newscore.name));  //copy name to the newscore struct
	newscore.score = score;   //copy score to the newscore struct
	int i;
	for (i = *count; i > 0 && scores[i - 1].score < score; i--) { //sort the score from bottom to top, condition is current playthrough score greater than the one at i position in the list
		if (i < TOP10) {
			scores[i] = scores[i - 1];
		}
	}
	if (i < TOP10) { //if the total number of values in file is less than 10 even after adding the newscore, increase the count of value
		scores[i] = newscore;
		if (*count < TOP10) {
			(*count)++;
		}
	}
}
//plays a sound when game ends
void gameoversound() {
    Beep(440, 250); // A note: 440 Hz, 300 ms
    Beep(349, 250); // F note: 349 Hz, 300 ms
    Beep(330, 250); // E note: 330 Hz, 300 ms
}
//plays a sound when snake eats food
void eatsound() {
	Beep(750, 150); // 750 Hz for 200 ms
}
//intializes coords snake and food
void init() {
	// Flag to signal the gameover
	quit = 0;//quit-game-when-playing flag, if (1) doesnt ask to enter name or stores highscore
	key = 0;//direction = none(snake stays there)
	gameover = 0;//game not over
	snaketaillen = 0;//reset/set snake length
	//initial coords of snake = (center)
	x = WIDTH / 2;
	y = HEIGHT / 2;
	//initial coords of food(checks if the food is on snake body or not)
	int valid = 0;//validity of spawn
	while (!valid) {
		foodx = rand() % WIDTH;
		foody = rand() % HEIGHT;
		valid = 1;
		int i;
		for ( i = 0; i < snaketaillen; i++) {
			if (snaketailx[i] == foodx && snaketaily[i] == foody) {
				valid = 0; //food spawned at snake body or head therefore not printed
				break;
			}
		}
	}
	//score initialized
	score = 0;
}
//draw the bounds, snake and food
void draw() {
	int i, j, k;
	COORD cursorPosition = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);//clearing screen by setting cursor to a position to reduce screen flicker, instead of system("cls")
	//top of game
	for (i = 0; i < WIDTH + 2; i++)    //The +2 accounts for the left and right side walls
		printf("-");
	printf("\n");
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j <= WIDTH; j++) {
			//sidewall
			if (j == 0 || j == WIDTH) {
				printf("|");
			}
			//snakehead
			if (i == y && j == x) {
				printf("@");
			}
			//snake food
			else if (i == foody && j == foodx) {
				printf("*");
			} else {
				//last tail part removal
				int tailcheck = 0;
				for (k = 0; k < snaketaillen; k++) {
					if (snaketailx[k] == j && snaketaily[k] == i) {
						printf("o");
						tailcheck = 1;
					}
				}
				if (!tailcheck) {
					printf(" ");//at end of the tail leave a space
				}
			}
		}
		printf("\n");
	}
	//bottom wall
	for (i = 0; i < WIDTH + 2; i++) {
		printf("-");
	}
	printf("\n");
	//print score and instructions
	printf("score = %d\n", score);
	printf("Press W, A, S, D to move.\n");
	printf("Press X to quit the game.");
}
//input function
void input() {
	//direction stored in key
	if (kbhit()) {
		switch (tolower(getch())) {
		case 'a':
			if (key != 2)//if its not going to right already
				key = 1;//go left
			break;
		case 'd':
			if (key != 1)//if its not going to left already
				key = 2;//go right
			break;
		case 'w':
			if (key != 4)//if its not going to down already
				key = 3;//go up
			break;
		case 's':
			if (key != 3)//if its not going to up already
				key = 4;//go down
			break;
		case 'x':
			gameover = 1;
			gameoversound();
			quit = 1;
		}
	}
}
//rule checks(collision and eating) and score
void rules() {
	int i;
	//updating the coords for snakehead
	int prevx = snaketailx[0];//previous position of head
	int prevy = snaketaily[0];
	snaketailx[0] = x;//current snake head is now at this x
	snaketaily[0] = y;//current snake head is now at this y
	int tempx, tempy;
	//moving snake body one part at a time to the space of it predecessor
	for (i = 1; i < snaketaillen; i++) {
		tempx = snaketailx[i];
		tempy = snaketaily[i];
		snaketailx[i] = prevx;
		snaketaily[i] = prevy;
		prevx = tempx;
		prevy = tempy;
	}
	//direction change
	switch (key) {
	case 1:
		x--;//left
		break;
	case 2:
		x++;//right
		break;
	case 3:
		y--;//up
		break;
	case 4:
		y++;//down
		break;
	default:
		break;
	}
	//wall hit check
	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
		gameover = 1;
		gameoversound();
	}
	//body collision check
	for (i = 0; i < snaketaillen; i++) {
		if (snaketailx[i] == x && snaketaily[i] == y) {
			gameover = 1;
			gameoversound();
		}
	}
	//updates food when consumed(checks if the new food is on snake body or not
	if (x == foodx && y == foody) {
	int valid = 0;
	while (!valid) {
		foodx = rand() % WIDTH;
		foody = rand() % HEIGHT;
		valid = 1;
		int i;
		for ( i = 0; i < snaketaillen; i++) {
			if (snaketailx[i] == foodx && snaketaily[i] == foody) {
				valid = 0; //food spawned at snake body or head
				break;
			}
		}
	}
		score += 10;
		snaketaillen++;//snake growing
		eatsound();//sound when eat
	}
}
int main() {
	count = loadhighscores(scores);//checking how many values in highscore file
	//menu loop
	while(start()==1){
		init();//start variables
		//game loop
		while (!gameover) {
			draw();
			input();
			rules();
			Sleep(30); //wait some time to give user time to react
		}
		char name[50];
		if (!quit) {//only stores info when game is not quit while playing
			printf("\n***Game Over!*** \nEnter your name: ");
			scanf("%49s", name);
			updatehighscores(scores, &count, name, score);//top10 list update
			savehighscores(scores, count);//save list to file
		}
	}
	return 0;
}