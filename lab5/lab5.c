/*
Cole Hunter
COEN 146 - Networks lab
Section 67567
Friday 2:15
*/


//lab 5
//compilation flag: -lpthread

//thread 2 -link state for function handler
//thread 3 - recieve info


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <limits.h>
#include <pthread.h>

//structs and globals
typedef struct{
	char name[50];
	char ip[50];
	int port;
}MACHINE;

pthread_mutex_t lock;
int sock;
int costs_matrix[4][4];
MACHINE nodes[4];
int global_nodes;
int global_id;
int data_out[3];
int data_in[3];

//prototypes
void read_costs(FILE* costs_file, int n_nodes);
void read_machines(FILE* machine_file, int n_nodes);
void cost_userinput(int localID);
void print_costs(int n);
void print_nodes(int n);
int minimum_dist(int distances[], int visited[]);
void *recieve_info(void *y);
void propagate_input(int n_nodes);
void *link_state(void *y); //argument actually unused, just made to match pthread_creat function def so
						   // I could get rid of the compiler warnings. same goes for recieve_info

int main(int argc, char* argv[])
{

	if(argc != 5)
	{
		printf("input args not right\n");
		printf("usage: routerID nNodes costsFile machineFile\n\n");
		return -1;
	}

	srand(time(0)); //seed for rand in link_state

	int machine_id = atoi(argv[1]);
	int n_nodes = atoi(argv[2]);
	
	//set global definitions for easy access in functions
	global_nodes = n_nodes;	
	global_id = machine_id;

	//deal with files
	FILE *costs_file;
	FILE *machines_file;
	costs_file = fopen(argv[3], "r");
	machines_file = fopen(argv[4], "r");

	read_costs(costs_file, n_nodes);
	read_machines(machines_file, n_nodes);

	int machine_port = nodes[machine_id].port;

	pthread_mutex_init(&lock, NULL); //create mutex, returns zero if all good

	//socket inits and stuff
    struct sockaddr_in machine_addr, extern_addr;
    socklen_t addr_size, extern_size;
	struct sockaddr_storage machine_storage;

	//standard config
	machine_addr.sin_family = AF_INET;
	machine_addr.sin_port = htons (machine_port);
	machine_addr.sin_addr.s_addr = htonl (INADDR_ANY);

	memset (machine_addr.sin_zero, '\0', sizeof (machine_addr.sin_zero));  
	addr_size = sizeof machine_storage;

	//create socket
	if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf ("socket error\n");
		return 1;
	}

	//bind socket to our IP
	if (bind (sock, (struct sockaddr *)&machine_addr, sizeof (machine_addr)) != 0)
	{
		printf ("bind error\n");
		return 1;
	}



	//start 2 additional threads needed (3 total)
	pthread_t rec_thread;
	pthread_create(&rec_thread, NULL, recieve_info, NULL);

	pthread_t link_thread;
	pthread_create(&link_thread, NULL, link_state, NULL);

	for(int i=0; i < 2; i++)	//2 random
	{
		cost_userinput(machine_id); 
		sleep(10); //defined in unistd, acts like time.sleep - who knew?
	}

	return 0;
}

void read_costs(FILE* costs_file, int n_nodes)
{
//NxN table
/*
	The table with costs will look like this if N = 3:
	<0,0> <0,1> <0,2>
	<1,0> <1,1> <1,2>
	<2,0> <2,1> <2,2>

	where <i,j> represents the cost between node i and node j.
	If <i, j> is equal to infinite (defined as 10,000),
	nodes i and j are not neighbors.
*/

	for(int i = 0; i < n_nodes; i++)
	{
		int k;
		for(int j = 0; j < n_nodes; j++) //square matrix, nxn
		{
			k = fscanf(costs_file, "%d", &costs_matrix[i][j]);
			//(file, formats, vars)
			if(k != 1)
				break;
		}

	}
	print_costs(n_nodes);
}

