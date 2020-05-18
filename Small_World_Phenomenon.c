#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//Vertex structure is our main structure that implements our real movie and actors
typedef struct vertex{
	struct vertex *parent;
	char *name;
	char type;
	int isVisited;
	struct vertex *next;
	struct edge *edgeHead;
}Vertex;

//Edge structure implements connections between actors and movies
typedef struct edge{
	Vertex *vertex;
	struct edge *next;
}Edge;

//The table structure stores the calculated information to serve when requested again.
typedef struct table{
	char *name;
	char *string;
	struct table *next;
}Table;

//Head of AdjacencyLinkedList
Vertex *adjacencyListHead = NULL;
//Vertex count we will use in dynamic Queue allocation size
int vertexCount = 0;
//Queue data structure for using in breadth first search
Vertex **Q = NULL;
//Enqueue index
int enq = 0;
//Dequeue index
int deq = 0;
//Head of linked list table we use to store calculations
Table *tableHead = NULL;
//count of movies scanned
int movies = 0;

void resetVisited();
Table *createTableNode(char *name, char *string);
void addNewTableNode(char *name, char *string);
Table *searchInTable(char *name);
Vertex *createVertex(char *name, char type);
Edge *createEdge(Vertex *vertex);
void connect(Vertex *movie, Vertex *actor);
Vertex* addVertexToGraph(char *name, Vertex *movie);
void createGraph();
Vertex *searchInVertices(char *name);
Vertex *breadthFirstSearch(char *name);
void findKevinBaconNumberForASpecificName();
void printTotalKevinBaconCounts();

int main(){
	//creates graph
	createGraph();
	
	//Allocate Queue with size vertexCount we calculated while creating graph
	Q = (Vertex**)calloc(vertexCount, sizeof(Vertex*));
	if(Q == NULL){
		printf("Allocation Error");
		exit(1);
	}
	//find kevin bacon's vertex and add the first node of Queue
	Vertex *kevin = searchInVertices("Bacon, Kevin");
	//Kevin bacon is enqueued so set its is visited 1
	kevin->isVisited = 1;
	//If file doesn't include kevin bacon its an error
	if(kevin == NULL){
		printf("Input file doesn't include Kevin Bacon.");
		exit(1);
	}else{
		//enqueu kevin bacon
		Q[enq++] = kevin;
	}
	
	int mode;
	printf("Please select mode:\n");
	printf("1 - for total numbers.\n");
	printf("2 - for finding a spesific actor's Kevin Bacon number (Input must be (Surname, Name) format).\n");
	printf("3 - for exit.\n");
	do{
		printf("\n(Mode)>>");
		scanf("%d", &mode);
		if(mode == 1){
			printTotalKevinBaconCounts();
			//reset Queue and isVisiteds of vertices for next search
			enq = 1;
			deq = 0;
			resetVisited();
			kevin->isVisited = 1;
		}else if(mode == 2){
			findKevinBaconNumberForASpecificName();
			//reset Queue and isVisiteds of vertices for next search
			enq = 1;
			deq = 0;
			resetVisited();
			kevin->isVisited = 1;
		}
	}while(mode != 3);
	printf("Program Finished!");
	
	return 0;
}

//Creates a table node with the given name and string
//return created table node
Table *createTableNode(char *name, char *string){
	Table *newTable = (Table*)malloc(sizeof(Table));
	if(newTable == NULL){
		printf("Allocation Error");
		exit(1);
	}
	//Allocates name and string fields dynamically
	newTable->name = (char*)calloc(strlen(name)+1, sizeof(char));
	strcpy(newTable->name, name);
	newTable->string = (char*)calloc(strlen(string)+1, sizeof(char));
	strcpy(newTable->string, string);
	newTable->next = NULL;
	return newTable;
}

