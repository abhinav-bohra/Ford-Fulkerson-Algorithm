/*------------------------------------------------------------------------------------------------------------------------------ 
    Implementation of Ford–Fulkerson method for computing the maximum flow in a flow network using Edmonds–Karp algorithm.
    Author - Abhinav Bohra
    Date - November 4, 2020
-------------------------------------------------------------------------------------------------------------------------------*/
#include<stdlib.h>
#include<stdio.h>
#include <limits.h>
#define MAX 10000

/*------------------------------------------------------------------------------------------------------------------------------------------------*/
/*----- Structures -----*/

typedef struct edge {

    int y;              // integer y storing the endpoint vertex y of an edge (x, y) (edge from x to y)
    int c;              // integer c storing the capacity of the edge
    int f;              // integer f storing the flow value to be assigned on the edge
    struct edge *next;  // pointer next to store edges in an adjacency list

}EDGE;

typedef struct vertex {

    int x;              // integer x storing the id of the vertex
    int n;              // integer n storing the need value of the vertex 
    EDGE *p;            // pointer p storing a pointer to an EDGE node 

}VERTEX;

typedef struct graph {

    int V;              // integer V storing the number of vertices
    int E;              // integer E storing the number of edges
    VERTEX *H;          // pointer H storing a pointer to an array of VERTEX nodes

}GRAPH;


typedef struct queue { 

    /* Queue for performing BFS (Queue has been implemented using array) */

    int front, rear;   //integer front & rear to keep track of front index & rear index respectively
    int size;          //integer size to keep track of current size of queue
    int capacity;      //integer capacity to store Maximum capacity of queue
    int* array;        //pointer to interger array storing elements in queue

}Queue; 

/*------------------------------------------------------------------------------------------------------------------------------------------------*/
/*----- Function Prototyping -----*/

GRAPH *ReadGraph(char *fname);                        // Function to read graph
void PrintGraph(GRAPH G);                             // Function to print graph's adjacency list
void ComputeMaxFlow(GRAPH *G, int s, int t);          // Function to compute maximum flow for given graph, souce vertex & sink vertex
void NeedBasedFlow(GRAPH *G);                         // Function to compute need based flow for given graph

int BFS(int s,int t,GRAPH *G,int * parent);           // Function to perform Breadth First Traversal to find shortest augmenting path
int checkFeasibility(GRAPH *G);                       // Function to check feasibility of need based flow

/*-----Queue Functions' Prototyping---*/
Queue* createQueue(int capacity);                     //Function to create queue of specified capacity
int isFull(Queue* queue);                             //Function to check whether queue is full or not
int isEmpty(Queue* queue);                            //Function to check whether queue is empty or not
void enqueue(Queue* queue, int item);                 //Function to insert element in queue 
int dequeue(Queue* queue);                            //Function to delete & return front element of queue
int front(Queue* queue) ;                             //Function to fetch front element in queue

/*------------------------------------------------------------------------------------------------------------------------------------------------*/
/*----- Main Driver Function -----*/
        
int main(){
	
	GRAPH *myGraph1,*myGraph2;

    char S[20];
    int source,sink;

    printf("Please Enter File Name: ");
    scanf("%s",&S);
    myGraph1 = ReadGraph(S);
	PrintGraph(*myGraph1);   

    printf("Please Enter id of Source Node: ");
    scanf("%d",&source);
    printf("Please Enter id of Sink Node: ");
    scanf("%d",&sink);

    printf("\nAssignment Part 1 : Compute Max Flow \n");
	ComputeMaxFlow(myGraph1,source,sink);
    PrintGraph(*myGraph1);

    printf("\nAssignment Part 2 : Need Based Flow \n");
    myGraph2 = ReadGraph(S);          
    NeedBasedFlow(myGraph2);
    //Print graph only if need based flow is feasible
    if(checkFeasibility(myGraph2)) PrintGraph(*myGraph2);
    else                            printf("\nNo Need Based Flow Exists.\n\n"); // Graph is not printed
    return 0;
}
/*------------------------------------------------------------------------------------------------------------------------------------------------*/
/*----- Read Graph Function -----*/