void print_costs(int n)
{
	printf("costs:\n");
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < n; j++)
		{
			printf("%d ", costs_matrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void read_machines(FILE* machine_file, int n_nodes)
{
	for(int i = 0; i < n_nodes; i++)
	{
		int k;
		k = fscanf(machine_file, "%s %s %d", &(nodes[i].name), &(nodes[i].ip), &(nodes[i].port));
		if(k < 1)
			break;
	}
	print_nodes(n_nodes);

}

void print_nodes(int n)
{
	printf("machines:\n");
	for(int i = 0; i < n; i++)
	{
		printf("%s %s %d\n", (nodes[i].name), (nodes[i].ip), (nodes[i].port));
	}
	printf("\n");
}

void cost_userinput(int localID)
{
	//asks user to input a new cost to adjacent node - called every 10 seconds 
	
	int newCost;
	int neighborID;

	printf("update cost to neighbor node - enter neighborID and newCost as 2 ints seperated by a space\n\n");
	scanf("%d %d", &neighborID, &newCost);

	pthread_mutex_lock(&lock);

	costs_matrix[localID][neighborID] = newCost;
	costs_matrix[neighborID][localID] = newCost;

	//send out new data
	data_out[0] = htonl(localID);
	data_out[1] = htonl(neighborID);
	data_out[2] = htonl(newCost);
	propagate_input(global_nodes); //do send

	//for debug
	printf("sending data: id1:%d id2:%d cost:%d\n\n", data_out[0], data_out[1], data_out[2]);

	print_costs(global_nodes);

	pthread_mutex_unlock(&lock);

}

void *link_state(void *y)
{
	time_t last_update;
	last_update = time(0); //seconds since epoch - year 1970

	while(1)
	{
		//prev iteration of waiting function - easier to just wat 15 seconds between caluclations.
		//int thresh = rand()%mod_waittime;
		//if( time(0) - last_update > thresh) //been too long since update
		
		if(1)
		{
			//djikstra goes here.
			
			int n = global_nodes;
			
			int distances[n];
			int visited_nodes[n];
			int costs[n][n];

			pthread_mutex_lock(&lock);

			for(int x = 0; x < n; x++)
			{
				for(int i = 0; i < n; i++)
				{
					//set initial values
					distances[i] = INT_MAX; //carlos says intmax, not 10,000
					visited_nodes[i] = 0;
				}
				distances[x] = 0; //distance to ourself

				for(int j = 0; j < n-1; j++)
				{
					//find distance index
					int ind = minimum_dist(distances, visited_nodes);

					visited_nodes[ind] = 1; //mark visited

					for(int k = 0; k < n; k++)
					{
						//very long line ahead
						if( (visited_nodes[k] == 0) && (costs_matrix[ind][k]) && (distances[ind] != INT_MAX) && ((distances[ind] + costs_matrix[ind][k]) < distances[k]) )
						{
							distances[k] = distances[ind] + costs_matrix[ind][k]; //update distance
						}
					}
				}

				//print distances
				printf("Dijkstra computed: distance from node %d = ", x);
				for(int z = 0; z < n; z++)
					printf("%d, ", distances[z]);
				printf("\n");
			}

			pthread_mutex_unlock(&lock);
			printf("\n");
			last_update = time(0); //no longer needed
			sleep(15);
		}
	}
}

int minimum_dist(int distances[], int visited[])
{
	int min = INT_MAX;
	int index;

	//iterate over distances to find min, return the index
	for(int i = 0; i < global_nodes; i++)
	{
		if( (visited[i] == 0) && (distances[i] < min) )
		{
			min = distances[i];
			index = i;
		}
	}

	return index;
}


void *recieve_info(void *y)
{
	//this is whole function is continuous duty of 1 thread


	while(1)
	{
		int nBytes = recvfrom (sock, &data_in, sizeof(data_in), 0, NULL,NULL); //recieve

		int hostA = ntohl(data_in[0]);
		int hostB = ntohl(data_in[1]);
		int cost = ntohl(data_in[2]);

		//for debug
		printf("recieved update - new data\n");
		printf("hostA: %d, hostB: %d, cost=%d\n\n", hostA, hostB, cost);

		pthread_mutex_lock(&lock);

		//update matrix
		costs_matrix[hostA][hostB] = cost;
		costs_matrix[hostB][hostA] = cost;

		print_costs(global_nodes);

		pthread_mutex_unlock(&lock);
	}
}

void propagate_input(int n_nodes)
{

	int send_sock;
	struct sockaddr_in dest_addr[n_nodes];
	socklen_t addr_size[n_nodes];

	for(int i = 0; i < n_nodes; i++)
	{
		dest_addr[i].sin_family = AF_INET;
		dest_addr[i].sin_port = htons (nodes[i].port);
		inet_pton (AF_INET, nodes[i].ip, &dest_addr[i].sin_addr.s_addr);
		memset (dest_addr[i].sin_zero, '\0', sizeof (dest_addr[i].sin_zero));
		addr_size[i] = sizeof(dest_addr[i]);
	}

	send_sock = socket(PF_INET, SOCK_DGRAM, 0);

	//send new data to each of the nodes that arent us.

	for(int i = 0; i < n_nodes; i++)
	{
		if(i != global_id)
			sendto (send_sock, &data_out, sizeof(data_out), 0, (struct sockaddr *)&(dest_addr[i]), addr_size[i]);
	}
}
