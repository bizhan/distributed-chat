#pragma once
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdint.h>
#include <time.h>
#include <ncursesw/curses.h>

#include "queue.h"
#include "llist.h"
#include "send_msg.h"
#include "unreliablesplash.h"

#define QUEUE_SIZE 1024
#define MSGBUFSIZE 256

#define RECEIVE_THREADNUM 0
#define SEND_THREADNUM 1
#define CHECKUP_THREADNUM 2
#define FAIRSEQ_THREADNUM 3

#define MAXIPLEN 32
#define MAXSENDERLEN 64
#define MAXUIDLEN 128
#define MAXPACKETLEN 1024
#define MAXPACKETBODYLEN 750// MAXPACKETLEN-MAXSENDERLEN-MAXUIDLEN-(2*sizeof(int))-sizeof(int)-(5*sizeof(char))
#define MESSAGEMULTIPLIER 10
#define MAXCHATMESSAGELEN MAXPACKETBODYLEN*MESSAGEMULTIPLIER
#define JOIN_TIMELIMIT_MS 5000000
#define CHECKUP_INTERVAL 3
#define CHECKUP_DEATH_TIMELIMIT 3
#define QUORUM_TIMEOUT_MS 8000
#define CATESTROPHIC_NETWORK_OUTAGE 30000000
#define ELECTION_TIMEOUT_MS 8000
#define ELECTION_SLEEP_INTERVAL_uS 100000

#define PACKETDELIM "\n"
#define IPPORTSTRDELIM ":"

#define DEFAULTPORT 2886

#define FAIR_SEQ_WAIT 50000
//#define FAIR_SEQ_WAIT 5000000 //for demonstration purposes


enum packettype { CHAT = 0, SEQUENCE = 1, CHECKUP = 2, ELECTION = 3, VOTE = 4, VICTORY = 5, JOIN_REQUEST = 6, LEADER_INFO = 7, JOIN = 8, EXIT = 9, QUORUMRESPONSE = 10, CONFIRMDEAD = 11, CONFIRMCOUP = 12};
typedef enum packettype packettype_t;

typedef struct packet_t {
  char sender[MAXSENDERLEN];
  char senderuid[MAXSENDERLEN];
  char uid[MAXUIDLEN];
  packettype_t packettype;
  int packetnum;
  int totalpackets;
  char packetbody[MAXPACKETBODYLEN];
} packet_t;

typedef struct chatmessage_t {
  packettype_t messagetype;
  int seqnum;
  int numpacketsexpected;
  bool iscomplete;
  bool packetsreceived[MESSAGEMULTIPLIER];//indicates which packets have been received
  char sender[MAXSENDERLEN];
  char uid[MAXUIDLEN];
  char senderuid[MAXSENDERLEN];
  char messagebody[MAXCHATMESSAGELEN];
} chatmessage_t;

typedef struct client_t {
  char username[MAXSENDERLEN];
  char uid[MAXSENDERLEN];
  char hostname[MAXIPLEN];
  int portnum;
  bool isleader;
  bool isCandidate;
  int missed_checkups;
  llist_t* unseq_chat_msgs;
  int num_votes;
  char deferent_to[MAXSENDERLEN];
} client_t;


llist_t* UNSEQ_CHAT_MSGS;
llist_t* CLIENTS;
llist_t* STRAY_SEQ_MSGS;
queue_t* HBACK_Q; 
client_t* me;
int LOCALPORT;
char* LOCALHOSTNAME;
char LOCALUSERNAME[MAXSENDERLEN];
int SEQ_NO; 
int LEADER_SEQ_NO; 

bool JOIN_SUCCESSFUL;

int UID_COUNTER; 
pthread_mutex_t dump_backlog_mutex;
pthread_mutex_t counter_mutex;
pthread_mutex_t seqno_mutex;
pthread_mutex_t me_mutex;
pthread_mutex_t missed_checkups_mutex;
pthread_mutex_t election_happening_mutex;
pthread_mutex_t coup_propogated_mutex;

bool DUMP_BACKLOG;

int num_clients_disagree_on_death_call;
int num_clients_agree_on_death_call;

int failed_quorums;
bool election_happening;
bool coup_propogated; // Reset this global variable only before return of receipt of VICTORY message

// Function Declarations

void get_new_uid(char uid[]);

void *get_user_input(void* t);

void *checkup_on_clients(void* t);

bool check_quorum_on_client_death(char uid_death_row_inmate[]);

void holdElection();

bool initialize_data_structures();

void stage_coup(char incoming_power[]);

void discover_ip(char ip[]);

void handle_major_network_outage_during_election();