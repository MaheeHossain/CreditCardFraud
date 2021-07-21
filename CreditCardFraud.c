/* Credit Card Reader:
 *
 * Developed by Mahee Hossain, 1080102
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "listops.h"
#include "listops.c"

#define STAGE_NUM_ONE 1							/* stage numbers */ 
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4

#define STAGE_EQUALS "========================="
#define STAGE_HEADER "%sStage %d%s\n"
#define HASH "%%%%%%%%%%"
#define SPACES "             "

#define CARD_ID_LENGTH 9
#define TRANSACTION_ID_LENGTH 13
#define CREDIT_CARD_MAX_NUM 100
#define TRANSACTION_MAX_NUM 100000                  // No given limit???

/****************************************************************/

/* typedefs */

typedef char card_id_t[CARD_ID_LENGTH];
typedef char trans_id_t[TRANSACTION_ID_LENGTH];

typedef struct node node_t;

typedef struct {                           // Struct for the time
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    
} time_t;

typedef struct {                           // Struct for the credit card
    card_id_t card_ID;
    int daily_limit;
    int trans_limit;    
    
} credit_card_t;

typedef struct {                           // Struct for the transactions
    trans_id_t trans_ID;
    card_id_t card_ID;
    time_t time;
    int trans_amount;    
    
} transaction_t;

struct node {
	transaction_t data;
	node_t *next;
};

typedef struct {
    node_t *head;
    node_t *foot;
    
} list_t;


/****************************************************************/

/* function prototypes */

// STAGES
void stage_one(credit_card_t *card);
void stage_two(credit_card_t cards[], int *num_cards);
void stage_three(list_t *trans_list);
void stage_four(list_t *trans_list, credit_card_t cards[], int num_cards);
void print_stage_header(int stage_num);

// STAGES ONE AND TWO
void read_one_card(credit_card_t *card, int max_id_len);
void print_card(credit_card_t card);
void set_cards(credit_card_t cards[], int *num_cards);
void read_card_records(credit_card_t cards[], int *num_cards);

// STAGE THREE
int read_one_trans(transaction_t *trans);
list_t *make_empty_list(void);
list_t *insert_at_foot(list_t *list, transaction_t value);
transaction_t get_head(list_t *list);
list_t *get_tail(list_t *list);
void print_trans(node_t *node);

// STAGE FOUR
char* print_limit_check(int check);
void money_today(node_t *node, int *curr_day, int *curr_month, 
                 int *curr_year, int *today_money);
int check_limits(node_t *node, credit_card_t card, int check, int today_money);
void non_binary_method(list_t *trans_list, credit_card_t cards[], 
                       int num_cards);


/****************************************************************/

/* main function controls all the action, do NOT modify this function */
int main(int argc, char *argv[]) {
	/* to hold all the credit cards */
	credit_card_t cards[CREDIT_CARD_MAX_NUM];
    /* to hold the number of credit cards */
    int num_cards=0;
	/* to hold all transactions */
	list_t *trans_list=make_empty_list();
	
	/* Reads a credit card and prints the output */
	stage_one(&cards[num_cards]);
    num_cards++;
	
	/* Reads all the credit card records, prints output */
	stage_two(cards, &num_cards);
	
	/* Reads the transactions, prints their IDs */
	stage_three(trans_list);
	
	/* Checks for fraudulent transactions */
	stage_four(trans_list, cards, num_cards);
	
	return 0;
}

/****************************************************************/

/* Helper Functions */

void print_stage_header(int stage_num) {
    /* print stage header given stage number */
	printf(STAGE_HEADER, STAGE_EQUALS, stage_num, STAGE_EQUALS);
}

void read_one_card(credit_card_t *card, int max_id_len) {
    /* read a line of input into one_msg */
    scanf("%s", card->card_ID);
    if (strcmp(&card->card_ID[0], HASH) != 0) {
        scanf(" %d %d", &card->daily_limit, &card->trans_limit);
        card->card_ID[max_id_len] = '\0';
    }
}

