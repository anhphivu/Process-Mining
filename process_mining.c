/* Program to discover a process model from an event log.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  August 2022, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: [Anh Phi Vu ; Student ID - 1266276]
  Dated:     [26/09/2022]

*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <math.h>

/* #DEFINE'S -----------------------------------------------------------------*/
#define MAX_LOG_CAPACITY 1000                  // Maximum event log capacity
#define CHAR_SEPERATOR ',' 
#define MAX_LINE_LENGTH 1000
#define MAX_DISTINCT_ACTION 1000
#define MAX_DISTINCT_TRACES 1000
#define MAX_ARRAY_DIMENSION 1024

/* TYPE DEFINITIONS ----------------------------------------------------------*/
typedef unsigned int action_t;  // an action is identified by an integer

struct pattern {                // a tuple type struct for the seq pantern
    int a;
    int b;
    int type;                   // 0 - SEQ; 1 - CON; 2 - CHC; -1 - ERROR
};

typedef struct event event_t;   // an event ...
struct event {                  // ... is composed of ...
    action_t actn;              // ... an action that triggered it and ...
    event_t* next;              // ... a pointer to the next event in the trace
};

typedef struct {                // a trace is a linked list of events
    event_t* head;              // a pointer to the first event in this trace
    event_t* foot;              // a pointer to the last event in this trace
    int      freq;              // the number of times this trace was observed
} trace_t;

typedef struct {                // an event log is an array of distinct traces
                                //     sorted lexicographically
    trace_t* trcs;              // an array of traces
    int      ndtr;              // the number of distinct traces in this log
    int      cpct;              // the capacity of this event log as the number
                                //     of  distinct traces it can hold
} log_t;

typedef action_t** DF_t;        // a directly follows relation over actions

/* FUNCTIONS DECLARATION -----------------------------------------------------*/
event_t *create_event(action_t action);
event_t *add_to_event(event_t * evnt, action_t action);
event_t *line_to_event(char *input_line);
event_t *get_end(event_t *evnt);
trace_t *create_trace(event_t *event);
log_t   *create_log();
log_t   *event_to_log(event_t *event, log_t *log);

struct pattern get_seq_pattern(int **sup_matrix, int **pd_matrix, 
                               int **w_matrix, action_t *actions, int length);

struct pattern get_pattern(int **sup_matrix, int **pd_matrix, int **w_matrix, 
                           action_t *actions, int length, int N);

int max(int x, int y);
int my_getline(char **line, int line_len, FILE *stream);
int cmp_events(event_t *event1, event_t *event2);
int get_distinct_event (log_t *log, action_t **ret);
int in_array(action_t action, action_t *array, int length);
int get_num_event (log_t *log);
int get_num_trace (log_t *log);
int get_num_action (log_t *log, action_t action);
int get_most_freq_traces (log_t *log, trace_t **most_freq_trace);
int **create_matrix(action_t *action, int length);
int cmp_func(const void * a, const void * b);
int compute_pd(int x, int y);
int abstract_pattern(log_t *log, struct pattern pattern, int abstraction);

void log_to_sup_matrix (log_t *log, int **matrix, int length);
void sup_to_pd_matrix (int **sup_matrix, int **pd_matrix, 
                       action_t *actions, int length);
void create_w_matrix (int **w_matrix, int **sup_matrix, 
                      int **pd_matrix, action_t *actions, int length);
void print_action(action_t action);
void print_event (event_t *e);
void print_trace(trace_t *t);
void print_log(log_t *l);
void print_matrix(int **matrix, int length, action_t *actions);
void free_event (event_t *e);
void free_trace (trace_t *t);
void free_log (log_t *l);
void free_trace_array (trace_t *t, int length);
void free_matrix (int **matrix, int length); 