//adds given name and string to table linked list sorted
void addNewTableNode(char *name, char *string){
	//if table head is null or name of table head greater than name
	if(tableHead == NULL || strcmp(tableHead->name, name) > 0){
		//create a node and insert it front
		Table *node = createTableNode(name, string);
		node->next = tableHead;
		tableHead = node;
	}else{
		Table *current = tableHead;
		//continue while currents name is less than name
		while(current->next != NULL && strcmp(current->next->name, name) < 0){
			current = current->next;
		}
		//if current->next == NULL adds the list rear
		//if current->next->name is greater than name add between current and current->next
		//if current->next->name is equal to name doesn't adds it 
		if(current->next == NULL || strcmp(current->next->name, name)){
			Table *node = createTableNode(name, string);
			node->next = current->next;
			current->next = node;
		}
	}
}
//search the given name in table linked list
//list is sorted
//if finds given name returns it else return null
Table *searchInTable(char *name){
	//if table head is NULL list is empty return NULL
	if(tableHead == NULL){
		return NULL;
	}else{
		Table *current = tableHead;
		while(current->next != NULL && strcmp(current->name, name) < 0){
			current = current->next;
		}
		//if current->name is equal to given name print info
		if(strcmp(current->name, name) == 0){
			printf("%s", current->string);
			printf("\nSince this result was calculated before, it was read from the table.\n");
			return current;
		}else{
			return NULL;
		}
	}
}
//Creates nodes with the given name and type
//returns created vertex
Vertex *createVertex(char *name, char type){
	Vertex *newVertex = (Vertex*)malloc(sizeof(Vertex));
	if(newVertex == NULL){
		printf("Allocation Error");
		exit(1);
	}
	//allocates name field dynamically
	newVertex->name = (char*)calloc(strlen(name)+1, sizeof(char));
	strcpy(newVertex->name, name);
	newVertex->type = type;
	newVertex->parent = NULL;
	newVertex->isVisited = 0;
	newVertex->next = NULL;
	newVertex->edgeHead = NULL;
	return newVertex;
}
//creates edge with the given vertex
//returns created edge
Edge *createEdge(Vertex *vertex){
	Edge *newEdge = (Edge*)malloc(sizeof(Edge));
	if(newEdge == NULL){
		printf("Allocation Error");
		exit(1);
	}
	newEdge->vertex = vertex;
	newEdge->next = NULL;
	return newEdge;
}
//connects two given vertex adds their edge list other as an edge 
void connect(Vertex *movie, Vertex *actor){
	Edge *edgeMovie = createEdge(movie);
	Edge *edgeActor = createEdge(actor);
	
	if(movie->edgeHead == NULL){
		movie->edgeHead = edgeActor;
	}else{
		edgeActor->next = movie->edgeHead;
		movie->edgeHead = edgeActor;
	}
	if(actor->edgeHead == NULL){
		actor->edgeHead = edgeMovie;
	}else{
		edgeMovie->next = actor->edgeHead;
		actor->edgeHead = edgeMovie;
	}
}
//if the given index doesnt exist adds it to adjacency list 
//if it already exists find and connect properly
//if given name is a movie adds and returns it
//if given name is an actor adds and returns it
//if given name already exists connects it to given movie and returns null
Vertex* addVertexToGraph(char *name, Vertex *movie){
	//if adjacency lists head is null or heads name is greater than given name
	//create new vertex and insert it front
	if(adjacencyListHead == NULL || (strcmp(adjacencyListHead->name, name) > 0)){
		//if given movie is null given name is a movie
		if(movie == NULL){
			//create new movie and insert it
			Vertex *newMovie = createVertex(name, 'M');
			vertexCount++;
			newMovie->next = adjacencyListHead;
			adjacencyListHead = newMovie;
			return newMovie;
		}else{
			//if given movie is not null given name is an actor
			//create an actor and insert it
			//and connet it with the given movie
			Vertex *newActor = createVertex(name, 'A');
			vertexCount++;
			newActor->next = adjacencyListHead;
			adjacencyListHead = newActor;
			connect(movie, newActor);
			return newActor;
		}
	}else{
		//adjacency list is sorted
		//find proper position for given name
		Vertex *current = adjacencyListHead;
		while(current->next != NULL && strcmp(current->next->name, name) < 0){
			current = current->next;
		}
		//if the given name is not in adjacency list 
		if(current->next == NULL || strcmp(current->next->name, name)){
			//if given movie is null, the given name is a movie
			if(movie == NULL){
				//create new movie with the given name and insert it
				Vertex *newMovie = createVertex(name, 'M');
				vertexCount++;
				newMovie->next = current->next;
				current->next = newMovie;
				return newMovie;
			}else{
				//if the movie is not null, the given name is an actor
				//create an actor and insert it
				//connect the anctor with the given movie
				Vertex *newActor = createVertex(name, 'A');
				vertexCount++;
				newActor->next = current->next;
				current->next = newActor;
				connect(movie, newActor);
				return newActor;
			}
		}else{
			//when the given name already in adjacency list connect it with the given movie
			connect(movie, current->next);
			return NULL;
		}
	}
}
//scans given file and creates graph
void createGraph(){
	//scan file name
	char fileName[50];
	printf("Please enter the file name:");
	scanf("%s", fileName);
	
	//open file
	FILE *fp = fopen(fileName, "r");
	if(fp == NULL){
		printf("File Error.");
		exit(1);
	}
	
	char buffer[50000];
	//when end of wile fgets return null breakes while
	//fgets scans 1 line in file
	while(fgets(buffer, 50000, fp)){
		//strtok splits given string from \n
		char *token = strtok(buffer, "\n");
		//strtok splits the given string from /
		char *movieName = strtok(token, "/");
		//first part of line is a movie name insert the movie to adjacency list
		Vertex *movie = addVertexToGraph(movieName, NULL);
		//increase movie counter
		movies++;
		//in the programs that take a long time, I instantly print the number of movies read on the screen in order not to think that the program does not work
		printf("\rScanned Movie Count : %d", movies);
		//strtok gives the first actors name
		char *actorName = strtok(NULL, "/");
		//while the actors name is not null
		while(actorName != NULL){
			//call addvertextograph function and insert it adjacency list
			addVertexToGraph(actorName, movie);
			//take new actor name
			actorName = strtok(NULL, "/");
		}
	}
	printf("\n");
	fclose(fp);
}
//search given name in vertices
//Vertices are sorted
//if finds given name returns its vertex else returns null
Vertex *searchInVertices(char *name){
	//if head == NULL return null
	if(adjacencyListHead == NULL){
		return NULL;
	}else{
		Vertex *current = adjacencyListHead;
		//go until the end of list or current->name is less than given name
		while(current->next != NULL && strcmp(current->name, name) < 0){
			current = current->next;
		}
		//if current->name is equal to given name return current
		if(strcmp(current->name, name) == 0){
			return current;
		}
		//if they are not equal return null
		return NULL;
	}
}
//breadth first search implementation
//root node must be queued for working properly
//when arrives given name returns it
//if given name has not got any connection with kevin bacon returns null
Vertex *breadthFirstSearch(char *name){
	//Vertex pointer we assign the dequeued vertex
	Vertex *tempV;
	//while the dequeued vertex is not null
	do{
		//temV is dequeued vertex
		tempV = Q[deq++];
		//if dequeued vertex is null searched name is not in graph so return null
		if(tempV == NULL){
			return NULL;
		}
		//if given name and name of dequeued vertex are equal return vertex
 		if(!strcmp(tempV->name, name)){
			return tempV;
		}
		//tempE is used to enqueue edges of vertex 
		Edge *tempE = tempV->edgeHead;
		//enqueue all edges is not visited to Queue
		while(tempE != NULL){
			if(tempE->vertex->isVisited == 0){
				//enqueue all edges is not visited to Queue
				tempE->vertex->isVisited = tempV->isVisited+1;
				tempE->vertex->parent = tempV;
				Q[enq++] = tempE->vertex;
			}
			tempE = tempE->next;
		}
		//while the dequeued vertex is not null
	}while(tempV != NULL);
}
//scans actors name and searches it in graph
void findKevinBaconNumberForASpecificName(){
	//scans actors name
	char name[200];
	printf("Actor's name :");
	scanf(" %[^\n]s", name);
	
	//first search it in table which we store data already calculated
	if(!searchInTable(name)){
		//if there is no in table calculate it
		//we use buffer and temp char arrays to save output strings to table
		char buffer[50000] = {0};
		char temp[500] = {0};
		//search the given name in graph
		Vertex *find = breadthFirstSearch(name);
		//if find != NULL we found it
		if(find != NULL){
			//if its isVisited is 0 it is kevin bacon so print it
			if(find->isVisited/2 == 0){
				printf("Kevin Bacon's Kevin Bacon number is 0\n");
				//to save this string into table use sprintf for formatted sting
				sprintf(temp, "Kevin Bacon's Kevin Bacon number is 0\n");
				//and strcat to concatenation of output strings
				strcat(buffer, temp);
				//add the name and buffer string to table linked list
				addNewTableNode(name, buffer);
			}else if(find->isVisited/2 == 1){ //if isVisited is equal to 1 they have 1 movie common
				printf("%s's Kevin Bacon number is %d\n", find->name, find->isVisited/2);
				//to save this string into table use sprintf for formatted sting
				sprintf(temp, "%s's Kevin Bacon number is %d\n", find->name, find->isVisited/2);
				//and strcat to concatenation of output strings
				strcat(buffer, temp);
				printf("Movie : %s\n", find->parent->name);
				//to save this string into table use sprintf for formatted sting
				sprintf(temp, "Movie : %s\n", find->parent->name);
				//and strcat to concatenation of output strings
				strcat(buffer, temp);
				//add the name and buffer string to table linked list
				addNewTableNode(name, buffer);
			}else{
				Vertex *actor1 = find;
				Vertex *movie = actor1->parent;
				Vertex *actor2 = movie->parent;
				printf("%s's Kevin Bacon number is %d\n", actor1->name, actor1->isVisited/2);
				//to save this string into table use sprintf for formatted sting
				sprintf(temp, "%s's Kevin Bacon number is %d\n", actor1->name, actor1->isVisited/2);
				//and strcat to concatenation of output strings
				strcat(buffer, temp);
				while(actor2 != Q[0]){
					printf("%s - %s : %s\n", actor1->name, actor2->name, movie->name);
					//to save this string into table use sprintf for formatted sting
					sprintf(temp, "%s - %s : %s\n", actor1->name, actor2->name, movie->name);
					//and strcat to concatenation of output strings
					strcat(buffer, temp);
					actor1 = movie->parent;
					movie = actor2->parent;
					actor2 = movie->parent;
				}
				printf("%s - %s : %s\n", actor1->name, actor2->name, movie->name);
				//to save this string into table use sprintf for formatted sting
				sprintf(temp, "%s - %s : %s\n", actor1->name, actor2->name, movie->name);
				//and strcat to concatenation of output strings
				strcat(buffer, temp);
				//add the name and buffer string to table linked list
				addNewTableNode(name, buffer);
			}
		}else{
			printf("%s's Kevin Bacon number is infinite.\n", name);
			//to save this string into table use sprintf for formatted sting
			sprintf(temp, "%s's Kevin Bacon number is infinite.\n", name);
			//and strcat to concatenation of output strings
			strcat(buffer, temp);
			//add the name and buffer string to table linked list
			addNewTableNode(name, buffer);
		}
	}
}
//prints total kevin bacon numbers in an graph
void printTotalKevinBaconCounts(){
	//to calculate all kevin bacon numbers
	//give to breadth first search algoritm an invalid name
	//so it adds all the vertices to queue
	//so it travels all nodes in graph
	//so isVisited/2 counts of vertices is their kevin bacon number
	breadthFirstSearch("#+&/[]");
	//create an array to store counts
	int counts[100] = {0};
	Vertex *tmp = adjacencyListHead;
	//increase index of counts array for all kevin bacon numbers of all vertices 
	while(tmp != NULL){
		if(tmp->type == 'A'){
			counts[tmp->isVisited/2]++;
		}
		tmp = tmp->next;
	}
	//kevin bacon's kevin bacon number is 0, I printf it directly
	printf("The total number of actors with the number of Kevin Bacon 0 : 1\n");
	int i;
	//prints all indexes of counts array which value is not 0
	for(i = 1; i < 100; i++){
		if(counts[i] != 0){
			//prints number of actors with the i
			printf("The total number of actors with the number of Kevin Bacon %d : %d\n", i, counts[i]);
		}
	}
	//if an Actors isVisited is 0, its kevin bacon number is infinite
	//but kevin bacon's kevin bacon number is also 0
	//so number of actors with the infinite kevin bacon number is counts[0}-1
	printf("The total number of actors with the number of Kevin Bacon infinite : %d\n", counts[0]-1);
}

//after a search this functions resets isVisited fields of all vertices
void resetVisited(){
	Vertex *tmp = adjacencyListHead;
	while(tmp != NULL){
		tmp->isVisited = 0;
		tmp = tmp->next;
	}
}

