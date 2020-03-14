/* Solution to comp10002 Assignment 2, 2019 semester 2.

   Authorship Declaration:

   (1) I certify that the program contained in this submission is completely
   my own individual work, except where explicitly noted by comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students,
   or by non-students as a result of request, solicitation, or payment,
   may not be submitted for assessment in this subject.  I understand that
   submitting for assessment work developed by or in collaboration with
   other students or non-students constitutes Academic Misconduct, and
   may be penalized by mark deductions, or by other penalties determined
   via the University of Melbourne Academic Honesty Policy, as described
   at https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will
   I do so until after the marks are released. I understand that providing
   my work to other students, regardless of my intention or any undertakings
   made to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring
   service, or drawing the attention of others to such services and code
   that may have been made available via such a service, may be regarded
   as Student General Misconduct (interfering with the teaching activities
   of the University and/or inciting others to commit Academic Misconduct).
   I understand that an allegation of Student General Misconduct may arise
   regardless of whether or not I personally make use of such solutions
   or sought benefit from such actions.

   Signed by: Cooper Marshall
   Dated:     7/10/2019

*/

//Test for stepping off the grid
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define EMPTY -1
/* Struct of coordinate pairs */
typedef struct {
	int row;
	int column;
} coords_t;
/*Struct for the blocks */
typedef struct {
	coords_t placement;
} positions_t;
/*Struct for the path nodes */
typedef struct node node_t;
struct node {
	coords_t coords;
	node_t *next;
}; 
/* Struct containing the initial and goal cell coordinates */
typedef struct {
	coords_t initial;
	coords_t goal;
} objective_t;
/* Struct for the queue of points to flood the grid */
typedef struct {
	coords_t coords;
	int num;
} gridpoints_t;


coords_t size();
objective_t objectivecells();
coords_t pointinterpreter(char initial[50], int length);
positions_t * blockcreation(coords_t gridsize, int size, positions_t blocks[],
	int *block_size, int stage);
node_t *pathing();
void printroute(node_t *current);
int pathcheck(coords_t gridsize, node_t *first, positions_t blocks[],
	int blockcount, objective_t objectives, int holdoutput);
node_t *create_node(coords_t coordinates, node_t* previous);
int objectives_check(node_t *first, objective_t obj);
int illegalmove_check(coords_t gridsize, node_t *first);
int block_check(node_t *first, positions_t blocks[], int block_count,
	int holdoutput);
node_t *find_last(node_t* first);
int printandrepair(int status, coords_t gridsize, node_t *first,
	positions_t blocks[], objective_t obj, int block_count, int holdout,
	int stage);
gridpoints_t *grid_create(coords_t gridsize, gridpoints_t gridpoints[], 
	int** blockgrid, int** grid, int *size, int *current, node_t *contination);
int checkvacency(int ** blockgrid, gridpoints_t point, coords_t gridsize,
	int row, int column, int **grids);
int checkpath(node_t *contination, gridpoints_t point);
void updatepath(node_t *breakpoint, coords_t pathspot, int counter, 
	int ** grids, coords_t gridsize);
void printgrid(coords_t gridsize, node_t *first, positions_t blocks[],
	objective_t obj, int block_count);
void printinfo(coords_t gridsize, int block_count, objective_t objectives);
int validateblock(coords_t gridsize, int x, int y);
void stage2(coords_t gridsize, node_t *firstl, positions_t blocks[],
	int block_count, objective_t objectives, int out, int status, int moved,
	int size);
/* Main function - controls calling other functions */
int
main(int argc, char *argv[]) {
	int moved=0;
	int out=0;
	printf("==STAGE 0=======================================\n");
	coords_t gridsize = size(); /* Reads in the gridsize */
	objective_t objectives = objectivecells(); /* Reads in the objectives */
	int size = 2;
	int block_count;
	/* Create blocks */
	positions_t *blocks = (positions_t *)malloc(size*sizeof(positions_t));
	blocks = blockcreation(gridsize ,size, blocks, &block_count, 1);
	assert(blocks);
	node_t *firstl;
	printinfo(gridsize, block_count, objectives);
	/* create path */
	firstl = pathing();
	int status;
	/* check path */
	status = pathcheck(gridsize, firstl, blocks, block_count, objectives, 0);
	printf("==STAGE 1=======================================\n");
	/* print and repair */
	printandrepair(status, gridsize, firstl, blocks, objectives ,block_count, 0,
		1);
	blocks = blockcreation(gridsize, size, blocks, &block_count, 2);
	/* Start stage 2 if status is 4 or 5 */
	if (status >3) {
		stage2(gridsize, firstl, blocks, block_count, objectives, out, status,
			moved, size);
	}
	printf("================================================\n");
	free(blocks);
	return 1;
}