GRAPH *ReadGraph(char *fname){

    /*
     * Arguments      : Character pointer to File Name
     * Task Performed : Initialises number of vertices & number of edges of Graph myGraph
     *                  Creats an adjacenty list (adjList) & stores graph edges in it
     * Returns        : Pointer to myGraph                 
    */
    
    FILE *fptr;
    fptr = fopen(fname, "r");
    
    if (fptr == NULL){
        printf("Could not open file %s.\n",fname);
        exit(0);
    }
    else{

        GRAPH *myGraph = (GRAPH *)malloc(sizeof(GRAPH));
        
        fscanf(fptr,"%d",&myGraph->V); //Initialise number of vertices in graph
        fscanf(fptr,"%d",&myGraph->E); //Initialise number of edges in graph
        
        VERTEX *H = (VERTEX *)malloc((myGraph->V+1)*sizeof(VERTEX));		
		EDGE **adjList = (EDGE **)malloc((myGraph->E+1)*sizeof(EDGE*));

        for(int i=0;i<=myGraph->E;i++){
        	adjList[i]=NULL;			//Initializing array of pointers with NULL
        }
        
        for(int i=1;i<=myGraph->V;i++){
            H[i].x=i;                             // Start Vertex of Edge
            fscanf(fptr,"%d",&H[i].n);            // Need of Vertex
            H[i].p=NULL;                          // Initialises pointer to array of neighbour nodes with NULL
        }        
    
        //Filling Adjacency List
        for(int i=0;i<myGraph->E;i++){
            
            EDGE *temp,*p;
            temp = (EDGE*)malloc(sizeof(EDGE));
            int x;
            fscanf(fptr,"%d",&x);         //Read x
            fscanf(fptr,"%d",&(temp->y)); //Read y
            fscanf(fptr,"%d",&(temp->c)); //Read c
            
            temp->f=0;
            temp->next=NULL;

            if(adjList[x]==NULL) adjList[x]=temp;
            else{

                p=adjList[x];
                if(p->y == temp->y){
                    p->c = p->c + temp->c;           //Increase capacity if edge already exists
                    continue;
                }

                int flag=0;
                while(p->next!=NULL){
                    
                    if(p->y == temp->y){
                        p->c = p->c + temp->c;        //Merge paralled edge if an edge already exists
                        flag=1;
                        break;
                    }
                    p=p->next;
                }

                if(flag) p->next=NULL;
                else     p->next=temp;
            }
        }  
        
        for(int i=1;i<=myGraph->E;i++){
            H[i].p=adjList[i]; 
        } 

        myGraph->H=H;
       
        return myGraph;
    }  
} 
/*------------------------------------------------------------------------------------------------------------------------------------------------*/
/*----- Print Graph Function -----*/

void PrintGraph(GRAPH G){
    
    /*
     * Arguments      : Graph G
     * Task Performed : Prints adjacenty list of Graph G in the following format
     *                  V1 -> (V2,c2,f2) -> (V3,c3,f3)
     *                  V2 
     *                  V3
     *                  The above network has 2 edges : One from V1 to V2 with capacity c2 & flow f2 
     *                  and another one from V1 to V3 with capacity c3 & flow f3                      
     * Returns        : void                 
    */

	printf("\nThe Graph is:- \n\n");
	
    VERTEX * H = G.H;
	for(int i=1;i<=G.V;i++){
		
		printf("%d", i);
		EDGE * pointer =H[i].p;
        
		while(pointer!=NULL){

			if(pointer->c > 0) printf(" -> (%d,%d,%d) ",pointer->y,pointer->c,pointer->f);	//Print edges with positive capacities
			pointer=pointer->next;
		}

		printf("\n");
	}
	printf("\n");
}

/*------------------------------------------------------------------------------------------------------------------------------------------------*/
/*----- Compute Max Flow Function -----*/