void print_card(credit_card_t card) {
    /* prints the information of a credit card in the style of stage 1 */
    printf("Card ID: %s\n", card.card_ID);
    printf("Daily limit: %d\n", card.daily_limit);
    printf("Transaction limit: %d\n\n", card.trans_limit);
}

void set_cards(credit_card_t cards[], int *num_cards) {
    /* Sets all the credit card info in the array "cards" */
    for (int i=1; i<CREDIT_CARD_MAX_NUM; i++) {
        read_one_card(&cards[i], CARD_ID_LENGTH);
        if (strcmp(&cards[i].card_ID[0], HASH) == 0) {
            break;
        }
        (*num_cards)++;
    }
}

void read_card_records(credit_card_t cards[], int *num_cards) {
    /* Takes an array of credit cards and prints their records */
    card_id_t max_trans_id;
    int max_trans=0;
    double ave_daily=0.0;
    for (int j=0; j<*num_cards; j++) {
        ave_daily += cards[j].daily_limit;
        if (cards[j].trans_limit > max_trans) {
            max_trans = cards[j].trans_limit;
            strcpy(max_trans_id, cards[j].card_ID);
        }
    }
    ave_daily /= *num_cards;
    printf("Number of credit cards: %d\n", *num_cards);
    printf("Average daily limit: %.2lf\n", ave_daily);
    printf("Card with the largest transaction limit: %s\n\n", max_trans_id);
}

int read_one_trans(transaction_t *trans) {
    /* read a line of input into one_msg */
    if (scanf("%s %s %d:%d:%d:%d:%d:%d %d", trans->trans_ID, trans->card_ID, 
         &trans->time.year, &trans->time.month, &trans->time.day, 
         &trans->time.hour, &trans->time.minute, &trans->time.second, 
         &trans->trans_amount) == 9) {
        return 0;
    }
    
    else {
        return 1;
    }
}

list_t *make_empty_list(void) {
    /* Taken from listops.c creates an empty linked list */
	list_t *list;
	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
    
	list->head = list->foot = NULL;
	return list;
}

list_t *insert_at_foot(list_t *list, transaction_t value) {
    /* Copied from listops.c, this function adds the transaction to the 
       end of the linked list */
    node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
    
	new->data = value;
	new->next = NULL;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		list->foot->next = new;
		list->foot = new;
	}
    return list;
}

transaction_t get_head(list_t *list) {
    /* Taken from listops.c, returns the transaction info (all of it) */
	assert(list!=NULL && list->head!=NULL);
	return list->head->data;
}

list_t *get_tail(list_t *list) {
    /* Copied in from listops.c, removes the head of the list, and frees the 
       memory */
	node_t *oldhead;
	assert(list!=NULL && list->head!=NULL);
	oldhead = list->head;
	list->head = list->head->next;
	if (list->head==NULL) {
		/* the only list node just got deleted */
		list->foot = NULL;
	}
	free(oldhead);
	return list;
}

void print_trans(node_t *node) {
    /* Prints trans_IDs in the linked list from Stage 3 */
    
    while (node != NULL) {
        printf("%s\n", node->data.trans_ID);
        node = node->next;
    }
    
    printf("\n");
    
}

char* print_limit_check(int check) {
    /* Depending on what the value of check is, return a string describing
       whether or not the transaction fits the limits */
    if (check == 0) {
        return("IN_BOTH_LIMITS");
    }
    else if (check == 1) {
        return("OVER_TRANS_LIMIT");
    }
    else if (check == 2) {
        return("OVER_DAILY_LIMIT");
    }
    else if (check == 3) {
        return("OVER_BOTH_LIMITS");
    }
    else {
        return("ERROR");
    }
}