/* continue through the rest of the different block layouts  */
void stage2(coords_t gridsize, node_t *firstl, positions_t blocks[],
	int block_count, objective_t objectives, int out, int status, int moved, 
	int size) {

		if (blocks!=NULL) {
			printf("==STAGE 2=======================================\n");		
		}
		/* blocks will be null when the program runs out of input */
		while(blocks!=NULL) {
			/* Try to fix the path */ 
			status = pathcheck(gridsize, firstl, blocks, block_count, 
				objectives, 1);
			moved = printandrepair(status, gridsize, firstl, blocks, objectives, 
				block_count, out, 2);
			out = 1;
			status = pathcheck(gridsize, firstl, blocks, block_count, 
				objectives, 1);
			if (status != 4 || moved == 0) {
				blocks = blockcreation(gridsize,size, blocks, &block_count, 2);
				if(blocks!= NULL) {
					printf("==========================");
					printf("======================\n");
				}
				out = 0;
			}
		}
}

/* Prints out the information in the first stage */
void printinfo(coords_t gridsize, int block_count, objective_t objectives){
	printf("The grid has %d rows and %d columns.\n", gridsize.row,
	gridsize.column);
	printf("The grid has %d block(s).\n", block_count);
	printf("The initial cell in the grid is [%d,%d].\n", 
		objectives.initial.row, objectives.initial.column);
	printf("The goal cell in the grid is [%d,%d].\n",
		objectives.goal.row, objectives.goal.column);
	printf("The proposed route in the grid is: \n");
	
}

/* Reads in the size from stdin */
coords_t size(){	
	coords_t gridsize;
	int rows=0;
	int columns=0;
	scanf("%dx%d", &rows, &columns); /* read in the rows and columns */
  	gridsize.row = rows;
  	gridsize.column = columns;
  	return gridsize; /* return structure with the gridsizes */
}

/* Reads in both the objectives */
objective_t objectivecells() {
	objective_t objectives;
	int x= 0;
	int y = 0;
	/* read and load in the objective cells */
	scanf(" [%d,%d] ", &x, &y); 
	objectives.initial.row = x;
	objectives.initial.column = y;
	scanf(" [%d,%d] ", &x, &y);
	objectives.goal.row = x;
	objectives.goal.column = y;
	return objectives; /* return stuct with initial and goal cells */
}

/* Reads in the blocks for stage0 and stage 2 and stores them in a list */
positions_t *blockcreation(coords_t gridsize, int size, positions_t blocks[],
	int *block_size, int stage) {
	int i=0;
	int x;
	int y;
	char o[10];
	
	/* Control to make sure that there are new blocks for stage 2 */
	if (stage==2) {
		int newstatus; /* Whether to load in a new load of blocks */
		int scancoords;
		newstatus= scanf("%s",o);
		if (newstatus == 1)	 {
			scancoords = scanf(" [%d,%d] ", &x, &y);	
			if (scancoords != 2 || scancoords==EOF) {
				(*block_size) = 0;
				return blocks;	
			}
			(*block_size) = 0;
			if (validateblock(gridsize, x,y)) {
				blocks[i].placement.row = x;
				blocks[i].placement.column = y;
				i++;
			}
		}
		else {
			return NULL;
		}
	}
	
	while (scanf(" [%d,%d] ", &x, &y) == 2) { /* Read the input */
		if (i == size){
			size = size*2;
			blocks = (positions_t *)realloc(blocks, (size)*sizeof(positions_t));
			assert(blocks);
		}
		if (validateblock(gridsize, x,y)) {
			blocks[i].placement.row = x;
			blocks[i].placement.column = y;
			i++;
		}
	}
	(*block_size) = i; /*Set the pointer to i for the counter number of blocks*/
	return blocks; /* return and list of all the blocks */
}