void ComputeMaxFlow(GRAPH *G, int s, int t){
    
    /* 
     * Based on       : Ford-Fulkerson Method
     * Arguments      : Pointer to Graph, source vertex : s, sink vertex : t
     * Task Performed : 1) Add reverse edges with capacity =0 (if applicable)
     *                  2) Finds shortest augmenting path with maximum residual capacity (among shortest paths) using function BFS, 
     *                  3) Stores augmenting path using a parent array where parent[i] is parent of vertex i, 
     *                     A vertex j with no parent has parent [j]= -1
     *                  4) To find the augmenting path, we backtrack till source vertex using parent array
     *                  5) Stores flow obtained from BFS function in var newFlow
     *                  6) Increases maxFlow in each iteration till there exists an augmenting path by amount = newFlow
     *                  7) Prints maximum flow for given network, source & sink vertex                  
     * Returns        : void                 
    */

   //Add reverse flow edges with capacity 0

    VERTEX * H = G->H;
	for(int i=1;i<=G->V;i++){
		
		EDGE * pointer =H[i].p;   
        int x = i;

		while(pointer!=NULL){

			int y = pointer->y;
            if(pointer->c == 0) {
                pointer=pointer->next;  // To avoid redundant edges
                continue;
            }

            EDGE * reverseEdge = (EDGE*)malloc(sizeof(EDGE));
            reverseEdge->y=x;
            reverseEdge->c=0;
            reverseEdge->f=0;
            reverseEdge->next=NULL;          

            if(H[y].p==NULL) H[y].p=reverseEdge;
            else{
                    EDGE * pointer_new =H[y].p;
                    int flag=0;
                    
                    while (pointer_new->next!=NULL)
                    {
                        if(pointer_new->y == x) {       //Reverse edge exists
                            flag=1;
                            break;
                        }
                        pointer_new = pointer_new->next;
                    }

                    if(flag == 0) pointer_new->next =  reverseEdge;       //Reverse edge does not exist, add one with capacity = 0                 
            }
          
          	pointer=pointer->next;
		}
    }

    
   int maxFlow=0;
   int newFlow=0; 
   int * parent = (int*)malloc((G->V + 1)*sizeof(int));

   while(newFlow = BFS(s,t,G,parent)){
        
        maxFlow = maxFlow + newFlow;
        int cur = t;
        
        while (cur != s) {   //Backtrack till soucrce vertex is found

            int prev = parent[cur];            
            EDGE * pointer = G->H[prev].p;

            while (pointer->y != cur) pointer=pointer->next;
            pointer->f = pointer->f + newFlow;                //Update flow of all edges in augmenting path
    
            pointer = G->H[cur].p;
            while (pointer->y != prev) pointer=pointer->next;
            pointer->f = pointer->f - newFlow;
    
            cur = prev;
        }
   }
  
   printf("\nThe maximum amount of integal flow that can flow from Source (id -> %d) to Sink (id -> %d) is %d.\n", s,t,maxFlow);

   /*   
        // To check existence of Need Based Flow and print message accordingly

        //Net need should be zero, sigma n[i]=0 is a neccesary condition for flow to exist.
	    int net_need=0,flag=0;
	    for(int i=1;i<=G->V;i++) net_need += G->H[i].n;
	    if(net_need!=0) {
	    		flag=1;
	    		break; // Not feasible
		}

		//Assigned flow on every edge from source to a producer should be equal to its capacity
		EDGE * p = G->H[G->V-1].p;
		while(p!=NULL){
		   if(p->f != p->c){
	    		flag=1;
	    		break; // Not feasible
		   }
		   p=p->next;
		}

		//Assigned flow on every edge from a consumer to a sink should be equal to its capacity
		EDGE * q = G->H[G->V-1].p;
		while(q!=NULL){
		   if(q->f != q->c){
	    		flag=1;
	    		break; // Not feasible
		    }
		   q=q->next;
		}
		    
		 if(flag) printf("\nNeed Based Flow exists and is equal to %d. \n",maxFlow);
		 else     printf("\nNeed Based Flow Does not Exist.");
   */

}

