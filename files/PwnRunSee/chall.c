#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFLEN 64
#define MAX_TICKET 16
#define MAX_AGENT_TICKET 4
#define BANNER " _____ _      _        _   _             \n|_   _(_)    | |      | | (_)            \n  | |  _  ___| | _____| |_ _ _ __   __ _ \n  | | | |/ __| |/ / _ \\ __| | '_ \\ / _` |\n  | | | | (__|   <  __/ |_| | | | | (_| |\n  \\_/ |_|\\___|_|\\_\\___|\\__|_|_| |_|\\__, |\n                                    __/ |\n   ---  Reynholm Industries  ---   |___/ \n\n"

typedef struct Agent_t {
    char name[12];  // the name of the agent
    char service[4];  // the service to which the agent belongs (basically ADM/USR)
    void (*run_task)();
    char task[12];  // what the agent is able to do?
    int ticket_count;  // how many tickets this agent has processed?
    struct Agent_t *predecessor;  // which agent has joined the company before him?
    struct Agent_t *successor;  // which agent has joined the company after him?
} Agent_t;

typedef struct Ticket_t {
    char from[12];  // which user has posted this ticket?
    char to[4];  // which service has been assigned to this ticket?
    char description[24];  // task description
} Ticket_t;

// Genesis
Agent_t *founder;
Ticket_t *tickets[MAX_TICKET] = {0};

// Run admin task if we're admin.
void run_task(Agent_t *agent, Ticket_t *ticket) {
    if (strcmp(agent->service, "ADM") == 0) {
        if (strcmp(agent->name, "Moss") == 0 || agent->ticket_count < 1) {  // don't accept commands if it's its first ticket!
            printf("Sorry %s, but you're really dangerous... I'm calling the 01189998819991197253!\n", agent->name);
        } else {
            printf("%s agent is processing the task...\n", agent->name);

            char *argv[] = {agent->task, ticket->description, NULL};
            execve(agent->task, argv, NULL);
        }
    } else {
        printf("%s agent isn't admin!\n", agent->name);
    }
}

// Update ticket content.
void update_ticket(Ticket_t *ticket, char *from, char *to, char *description) {
    strncpy(ticket->from, from, 12);
    strncpy(ticket->to, to, 4);
    strncpy(ticket->description, description, 24);
}

// Create a new ticket and add it to the ticket list.
void new_ticket(char *from, char *to, char *description) {
    int ticket_id = -1;
    Ticket_t *ticket = NULL;
    int i = 0;

    while (i < MAX_TICKET) {
        if (tickets[i] == NULL) {
            ticket_id = i;
            break;
        }

        i++;
    }

    if (ticket_id != -1) {
        tickets[ticket_id] = malloc(sizeof(Ticket_t));
        ticket = tickets[ticket_id];

        update_ticket(ticket, from, to, description);
    } else {
        printf("Our agents are too busy...\n");
    }
}

void show_ticket(Ticket_t *ticket) {
    printf("======== TICKET [%p] ========\n", ticket);
    printf("From: %s\n", ticket->from);
    printf("To: %s\n", ticket->to);
    printf("Description: %s\n", ticket->description);
    printf("====================================\n");
}

void list_tickets() {
    int ticket_id = 0;
    Ticket_t *ticket = NULL;

    ticket = tickets[ticket_id];
    while (ticket != NULL && ticket_id < MAX_TICKET) {
        show_ticket(ticket);

        ticket_id++;
        ticket = tickets[ticket_id];
    }
}

// Update agent info.
void update_agent(Agent_t *agent, char *name, char *service, char *task) {
    strncpy(agent->name, name, 12);
    strncpy(agent->service, service, 4);
    strncpy(agent->task, task, 12);
    agent->ticket_count = 0;
    agent->run_task = run_task;
}

// Create a new agent.
void new_agent(char *name, char *service, char *task) {
    Agent_t *agent = NULL;
    Agent_t *new_agent = malloc(sizeof(Agent_t));

    agent = founder;
    while (agent->successor != NULL) {
        agent = agent->successor;
    }
    agent->successor = new_agent;
    new_agent->predecessor = agent;
    new_agent->successor = NULL;

    update_agent(new_agent, name, service, task);
}

void show_agent(Agent_t *agent) {
    printf("======== AGENT [%p] ========\n", agent);
    printf("Name: %s\n", agent->name);
    printf("Service: %s\n", agent->service);
    printf("Task: %s\n", agent->task);
    printf("Ticket count: %d\n", agent->ticket_count);
    printf("Run task: %p\n", agent->run_task);
    printf("====================================\n");
}

void list_agents() {
    Agent_t *agent = NULL;

    agent = founder;
    while (agent != NULL) {
        show_agent(agent);
        agent = agent->successor;
    }
}

// Create the founder and the agent(s).
void build_company() {
    founder = malloc(sizeof(Agent_t));

    update_agent(founder, "The Founder", "DIR", "/bin/false");
    new_agent("Moss", "ADM", "/bin/cat");
}