/* Function that checks that the values are within the grid */
int validateblock(coords_t gridsize, int x, int y) {
	if (x > gridsize.row || x < 0 || y > gridsize.column || y <0) {		
		return 0;
	}
	return 1;	
}

/* Reads in the path */
node_t *pathing() {
	node_t *current = NULL;
	node_t *first = NULL;
	short int x= 0;
	short int y=0;
	scanf("%*s");
	
	/* Scans in the paths coordinates */
	while (scanf(" [%hd,%hd]-> ", &x, &y) == 2) { 
		coords_t coordinates;
		coordinates.row = x;
		coordinates.column =y;
		node_t *node = create_node(coordinates, current); /* creates the node */
		if (current != NULL) {		
			current->next = node;
		}
		current = node;
		if (first == NULL) {
			first = current;
		}
	}

	printroute(first);
	return first;
}

/* Prints the route */
void printroute(node_t *current){
	int i=1;
	/* Itterates over the path and prints it */
	while(current != NULL) {
		printf("[%d,%d]", current->coords.row, current->coords.column);
		if (current->next != NULL){
			printf("->");
		}
		else {
			break;
		}
		if ((i%5)==0 && i!=0){
			printf("\n"); /* Print a newline for every 5th set of coords */
		}
		i++;
		current = current->next;
	}
	printf(".\n");
}

/* Validates the path and finds the status of the route */
int pathcheck(coords_t gridsize, node_t *first, positions_t blocks[],
	int blockcount, objective_t objectives, int holdoutput){
	int a;
	if ((a=objectives_check(first, objectives)) != 0) { /*Check objectives */
		return a;
	}
	
	/* Check for illegal moves */
	if ((a =  illegalmove_check(gridsize, first)) != 0) {
		return a;
	}
	
	/* Checks for blocks on the path*/
	if ((a =  block_check(first, blocks, blockcount, holdoutput)) != 0) {	
		return a;
	}
	
	/* Only prints if we want an output*/
	if (!holdoutput){
		printf("The route is valid!\n");
	}
	
	return 5;
}

/* Checks that the goal cell and initial cell are the same as on the path */
int objectives_check(node_t *first, objective_t obj) {
	node_t *last = find_last(first);
	if (obj.initial.row != first->coords.row||
		obj.initial.column != first->coords.column ){
		printf("Initial cell in the route is wrong!\n");
		return 1;
	}
 
	if (obj.goal.row != last->coords.row||
		obj.goal.column != last->coords.column ){
		printf("Goal cell in the route is wrong!\n");
		return 2;
	}
	return 0;		
}

/* Checks for an illegal move */ 
int illegalmove_check(coords_t gridsize, node_t *first) {
	node_t *prev = first;
	node_t *current = first->next;
	while (current != NULL) {
		int xdiff=0; int ydiff=0;
		xdiff = abs(prev->coords.row - current->coords.row);
		ydiff = abs(prev->coords.column - current->coords.column);
		if ((xdiff+ydiff) != 1 || prev->coords.row <0 || prev->coords.column <0
			|| prev->coords.row > gridsize.row ||
			prev->coords.column > gridsize.column){
			printf("There is an illegal move in this route!\n");
			return 3;
		}
		prev = current;
		current = current->next;
	}
	return 0;
}

/* Checks to see whether a block is on the path */
int block_check(node_t *first, positions_t blocks[], int block_count, 
	int holdoutput) {
	int i;
	node_t *current = first;
	while (current != NULL) {
		for (i=0;i<block_count;i++) {
			if (blocks[i].placement.row == current->coords.row &&
				blocks[i].placement.column == current->coords.column){
				if (!holdoutput) {
					printf("There is a block on this route!\n");
				}
				return 4;
			}
		}
		current = current->next;
	}
	return 0;
}

/* Finds the last position in the path */
node_t *find_last(node_t* first) {
	node_t *current = first;
	while(1) {
		if (current->next == NULL) {
			break;
		}
		current = current->next;
	}
	node_t *last = current;	
	return last;	
}
		
/* Creates a new node object that stores the path */
node_t *create_node(coords_t coordinates, node_t* previous) {
	node_t *node = (node_t*)malloc(sizeof(node_t));
	assert(node!=NULL);
	node->coords = coordinates;
	node->next = NULL;
	return node;
}