/*------------------------------------------------------------------------------------------------------------------------------------------------*/
/*----- Need Based Flow Function -----*/

void NeedBasedFlow(GRAPH *G){
    /*
     * Based on       : Edmond-Karp Algorithm
     * Arguments      : Pointer to Grarph
     * Task Performed : 1) Create Universal Source S
     *                  2) Create Universal Sink T
     *                  3) Connect all consumers to S with edge weight = need[i]
     *                  4) Connect all producers to T with edge weight = -need[i]
     *                  Now, we apply computeMaxFlow() to find maxFlow in the above augmented graph.
     *                  The obtained maxFlow will be the need based flow for the original graph.
     * Returns        : void                 
     * NOTE           : The algorithm assigns zero flow to all edges in network when no need based flow exists
    */

    //Condition 1
    int net_need=0;
    for(int i=1;i<=G->V;i++){
        net_need  = net_need + G->H[i].n;
    }

    if(net_need !=0) return;

    //Add two more vertices (Universal source & sink) to current graph    
    G->V = G->V + 2;
    G->H = (VERTEX*) realloc(G->H, (G->V +1)*sizeof(VERTEX) );
    
    //Filling data for new 2 vertices
    int source = G->V - 1;
    int sink   = G->V;
    
    G->H[source].x=G->V-1;  //Source Vertex
    G->H[sink].x=G->V;      //Sink Vertex

    G->H[source].n=0;
    G->H[sink].n=0;

    G->H[source].p=NULL;
    G->H[sink].p=NULL;

    //Add new edges
    for(int i=1;i<=G->V;i++){

        if(G->H[i].n == 0) continue;
        if(G->H[i].n < 0)
        {
            //Vertex is a producer, so add an edge from source to vertex i
            EDGE *temp,*p;
            temp = (EDGE*)malloc(sizeof(EDGE));
            temp->y=i;
            temp->c = -G->H[i].n;
            temp->f = 0;
            temp->next=NULL;

            if(G->H[source].p==NULL) G->H[source].p=temp;
            else
            {
                p=G->H[source].p;
                while(p->next!=NULL){
                    p=p->next;
                }
                p->next=temp;                
            }            
        }
        else
        {
            //Vertex i is a consumer, so add an edge from vertex i to sink
            EDGE *temp,*p;
            temp = (EDGE*)malloc(sizeof(EDGE));
            temp->y=sink;
            temp->c = G->H[i].n;
            temp->f = 0;
            temp->next=NULL;

            if(G->H[i].p==NULL) G->H[i].p=temp;
            else{
                p=G->H[i].p;
                while(p->next!=NULL){
                    p=p->next;
                }
                p->next=temp;
            } 
        }
    }
     
	ComputeMaxFlow(G,source,sink);

    return;
    
}