Agent_t *find_agent(char *service) {
    Agent_t *found_agent = NULL;
    Agent_t *agent = NULL;

    agent = founder;
    while (agent != NULL && found_agent == NULL) {
        if (strcmp(agent->service, service) == 0 && agent->ticket_count < MAX_AGENT_TICKET) {
            found_agent = agent;
        } else {
            agent = agent->successor;
        }
    }

    return found_agent;
};

// Interns aren't really useful isn't it?
void fire_interns() {
    Agent_t *agent = NULL;
    Agent_t *next = NULL;

    agent = founder;
    while (agent != NULL) {
        printf("Looking %s for firing\n", agent->name);
        next = agent->successor;
        // TODO: update linked list.
        if (strcmp(agent->name, "Intern") == 0) {
            printf("%s has been fired\n", agent->name);
            free(agent);
        }

        agent = next;
    }
}

// Reset ticket counters for every agent.
void reset_tickets_count() {
    Agent_t *agent = NULL;

    agent = founder;
    while (agent != NULL) {
        agent->ticket_count = 0;
        agent = agent->successor;
    }
}

// Parse tickets and process them.
void process_tickets() {
    int ticket_id = 0;
    Ticket_t *ticket = NULL;
    Agent_t *agent = NULL;

    ticket = tickets[ticket_id];
    while (ticket != NULL && ticket_id < MAX_TICKET) {
        agent = find_agent(ticket->to);

        if (agent == NULL) {
            printf("Your ticket can't be processed because our agents are overwhelmed by work :/ We're recruiting an intern!\n");
            new_agent("Intern", "ADM", "/bin/false");
        } else {
            printf("The following ticket will be processed by %s!\n", agent->name);

            show_ticket(ticket);

            agent->run_task(agent, ticket);

            free(tickets[ticket_id]);

            agent->ticket_count += 1;
        }

        tickets[ticket_id] = NULL;
        ticket_id++;
        ticket = tickets[ticket_id];
    }

    fire_interns();
}

// Get string from standard input.
void fget_str(const char message[], char *buf, const int max_buf_size) {
    char junk_char;

    printf("%s", message);

    /* Fill the buffer with user input and then convert the buffer to char. */
    if (fgets(buf, max_buf_size, stdin) == NULL) {
        printf("Enter a string!");
    }

    /* Eventually flush the input buffer in order to prevent overflow issues. */
    if (buf[strlen(buf) - 1] != 0xa) {  /* Use strlen as index to handle non-full buffer. */
        while ((junk_char = getchar()) != 0xa && junk_char != EOF) {
            /* printf("Flushing! Removing 0x%x from input.\n", junk_char); */
        }
    } else {
        buf[strlen(buf) - 1] = 0x0;
    }
}

// Get int from standard input.
int fget_int(const char message[], char *buf, const int max_buf_size) {
    int integer, finished = 0;
    char junk_char;

    do {
        printf("%s", message);

        /** Fill the buffer with user input and then convert the buffer to integer. */
        if (fgets(buf, max_buf_size, stdin) != NULL) {
            if (sscanf(buf, "%d", &integer) != 1) {  /* Buffer can't be converted to integer. */
                fprintf(stderr, "Please enter a valid integer!\n");
            } else {
                finished = 1;
            }
        }

        if (buf[strlen(buf) - 1] != 0xa) {  /** Use strlen as index to handle non-full buffer. */
            while ((junk_char = getchar()) != 0xa && junk_char != EOF) {
                /** printf("Flushing! Removing 0x%x from input.\n", junk_char); */
            }
        }
    } while (!finished);

    return integer;
}

// Create ticket.
void create_ticket() {
    char from[12] = {0};
    char to[4] = {0};
    char description[24] = {0};

    fget_str("Your name: ", from, sizeof(from));
    fget_str("The destination service: ", to, sizeof(to));
    fget_str("Description: ", description, sizeof(description));

    new_ticket(from, to, description);
}

int main(){
    int choice;
    char buf[sizeof(int)];

    setvbuf(stdout, NULL, _IONBF, 0);  // stdout is unbuffered.
    setvbuf(stderr, NULL, _IONBF, 0);  // stderr is unbuffered.

    build_company();

    do {
        printf(BANNER);
        printf("1 - Show company agents\n");
        printf("2 - Show tickets\n");
        printf("3 - Create ticket\n");
        printf("4 - Ask the agents to process tickets\n");
        printf("0 - Leave\n");
        choice = fget_int("Your choice:\n=> ", buf, sizeof(buf));

        switch (choice) {
            case 1:
                list_agents();
                break;
            case 2:
                list_tickets();
                break;
            case 3:
                create_ticket();
                break;
            case 4:
                process_tickets();
                break;
            case 0:
                break;
            default:
                printf("Wrong choice!\n");
        }
    } while (choice != 0);

    return EXIT_SUCCESS;
}