/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
int main(int argc, char *argv[]) {
    int line_len = MAX_LINE_LENGTH;
    char *line = NULL;
    //create log
    log_t *log = create_log();
    
    // READ INPUT
    while (my_getline(&line, line_len, stdin) > 0) { 
        //add line to log
        event_t *e = line_to_event(line);
        event_to_log(e, log);
        free(line);
    }

    // STAGE 0
    action_t *distinct_events = NULL;
    trace_t *most_freq_traces = NULL;
    int num_most_freq_traces = get_most_freq_traces(log, &most_freq_traces);
    int num_distinct_event = get_distinct_event(log, &distinct_events);
    printf("==STAGE 0============================\n");
    printf("Number of distinct events: %d\n", num_distinct_event);
    printf("Number of distinct traces: %d\n", log -> ndtr);
    printf("Total number of events: %d\n", get_num_event(log)); 
    printf("Total number of traces: %d\n", get_num_trace(log));
    printf("Most frequent trace frequency: %d\n", most_freq_traces[0].freq);
    for (int i = 0; i < num_most_freq_traces; i++) {
        print_event(most_freq_traces[i].head);
    }
    for (int i = 0; i < num_distinct_event; i++) {
        printf("%c = %d\n", distinct_events[i], 
        get_num_action(log, distinct_events[i]));
    }

    // STAGE 1
    printf("==STAGE 1============================\n");
    int **sup_matrix = NULL;
    int **pd_matrix = NULL;
    int **w_matrix = NULL;
    int num_abstract = 256; 
    int firsttime = 1;

    while (1) {
        sup_matrix = create_matrix(distinct_events, num_distinct_event);
        pd_matrix = create_matrix(distinct_events, num_distinct_event);
        w_matrix = create_matrix(distinct_events, num_distinct_event);
        log_to_sup_matrix(log, sup_matrix, MAX_ARRAY_DIMENSION);

        sup_to_pd_matrix(sup_matrix, pd_matrix, 
                         distinct_events, num_distinct_event);
        create_w_matrix(w_matrix, sup_matrix, pd_matrix, 
                        distinct_events, num_distinct_event);
        struct pattern pattern = get_seq_pattern(sup_matrix, pd_matrix, 
                            w_matrix, distinct_events, num_distinct_event);

        if (pattern.a < 0) {
            break;
        }
        if (!firsttime) {
            printf("=====================================\n");
        }
        firsttime = 0;
        print_matrix(sup_matrix, num_distinct_event, distinct_events);
        printf("-------------------------------------\n");
        printf("%d = SEQ(", num_abstract);
        print_action(pattern.a);
        printf(",");
        print_action(pattern.b);
        printf(")\n");
        printf("Number of events removed: %d\n", 
                abstract_pattern(log, pattern, num_abstract));
        free(distinct_events);
        num_distinct_event = get_distinct_event(log, &distinct_events);
        for (int i = 0; i < num_distinct_event; i++) {
            print_action(distinct_events[i]);
            printf(" = %d\n", get_num_action(log, distinct_events[i]));
        }
        num_abstract++;
    }

    // STAGE 2
    printf("==STAGE 2============================\n");
    firsttime = 1;
    while (1) {
        sup_matrix = create_matrix(distinct_events, num_distinct_event);
        pd_matrix = create_matrix(distinct_events, num_distinct_event);
        w_matrix = create_matrix(distinct_events, num_distinct_event);
        log_to_sup_matrix(log, sup_matrix, MAX_ARRAY_DIMENSION);

        sup_to_pd_matrix(sup_matrix, pd_matrix, 
                         distinct_events, num_distinct_event);
        create_w_matrix(w_matrix, sup_matrix, pd_matrix, 
                        distinct_events, num_distinct_event);
        
        int N = get_num_event(log);
        struct pattern pattern = get_pattern(sup_matrix, pd_matrix, 
        w_matrix, distinct_events, num_distinct_event, N);

        if (pattern.a < 0) {
            break;
        }
        if (!firsttime) {
            printf("=====================================\n");
        }
        firsttime = 0;
        print_matrix(sup_matrix, num_distinct_event, distinct_events);
        printf("-------------------------------------\n");
        
        if (pattern.type == 0) {
            printf("%d = SEQ(", num_abstract); 
        }
        if (pattern.type == 1) {
            printf("%d = CON(", num_abstract);
        }
        if (pattern.type == 2) {
            printf("%d = CHC(", num_abstract);
        }

        print_action(pattern.a);
        printf(",");
        print_action(pattern.b);
        printf(")\n");

        printf("Number of events removed: %d\n", 
                abstract_pattern(log, pattern, num_abstract));
        free(distinct_events);
        num_distinct_event = get_distinct_event(log, &distinct_events);
        for (int i = 0; i < num_distinct_event; i++) {
            print_action(distinct_events[i]);
            printf(" = %d\n", get_num_action(log, distinct_events[i]));
        }
        num_abstract++;
    }
    printf("==THE END============================\n");

    // FREE EVERYTHING
    free_matrix(sup_matrix, num_distinct_event);
    free_matrix(pd_matrix, num_distinct_event);
    free_matrix(w_matrix, num_distinct_event);
    free_log(log);
    free(distinct_events);
    for (int i = 0; i < num_most_freq_traces;i++) {
        free_event((most_freq_traces[i]).head);
    }
    free(most_freq_traces);
    return EXIT_SUCCESS;       
}