int BFS(int s,int t, GRAPH *G, int * parent){
    
    /*
     * Arguments      : Pointer to Grarph, source vertex : s, sink vertex : t, parent array to store augmenting path
     * Task Performed : 1) Maintains two Queues namely, vertexQueue (used for performing BFS using queue) and 
     *                     flowQueue (used to store flow of corresponding vertex in vertexQueue)      
     *                  2) Any enque/dequeue operation on vertexQueue is accompanied by the same operation on flowQueue
     *                  3) Performs BFS on graph G to find the shortest augmenting path
     *                  4) Determines minimum capacity (stored in var newFlow) in the augmenting math
     *                  5) Determines maximum of all the newFlows (stored in var maxFlow_path)on reaching the sink vertex
     * Returns        : maximum flow among all shortest augmenting paths (Returns 0 if no augmenting path found)                
     * 
     * NOTE           : The algorithm assigns zero flow to all edges in network when no need based flow exists.
    */
    
    if(s==t) return 0; //Trivial Corner Case Handling

    for(int i=0;i<= G->V ;i++) {
        parent[i]=-1;
    }
    parent[s]=0;

    int maxFlow_path=0;
    Queue* vertexQueue = createQueue(MAX);
    Queue* flowQueue   = createQueue(MAX);
    enqueue(vertexQueue,s);                         //Starts with source vertex
    enqueue(flowQueue,INT_MAX);                     //Denotes infinte flow edges

    while(!isEmpty(vertexQueue)){

        int currentVertex = dequeue(vertexQueue);
        int currentFlow   = dequeue(flowQueue);    
        
        EDGE * pointer = G->H[currentVertex].p;

        while(pointer!=NULL){
            
            int nextVertex = pointer->y;
            int residualCapacity = pointer->c - pointer->f;
            if(nextVertex==t){                                   //Update maxFlow_path if nextVertex is sink

                int newFlow = currentFlow < residualCapacity ? currentFlow : residualCapacity;
                       
                if(maxFlow_path < newFlow){
                    maxFlow_path= newFlow;
                    parent[nextVertex]= currentVertex;
                }
            }
            else if(parent[nextVertex]==-1 && residualCapacity > 0){
                
                parent[nextVertex] = currentVertex;                                             //current vertex is parent of next vertex
                int newFlow = currentFlow < residualCapacity ? currentFlow : residualCapacity;  //Update newFlow
                enqueue(vertexQueue,nextVertex);                                                //push next vertex in queue
                enqueue(flowQueue,newFlow);                                                     //push next flow in queue
            }

            pointer=pointer->next;
        }
    }
    return maxFlow_path;

}
int checkFeasibility(GRAPH *G){
    /*
     * Arguments      : Pointer to Grarph
     * Task Performed : 1) Condition 1 : Sigma n[i]=0 is a neccesary condition for flow to exist.
     *                  2) Condition 2 : Sum of assigned flow to Edges from universal source must be equal to sum of need of producers
     *                  3) Condition 3 : Sum of assigned flow to Edges to universal sink must be equal to sum of need of consumers
     *                  NOTE : Condition 2 and 3 are equivalent, checking any one of them is sufficient
     * Returns        : 1 if need based flow is feasible, 0 if not              
    */

    //Net need should be zero, sigma n[i]=0 is a neccesary condition for flow to exist.
    int net_need=0;
    for(int i=1;i<=G->V;i++) net_need += G->H[i].n;
    if(net_need!=0) return 0; // Not feasible

    //Assigned flow on every edge from source to a producer should be equal to its capacity
    EDGE * p = G->H[G->V-1].p;
    while(p!=NULL){
       if(p->f != p->c) return 0; // Not feasible
       p=p->next;
    }
    
    //Assigned flow on every edge from a consumer to a sink should be equal to its capacity
    EDGE * q = G->H[G->V-1].p;
    while(q!=NULL){
       if(q->f != q->c) return 0; // Not feasible
       q=q->next;
    }

    return 1;
}
//--------------------------------------------------------------------------------------------------------------------------------
/* --- Queue Standard Functions (Implemented using array) --*/

Queue* createQueue(int capacity) 
{ 
    Queue* queue =(Queue*)malloc(sizeof(Queue)); 
    queue->capacity = capacity; 
    queue->front = 0;
    queue->size = 0; 
    queue->rear = capacity - 1; 
    queue->array = (int*)malloc(capacity*sizeof(int));
    return queue; 
} 

int isFull(Queue* queue) 
{ 
    return (queue->size == queue->capacity); 
} 

int isEmpty(Queue* queue) 
{ 
    return (queue->size == 0); 
} 
  
void enqueue(Queue* queue, int item) 
{ 
    if (isFull(queue)) return; 
    queue->rear = (queue->rear + 1) % queue->capacity; 
    queue->array[queue->rear] = item; 
    queue->size = queue->size + 1; 
} 
  
int dequeue(Queue* queue) 
{ 
    if (isEmpty(queue)) return -1; 
    int item = queue->array[queue->front]; 
    queue->front = (queue->front + 1) % queue->capacity; 
    queue->size = queue->size - 1; 
    return item; 
} 
  
int front(Queue* queue) 
{ 
    if (isEmpty(queue)) return -1; 
    return queue->array[queue->front]; 
} 
//--------------------------------------------------------------------------------------------------------------------------------