/* Function that prints the current grid with the path, blocks, initial and goal
cell */
void printgrid(coords_t gridsize, node_t *first, positions_t blocks[],
	objective_t obj, int block_count) {
	int i;
	int k;
	int l;
	int m;
	int j;
	int a = gridsize.column;
	int b = gridsize.row;
	char **grid = (char **)malloc((gridsize.row+1)*sizeof(char*));
	for (i=0; i<gridsize.row+1;i++) {
		grid[i] = (char *)malloc(gridsize.column*sizeof(char));
	}
	for(k=0; k<gridsize.row+1; k++){
		for(l=0; l<gridsize.column+1; l++) {
			grid[k][l] = ' ';	
		}
	}

	for (i=0; i<a; i++){ /* setup the column of the output */
		char x;
		x = i%10 + '0';
		grid[0][i+1] = x;	
	}	
	for (j=0; j<b; j++){ /* setup for the row of the output */
		char x;
		x = j%10 + '0';
		grid[j+1][0] = x;		
	}
	node_t *current = first;
	gridsize.column = a;
	gridsize.row = b;
	
	/* Assigns the rows */
	while(current != NULL) {
		grid[current->coords.row+1][current->coords.column+1] = '*';	
		current = current->next;	
	}
	
	/* Places the blocks */
	for(m=0;m<block_count; m++) {
		int rowval = (blocks[m].placement.row) + 1;
		int colval = (blocks[m].placement.column) + 1;
		grid[rowval][colval] = '#';
	}
	
	/* Places the initial cell */
	grid[obj.initial.row+1][obj.initial.column+1] = 'I';
	/* Places the goal cell */
	grid[obj.goal.row+1][obj.goal.column+1] = 'G';
	for(k=0; k<gridsize.row+1; k++){
		for(l=0; l<gridsize.column+1; l++) {
			printf("%c", grid[k][l]);	
			
		}
		printf("\n");	
	}
	for (i=0; i<gridsize.row+1;i++) {
		free(grid[i]);
	}
	free(grid);
}

/* Function that prints the grid and fixes the bath when it is broken */
int printandrepair(int status, coords_t gridsize, node_t *first, 
	positions_t blocks[], objective_t obj, int block_count, int holdout, 
	int stage) {
	int i;
	int k;
	int l;	
	int count = 0;
	/* print if ready */
	if (!holdout) {
		printgrid(gridsize, first, blocks, obj, block_count);
	}
	/* Repair path if it needs to be */
	if (status==4) {
		int a = 1;
		node_t * current = first;
		while (current != NULL && a) {
			for (i=0;i<block_count;i++) {
				if (blocks[i].placement.row == current->next->coords.row &&
					blocks[i].placement.column == current->next->coords.column){
						a = 0;
						break;
				}
			}
			
			if(a){
				count++;
				current = current->next;
			}
		}
		
		node_t* beforebreak;
		beforebreak = current;
		
		/* Setup the grid to flood */
		int **grids = (int **)malloc(gridsize.row*sizeof(int*));
		for (i=0; i<gridsize.row;i++) {
			grids[i] = (int *)malloc(gridsize.column*sizeof(int));
		}
		for(k=0; k<gridsize.row; k++){
			for(l=0; l<gridsize.column; l++) {
				grids[k][l] = EMPTY;	
			}
		}    
		/* Setup the blockgrid */
		int **blockgrid = (int **)malloc(gridsize.row*sizeof(int*));
		for (i=0; i<gridsize.row;i++) {
			blockgrid[i] = (int *)malloc(gridsize.column*sizeof(int));
		}
		for(k=0; k<gridsize.row; k++){
			for(l=0; l<gridsize.column; l++) {
				blockgrid[k][l] = 0;	
			}
		}
		for (i=0; i<block_count; i++) {
			blockgrid[blocks[i].placement.row][blocks[i].placement.column] = 1;
		}
		/* Setup the queue and iterate over it */
		int size = 4;
		gridpoints_t *queue = (gridpoints_t *)malloc(size*sizeof(gridpoints_t));
		queue[0].coords.row = beforebreak->coords.row;
		queue[0].coords.column = beforebreak->coords.column;
		queue[0].num = 0;
		grids[beforebreak->coords.row][beforebreak->coords.column] = 0;
		int counter= 1;
		int notfound = 1;

		node_t *contination = beforebreak->next;
		while(notfound){
			queue = grid_create(gridsize ,queue, blockgrid, grids, &size,
				&counter, contination);
			if (queue[0].num == -1 || queue[0].num == -2){
				notfound = 0;	
			}
		}
		
		/* updates the path and checks the status of the newpath */
		if (queue[0].num == -1) {
			updatepath(beforebreak, queue[counter-1].coords, counter, grids, 
				gridsize);
			status = pathcheck(gridsize, first, blocks, block_count, obj, 1);
			/* if the path is fully repaired */
			if (status == 5 || stage == 1){
				printf("------------------------------------------------\n");
				printgrid(gridsize, first, blocks, obj, block_count);
				printf("------------------------------------------------\n");
				printroute(first);
				status = pathcheck(gridsize, first, blocks, block_count, obj, 
					0);
			}
			
			/* Free the grid and queue*/
			free(queue);
			for (i=0;i<gridsize.row;i++) {
				free(grids[i]);
			}
			free(grids);
			
			if (status !=4 && status != 5) {
				return 0;	
			}
		}
		
		/* When the route cannot be repaired */
		else {
			free(queue);	
			printf("------------------------------------------------\n");
			printgrid(gridsize, first, blocks, obj, block_count);
			printf("------------------------------------------------\n");
			printf("The route cannot be repaired!\n");
			return 0;
			
		}
	}
	return 1;
}

