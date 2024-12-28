#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct process{
    int pID;
    bool isAlive;
    struct process* next;
}process;

process* coord = NULL;
process* head = NULL;


void insertProcess(int id){
    process* proc = (process*)malloc(sizeof(process));
    proc->pID = id;
    proc->isAlive = true;
    if(head==NULL){
        head = proc;
    }else{
        process* temp = head;
        while(temp->next!=head){
            temp = temp->next;
        }
        temp->next = proc; 
    }
    proc->next = head;
}

void beginElection(int initID) {
    if (!head) {
        printf("No processes exist in the system.\n");
        return;
    }

    process* temp = head;
    process* highest = NULL;  
    bool foundInit = false;

    do {
        if (temp->pID == initID && temp->isAlive) {
            foundInit = true;
            break;
        }
        temp = temp->next;
    } while (temp != head);

    if (!foundInit) {
        printf("Initiating process with ID %d is not alive or does not exist.\n", initID);
        return;
    }

    temp = head;
    do {
        if (temp->isAlive) {
            if (!highest || temp->pID > highest->pID) {
                highest = temp;
            }
        }
        temp = temp->next;
    } while (temp != head);

    if (highest) {
        coord = highest;
        printf("Election completed. New coordinator is process with ID %d.\n", coord->pID);
    } else {
        printf("No alive processes found. Coordinator cannot be elected.\n");
    }
}

void markAlive(int id){
    process* temp = head;
    if(temp->pID==id){
        if(temp->isAlive==false)
            temp->isAlive = true;
        else
            printf("Process with ID %d is already alive!\n",id);
    }else{
        temp = temp->next;
        while(temp!=head){
            if(temp->pID == id){
                if(temp->isAlive==false){
                    printf("Marked process with ID %d as alive\n",id);
                    temp->isAlive = true;
                }else{
                    printf("Process with ID %d is already alive!\n",id);
                }
                beginElection(temp->pID);
                break;
            }
            temp = temp->next;
        }
    }

    if(coord->pID == id && coord->isAlive){
        coord->isAlive = false;
    }

    if(temp->isAlive)
        printf("Specified process with ID %d does not exist\n",id);
}

void markCrashed(int id) {
    if (!head) {
        printf("The process list is empty.\n");
        return;
    }

    process* temp = head;
    bool found = false;

    do {
        if (temp->pID == id) {
            found = true;
            if (temp->isAlive) {
                temp->isAlive = false;
                printf("Marked process with ID %d as crashed.\n", id);

                if (coord && coord->pID == id) {
                    coord->isAlive = false;
                    
                }
            } else {
                printf("Process with ID %d is already crashed.\n", id);
            }
            break;
        }
        temp = temp->next;
    } while (temp != head);

    if (!found) {
        printf("Specified process with ID %d does not exist.\n", id);
    }
}

void sendRequestToCoord(int id) {
    if (!head) {
        printf("No processes exist in the system.\n");
        return;
    }

    process* req = NULL;
    process* temp = head;

    do {
        if (temp->pID == id) {
            req = temp;
            break;
        }
        temp = temp->next;
    } while (temp != head);

    if (!req) {
        printf("Requesting process with ID %d does not exist.\n", id);
        return;
    }

    if (!req->isAlive) {
        printf("Requesting process with ID %d is not alive and cannot send a request.\n", id);
        return;
    }

    if (coord && coord->isAlive) {
        printf("Successfully sent request from process with ID %d to coordinator with ID %d.\n", id, coord->pID);
    } else {
        printf("Coordinator is not alive. Process with ID %d initiates election.\n", id);
        beginElection(id);
    }
}

void showCoord(){
    if(!coord){
        printf("No coordinator has currently been elected\n");
        printf("Electing coordinator...\n");
        beginElection(head->pID);
    }

    printf("Process with ID %d is the coordinator at present\n\n",coord->pID);
}

int main(){
    int n;
    int id;
    printf("Enter the number of processes: ");
    scanf("%d",&n);
    for(int i=0; i<n; i++){
        printf("Enter the pID of the process: ");
        scanf("%d",&id);
        insertProcess(id);
    }

    int choice;

    do{
        printf("\n\nMENU:\n");
        printf("1. Make a process alive\n");
        printf("2. Make a process crash\n");
        printf("3. Send request message to coordinator\n");
        printf("4. Show current coordinator\n");
        printf("5. Exit the program\n");
        printf("\nEnter your choice: ");
        scanf("%d",&choice);
        switch(choice){
            case 1:
                printf("Enter the ID of the process to be made alive: ");
                scanf("%d",&id);
                markAlive(id);
                break;
            case 2:
                printf("Enter the ID of the process to be crashed: ");
                scanf("%d",&id);
                markCrashed(id);
                break;
            case 3:
                printf("Enter the ID of the process from which you'd like to send a request message to the coordinator: ");
                scanf("%d",&id);
                sendRequestToCoord(id);
                break;
            case 4:
                showCoord();
                break;
            case 5:
                printf("Exiting the program...\n");
                exit(0);
                break;
            default:
                printf("Invalid choice given as input. Choose a valid choice from the menu\n\n");
        }
    }while(choice!=5);
    return 0;
}