/* Create an event of length 1. Put an action inside an event */
event_t *create_event(action_t action) {
    event_t *ret = (event_t *)malloc(sizeof(event_t));
    if (ret == NULL) {
        printf("Creat_event_error");
        return NULL;
    }
    ret -> actn = action;
    ret -> next = NULL;
    return ret;
}

/* Adds an action to the end of an event */
event_t *add_to_event(event_t * evnt, action_t action) {
    if (evnt == NULL) {
        return create_event(action);
    }
    while (evnt -> next != NULL) {
        evnt = evnt -> next;
    }
    evnt -> next = create_event(action);
    return evnt -> next;
}

/* Converting an input line into an event */ 
event_t *line_to_event(char *input_line) {
    int i;
    event_t *event;

    if (strlen(input_line) <= 0) {
        return NULL;
    }

    event = create_event(input_line[0]);

    for (i = 1; i < strlen(input_line); i++) {
        if (input_line[i - 1] == CHAR_SEPERATOR) {
            add_to_event(event,input_line[i]);
        }
    }
    return event;
}

/* Create a trace from an event*/
trace_t *create_trace(event_t *event) {
    trace_t *ret = (trace_t *)malloc(sizeof(trace_t));
     if (ret == NULL) {
        printf("Creat_trace_error");
        return NULL;
    }
    ret -> freq = 1;
    ret -> head = event;
    ret -> foot = get_end(event);

    return ret;
}

/* Create an empty log to append event */
log_t *create_log() {
    log_t *ret = (log_t *)malloc(sizeof(log_t));
     if (ret == NULL) {
        printf("Creat_log_error");
        return NULL;
    }
    ret -> ndtr = 0;
    ret -> cpct = MAX_LOG_CAPACITY;
    ret -> trcs = (trace_t *)malloc(sizeof(trace_t) * ret -> cpct);
    assert((ret -> trcs) != NULL);

    return ret;
}

/* Add event to the log */
log_t *event_to_log(event_t *event, log_t *log) {
    int i;
    for (i = 0; i < (log -> ndtr); i++) {
        trace_t *trace = log -> trcs + i;
        if (cmp_events(event, trace -> head) == 0) {
            (trace -> freq)++;
            return log;
        }
        if (cmp_events(event, trace -> head) == 1) {
            for(int j = (log -> ndtr) - 1; j >= i; j--) {
                (log -> trcs)[j + 1] = (log -> trcs)[j];
            }

            trace_t *t = create_trace(event);
            (log -> trcs)[i] = *t;
            free(t);
            log -> ndtr += 1;
            return log;
        }
    }
    trace_t *t  = create_trace(event);
    (log -> trcs)[(log -> ndtr)] = *t;
    log -> ndtr++;
    free(t);
    return log;
}