/* Floods the grid with values of the steps away from the initial cell of the
path */
gridpoints_t * grid_create(coords_t gridsize, gridpoints_t gridpoints[],
	int** blockgrid, int** grids, int *size, int *counter, node_t *contination){
	
	int i;
	int newcount = *counter;
	int changes = 0; /* Counts the changes made */
	for (i=0;i<*counter;i++) {
		int down=0;
		int up=0;
		int left=0;
		int right=0;
		int status = 0;
		/* checks to see whether the move should be made */
		down = checkvacency(blockgrid, gridpoints[i], gridsize, 1,0, grids);
		up = checkvacency(blockgrid, gridpoints[i], gridsize, -1,0, grids);
		left = checkvacency(blockgrid, gridpoints[i], gridsize, 0,-1, grids);
		right = checkvacency(blockgrid, gridpoints[i], gridsize, 0,1, grids);
		/* control the size of the array */
		if (*size <= newcount+4) {
			*size = *size*2;	
			gridpoints = (gridpoints_t *)realloc(gridpoints, 
				(*size)*sizeof(gridpoints_t));
		}
		/* Control for the different movement directions */
		if (up) {
			gridpoints[newcount].coords.row = gridpoints[i].coords.row-1;
			gridpoints[newcount].coords.column = gridpoints[i].coords.column;
			gridpoints[newcount].num = gridpoints[i].num+1;
			status = checkpath(contination, gridpoints[newcount]);
			newcount++;
			changes++;
		}
		
		if (status) {
			*counter = newcount;
			gridpoints[0].num = -1;
			return gridpoints;
		}
		
		if (down) {
			gridpoints[newcount].coords.row = gridpoints[i].coords.row+1;
			gridpoints[newcount].coords.column = gridpoints[i].coords.column;
			gridpoints[newcount].num = gridpoints[i].num+1;
			status = checkpath(contination, gridpoints[newcount]);
			newcount++;
			changes++;
		}
		
		if (status) {
			*counter = newcount;
			gridpoints[0].num = -1;
			return gridpoints;
		}

		if (left) {
			gridpoints[newcount].coords.row = gridpoints[i].coords.row;
			gridpoints[newcount].coords.column = gridpoints[i].coords.column-1;
			gridpoints[newcount].num = gridpoints[i].num+1;
			status = checkpath(contination, gridpoints[newcount]);
			newcount++;
			changes++;
		}
		
		if (status) {
			*counter = newcount;
			gridpoints[0].num = -1;
			return gridpoints;
		}
		
		if (right) {
			gridpoints[newcount].coords.row = gridpoints[i].coords.row;
			gridpoints[newcount].coords.column = gridpoints[i].coords.column+1;
			gridpoints[newcount].num = gridpoints[i].num+1;
			status = checkpath(contination, gridpoints[newcount]);
			newcount++; 
			changes++;
		}
		
		if (status) {
			*counter = newcount;
			gridpoints[0].num = -1;
			return gridpoints;
		}
	}
	
	/* On no changes to the grid, we fail to get back on the path */
	if (!changes) {
		gridpoints[0].num = -2;	
		*counter = newcount;
		return gridpoints;

	}
	*counter = newcount;
	return gridpoints;
}