void money_today(node_t *node, int *curr_day, int *curr_month, 
                 int *curr_year, int *today_money) {
    if (node->data.time.day != *curr_day) {
        *curr_day = node->data.time.day;
        *today_money = node->data.trans_amount;
    }
    else if (node->data.time.month != *curr_month) {
        *curr_month = node->data.time.month;
        *today_money = node->data.trans_amount;
    }
    else if (node->data.time.year != *curr_year) {
        *curr_year = node->data.time.year;
        *today_money = node->data.trans_amount;
    }
    else {
        *today_money += node->data.trans_amount;
    }
}

int check_limits(node_t *node, credit_card_t card, int check, 
                 int today_money) {
    /* Checks if the limits are met between a card and a transaction
       Used in Stage 4 */
    
    /* Check if over trans limit */
    if (node->data.trans_amount > card.trans_limit) {
        check++;
    }
                
    /* Check if over daily limit */
    
    if (today_money > card.daily_limit) {
        check = check + 2;
    }
    return(check);
}

void non_binary_method(list_t *trans_list, credit_card_t cards[], 
                       int num_cards) {
    /* Solves Stage 4 with a time complexity of O(mn) */
    node_t *node=trans_list->head;
    
    int curr_day=node->data.time.day, curr_month=node->data.time.month;
    int curr_year=node->data.time.year, today_money=0;
    
    while (node != NULL) {
        int check=0;
        
        /* Checks how much money has been used this day */
        money_today(node, &curr_day, &curr_month, &curr_year, &today_money);
        
        /* Haven't used binary search, instead just iterates through the 
           entire cards list every time */
        for (int i=0; i<num_cards; i++) {
            if (strcmp(node->data.card_ID, cards[i].card_ID) == 0) {
                /* Checks limits */
                check = check_limits(node, cards[i], check, today_money);
            }
        }
        
        /* Print the line and then iterate */
        printf("%s%s%s\n", node->data.trans_ID, SPACES, 
               print_limit_check(check));
        
        node = node->next;
    }
}

/****************************************************************/

/* Stage Functions - algorithms are fun! */

/* Reads a credit card and prints the output */
void stage_one(credit_card_t *card) {
    /* print stage header */
	print_stage_header(STAGE_NUM_ONE);
    
    /* Read the credit card records */
    read_one_card(card, CARD_ID_LENGTH);
    
    /* Print the credit card records */
    print_card(*card);
    
}

/* Reads all the credit card records, prints output */
void stage_two(credit_card_t cards[], int *num_cards) {
    /* print stage header */
	print_stage_header(STAGE_NUM_TWO);
    
    /* Fill the array "cards" with all the credit card details */
    set_cards(cards, num_cards);
    
    /* Finding average daily transactions and card with largest trans limit */
    read_card_records(cards, num_cards);
    
}

/* Reads the transactions, prints their IDs */
void stage_three(list_t *trans_list) {
    /* print stage header */
	print_stage_header(STAGE_NUM_THREE);
    
    /* Creates the linked list */
    transaction_t curr_trans;
    while (read_one_trans(&curr_trans) == 0) {
        insert_at_foot(trans_list, curr_trans);
    }
    
    /* Print the transaction IDs of the linked list */
    print_trans(trans_list->head);
    
}

/* Checks for fraudulent transactions */
void stage_four(list_t *trans_list, credit_card_t cards[], int num_cards) {
    /* print stage header */
	print_stage_header(STAGE_NUM_FOUR);
    
    /* for each transaction, find the matching card and check both limits */
    non_binary_method(trans_list, cards, num_cards);
    
}

/* The average time complexity of the algorithm is O(nm), with n being the 
   number of transactions, and m being the number of credit card records.
   
   This is the case as the algorithm goes through all the transactions in a 
   loop (giving O(n), which is what is wanted), and for all the transactions, 
   the cards list is iterated through in full to find the credit card that 
   matches. In the worst case scenario, assuming that all the credit card IDs 
   in the transaction records are present in the credit cards list, the 
   correct card will always be last, giving a time complexity of O(mn). The 
   average case would see the correct card being in the middle, which would 
   give a time complexity of O(mn/2), which is the same as O(mn) anyways 
*/