/* get the final action in a given event*/
event_t *get_end(event_t *evnt) {
    if (evnt == NULL) {
        return NULL;
    }
    while (evnt -> next != NULL) {
        evnt = evnt -> next;
    }
    return evnt;
}

/* get the SEQ pattern from the matrix*/
struct pattern get_seq_pattern(int **sup_matrix, int **pd_matrix, 
int **w_matrix, action_t *actions, int length) {
    int max_w = 0;
    struct pattern ret = {-1, -1, 0};
    for (int i = 0; i < length; i++) {
        action_t x = actions[i];
        for (int j = 0; j < length; j++) {
            action_t y = actions[j];
            if (x == y) {
                continue;
            }
            if (pd_matrix[x][y] <= 70) {
                continue;
            }
            if (x >= 256 || y >= 256) {
                continue;
            }
            if (w_matrix[x][y] > max_w) {
                max_w = w_matrix[x][y];
                ret.a = x; 
                ret.b = y;
            }
        }
    }
    return ret;
}

/* get the pattern(SEQ, CON or CHC) from the matrix */
struct pattern get_pattern(int **sup_matrix, int **pd_matrix, 
int **w_matrix, action_t *actions, int length, int N) {
    int max_w = 0;
    struct pattern ret = {-1, -1, -1};
    for (int i = 0; i < length; i++) {
        action_t x = actions[i];
        for (int j = 0; j < length; j++) {
            int pat = -1;
            int w = 0;
            action_t y = actions[j];
            if (x == y) {
                continue;
            }
            if (max(sup_matrix[x][y], sup_matrix[y][x]) <= N/100) {
                pat = 2;  
                w = N * 100;
            }
            else if (pd_matrix[x][y] < 30){
                pat = 1;
                w =  w_matrix[x][y];
                if (!(x >= 256 || y >= 256)) {
                    w = w * 100;
                }
            }
            else if (pd_matrix[x][y] > 70) {
                pat = 0;
                w =  w_matrix[x][y];
                if (!(x >= 256 || y >= 256)) {
                    w = w * 100;
                }
            }
            if (pat == -1){
                continue;
            }
            if (w > max_w) {
                max_w = w;
                ret.a = x; 
                ret.b = y;
                ret.type = pat;
            }
        }
    }
    return ret;
}

/* abstract the given pattern in to a number */
int abstract_pattern(log_t *log, struct pattern pattern, int abstraction) {
    int num_removed = 0;
    for (int i = 0; i < log -> ndtr; i++) {
        event_t *current = (log -> trcs + i) -> head;
        while (current != NULL) {
            if ((current -> actn == pattern.a) 
            || (current -> actn == pattern.b)) {
                current -> actn = abstraction;
            }
            current = current -> next;
        } 
    }
    for (int i = 0; i < log -> ndtr; i++) {
        event_t *current = (log -> trcs + i) -> head;
        while (current -> next != NULL) {
            if ((current -> actn == abstraction) 
            && (current -> next -> actn == abstraction)) {
                event_t *nextnext = current -> next -> next;
                free(current -> next);
                current -> next = nextnext;
                num_removed += (log -> trcs + i) -> freq;
            } else{
                current = current -> next;
            }
        } 
    }
    return num_removed;
}

/* Read in input from the command prompt*/
int my_getline(char **line, int line_len, FILE *stream) {
    char c = 0;
    int length = 0;
    (*line) = (char*)malloc(sizeof(char) * line_len);
    assert((*line) != NULL);
    while ((c = getc(stream)) != '\n') {
        if (c == EOF) {
            return length;
        }
        (*line)[length] = c;
        (*line)[length + 1] = 0;
        length++;
    }
    return length;
}

/* compare two event in ASCII code*/
int cmp_events(event_t *event1, event_t *event2) {
    while ((event1 != NULL) && (event2 != NULL)) {
        // if event1 < event 2 (ASCII)
        if (event1 -> actn > event2 -> actn) {
            return -1;
        }
        // if event 2 > event 1 (ASCII)
        if (event1 -> actn < event2 -> actn) {
            return 1;
        }
        // if event 1 == event 2 (ASCII), move it to the next event
        event1 = event1 -> next;
        event2 = event2 -> next;
    }
    if ((event1 == NULL) && (event2 != NULL)) {
        return -1;
    }
    if ((event1 != NULL) && (event2 == NULL)) {
        return 1;
    }
    return 0;
}