/* Checks to make sure the certain cell should be filled with the numbers */
int checkvacency(int ** blockgrid, gridpoints_t point, coords_t gridsize, 
	int row, int column, int **grids){

	if (row!=0) {
		if (point.coords.row+row >= 0 && point.coords.row+row < gridsize.row) {
			if (!blockgrid[point.coords.row+row][point.coords.column]){
				if(grids[point.coords.row+row][point.coords.column] == EMPTY || 
					(grids[point.coords.row+row][point.coords.column] >
						point.num+1)) {
					(grids[point.coords.row+row][point.coords.column] =
						point.num +1);
					return 1;	
				}
			}	
		}	
	}
	
	if (column!=0) {
		if (point.coords.column+column >=0 && 
			point.coords.column+column < gridsize.column){
			if(!blockgrid[point.coords.row][point.coords.column+column]) {
				if((grids[point.coords.row][point.coords.column+column] == EMPTY 
					|| (grids[point.coords.row][point.coords.column+column] > 
						point.num+1))) {
					(grids[point.coords.row][point.coords.column+column] =
						point.num +1);
					return 1;	
				}
			}
		}
	}
	
	return 0;
}

/*Checks whether or not the filled cell is on the path */
int checkpath(node_t *contination, gridpoints_t point) {
	node_t *current = contination;
	while(current!=NULL) {
		if (current->coords.row == point.coords.row &&
			current->coords.column == point.coords.column){
			return 1;
		}
		current = current->next;
	}
	return 0;
}

/* Updates the path to the new one based on the numbers grid */
void updatepath(node_t *breakpoint, coords_t pathspot, int counter, 
	int ** grids, coords_t gridsize){

	node_t *reconstructpoint = breakpoint->next;
	while (reconstructpoint->coords.row != pathspot.row || 
		reconstructpoint->coords.column != pathspot.column) {
		reconstructpoint = reconstructpoint->next;
	}
	int notfinished = 1; 
	node_t *current = reconstructpoint;
	
	/* Works out the best path based off the flooded grid */
	while (notfinished) {
		int up = EMPTY;
		int down = EMPTY;
		int left = EMPTY;
		int right = EMPTY;
		/* Make sure that the movement will be within bounds of the grid  */
		if ((pathspot.row-1)>=0) {
			up = grids[pathspot.row-1][pathspot.column];
		}
		if ((pathspot.row+1) <gridsize.row) {
			down = grids[pathspot.row+1][pathspot.column];
		}
		if ((pathspot.column-1) >= 0) {
			left = grids[pathspot.row][pathspot.column-1];
		}
		if (pathspot.column+1 < gridsize.column) {
			right = grids[pathspot.row][pathspot.column+1];
		}	
		
		int a[] = {up, down, left, right};
		int smallest = a[0];
		int i = 0;
		int smallesti = 0;
		/* find the direction value */
		for (i=0; i<4; i++) {				
			if ((a[i] < smallest && a[i]!=EMPTY) || (smallest == EMPTY && 
				a[i]!=EMPTY)) {
				smallest = a[i];	
				smallesti = i;
				
			}
			if (a[i] == 0) {
				smallest = 0;
				smallesti = i; 
				break;
			}
		}
		
		/* Make a moved based on the values */
		if (smallesti<=1) {
			if (smallesti == 0){
				pathspot.row = pathspot.row-1;
			}
			else {
				pathspot.row = pathspot.row+1;
			}
		}
		else {
			if (smallesti==2) {
				pathspot.column = pathspot.column-1;
			}
			else {
				pathspot.column = pathspot.column+1;
				}
			}
		if (pathspot.column == breakpoint->coords.column && 
			pathspot.row == breakpoint->coords.row) {
			break;
		}
		
		current = create_node(pathspot, NULL);
		current->next = reconstructpoint;
		reconstructpoint = current;
	}
	breakpoint->next = current;	
}
/* algorithms are fun */