/* compare function use for call qsort */
int cmp_func(const void * a, const void * b) {
    return (*(int*)a - *(int*)b);
}

/* get the number of distinct event */
int get_distinct_event (log_t *log, action_t **ret) {
    (*ret) = (action_t *)malloc(sizeof(action_t) * MAX_DISTINCT_ACTION);
    assert((*ret) != NULL);
    int length = 0;
    for (int i = 0; i < log -> ndtr; i++) {
        event_t *current = (log -> trcs + i) -> head;
        while (current != NULL) {
            if (!in_array(current -> actn, (*ret), length)){
                (*ret)[length] = current -> actn;
                length++;
            }
            current = current -> next;
        }
    }
    qsort(*ret, length, sizeof(action_t), cmp_func);
    return length;
}

/* get the total number of event */
int get_num_event (log_t *log) {
    int count_event = 0;
    for (int i = 0; i < log -> ndtr; i++) {
        int c = 0;
        event_t *current = (log -> trcs + i) -> head;
        while (current != NULL) {
            c++;
            current = current -> next;
        }
        count_event += (c * ((log -> trcs + i) -> freq));
    }
    return count_event;
}

/* get the total number of traces */
int get_num_trace (log_t *log) {
    int count_trace = 0;
    for (int i = 0; i < log -> ndtr; i++) {
        count_trace += (log -> trcs)[i].freq;
    }
    return count_trace;
}

/* get the most frequent traces */
int get_most_freq_traces (log_t *log, trace_t **most_freq_trace) {
    (*most_freq_trace) = (trace_t *)malloc(sizeof(trace_t) 
    * MAX_DISTINCT_TRACES);
    assert((*most_freq_trace) != NULL);
    int length = 0;
    int most_freq = 0;
    for (int i = 0; i < log -> ndtr; i++) {
        if (most_freq == (log -> trcs)[i].freq) {
            length += 1;
            (*most_freq_trace)[length - 1] = (log -> trcs)[i];
        }
        if (most_freq < (log -> trcs)[i].freq) {
            length = 1;
            (*most_freq_trace)[0] = (log -> trcs)[i];
            most_freq = (log -> trcs)[i].freq;
        }
    }
    return length;
}

/* get the total number of actions */
int get_num_action (log_t *log, action_t action) {
    int count_action = 0;
    for (int i = 0; i < log -> ndtr; i++) {
        int c = 0;
        event_t *current = (log -> trcs + i) -> head;
        while (current != NULL) {
            if (current -> actn == action) {
                c++; 
            }
            current = current -> next;
        }
        count_action += (c * ((log -> trcs + i) -> freq));
    }
    return count_action;
}

/* check if the current action is in the array */
int in_array(action_t action, action_t *array, int length) {
    for (int i = 0; i < length; i++) {
        if (array[i] == action) {
            return 1;
        }  
    }
    return 0;
}

/* create an empty matrix */
int **create_matrix(action_t *action, int length) {
    int **ret = (int **)malloc(sizeof(int*) * MAX_ARRAY_DIMENSION);
    assert(ret != NULL);
    for (int i = 0; i < length; i++) {
        ret[action[i]] = (int *)malloc(sizeof(int) * MAX_ARRAY_DIMENSION);
        assert((ret[action[i]]) != NULL);
    }
    return ret;
}

/* create the sup matrix for a given log */
void log_to_sup_matrix (log_t *log, int **matrix, int length) {
    for (int i = 0; i < log -> ndtr; i++) {
        event_t *current = (log -> trcs + i) -> head;
        while (current != NULL) {
            if (current -> next != NULL) {
                matrix[current -> actn][(current -> next) -> actn] 
                += (((log -> trcs) + i) -> freq);
            }
            current = current -> next;
        }
    }
}

/* transform the sup matrix into pd matrix */
void sup_to_pd_matrix (int **sup_matrix, int **pd_matrix, 
action_t *actions, int length) {
    for (int i = 0; i < length; i++) {
        action_t x = actions[i];
        for (int j = 0; j < length; j++) {
            action_t y = actions[j];
            if ((x != y) && (sup_matrix[x][y] > sup_matrix[y][x])) {
                pd_matrix[x][y] = compute_pd(sup_matrix[x][y], 
                sup_matrix[y][x]); 
            }
            else {
                pd_matrix[x][y] = 0;
            } 
        }
    }
}

/* create the weight matrix from sup and pd matrix */
void create_w_matrix (int **w_matrix, int **sup_matrix, 
int **pd_matrix, action_t *actions, int length) {
    for (int i = 0; i < length; i++) {
        action_t x = actions[i];
        for (int j = 0; j < length; j++) {
            action_t y = actions[j];
            w_matrix[x][y] = abs(50 - pd_matrix[x][y]) 
            * max(sup_matrix[x][y], sup_matrix[y][x]);
        }
    }
}

/* calculate the pd value for a pair of action */
int compute_pd(int x, int y) {
    return (100 * abs(x - y))/(max(x, y)); 
}

/* find the maximum number out of two number */
int max(int x, int y) {
    if (x > y) {
        return x;
    }
    return y;
}

/* print out the matrix */
void print_matrix(int **matrix, int length, action_t *actions) {
    // print header
    printf("     ");
    for (int i = 0; i < length; i++) {
        if (isalpha(actions[i])) {
            printf("%*c", 5, actions[i]);
        }
        else {
            printf("%*d", 5, actions[i]);
        }   
    }
    printf("\n");
    // print the matrix content
    for (int i = 0; i < length; i++) {
        if (isalpha(actions[i])) {
            printf("%*c", 5, actions[i]);
        }
        else {
            printf("%*d", 5, actions[i]);
        }   
        for (int j = 0; j < length; j++) {
            printf("%*d", 5, matrix[actions[i]][actions[j]]);
        }
        printf("\n");
    }
}

/* print out the action */
void print_action(action_t action) {
    if (isalpha(action)) {
            printf("%c",action);
        }
        else {
            printf("%d", action);
        }
}

/* print out the event */
void print_event (event_t *e) {
    while (e != NULL) {
        print_action(e -> actn);
        e = e->next;
    }
    printf("\n");
}

/* print out the trace */
void print_trace(trace_t *t) {
    printf("head: ");
    print_event(t -> head);
    printf("foot: ");
    print_event(get_end(t -> head)); 
    printf("freq: ");
    printf("%d\n", t -> freq);
}

/* print out the log */
void print_log(log_t *l) {
    int i;
    printf("\n---printing log---\n");
    for (i = 0; i < l -> ndtr; i++) {
        printf("\ntrace %d: \n",i);
        print_trace((l -> trcs) + i);
    }
    printf("length = %d\n", l -> ndtr);
}

/* Free the memory allocated for the event */
void free_event (event_t *e) {
    if (e != NULL) {
        free_event(e -> next);
        free(e);
    }
}

/* Free the memory allocated for the trace*/
void free_trace (trace_t *t) {
    if (t != NULL) {
        free_event(t -> head);
        free(t);
    }
}

/* Free the memory allocated for the log */
void free_log (log_t *l) {
    if (l != NULL) {
        free_trace_array(l -> trcs, l -> ndtr);
        free(l);
    }
}

/* Free each trace in the log struct */
void free_trace_array (trace_t *t, int length) {
    int i;
    for (i = 0; i < length; i++) {
    free_event(t[i].head);
    }
    free(t);
}

/* free memory allocated for the matrix */
void free_matrix (int **matrix, int length) {
    if (matrix != NULL) {
        for (int i = 0; i < length; i++) {
            if (matrix[i] != NULL) {
                free(matrix[i]);
            }
        }
        free(matrix);
    }
}

/* algorithms are fun */