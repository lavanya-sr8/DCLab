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

    if(coord==NULL){
        coord = proc;
    }else{
        if(coord->pID<proc->pID)
            coord = proc;
    }
}

void sendCoordMessage(process* init){
    process* ptr = init;
    int senderID = -1, rcvrID = -1;
    do {
        if (ptr->isAlive) {
            if(senderID==-1){
                senderID = ptr->pID;    
            }else{
                rcvrID = ptr->pID;
                if(senderID!=rcvrID)
                    printf("Coordinator message sent from process %d to process %d, indicating that process %d is the coordinator\n",senderID,rcvrID,coord->pID);
                senderID = rcvrID;
            }
        }
        ptr = ptr->next;
    } while (ptr != init);

    printf("Coordinator message sent from process %d to process %d, indicating that process %d is the coordinator\n",senderID,init->pID,coord->pID);
}

void beginElection(int n, int initID) {
    if (!head) {
        printf("No processes exist in the system.\n");
        return;
    }

    process* temp = head;
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

    int* priorities = (int*)malloc(n * sizeof(int));
    process* ptr = head;

    int ctr = -1;
    int senderID = -1;
    int rcvrID = -1;
    int maxPrio = -1;
    process* newCoord = NULL;  

    do {
        if (ptr->isAlive) {
            priorities[++ctr] = ptr->pID;
            if (senderID == -1) {
                senderID = ptr->pID;
            } else {
                rcvrID = ptr->pID;
                printf("Election message sent from process %d to process %d\n", senderID, rcvrID);
                senderID = rcvrID;
            }
            printf("Process %d has appended its priority to the message\n", ptr->pID);

            if (ptr->pID > maxPrio) {
                maxPrio = ptr->pID;
                newCoord = ptr;  
            }
        }
        ptr = ptr->next;
    } while (ptr != head);

    printf("Election message sent from process %d to process %d\n", senderID, temp->pID);

    if (ctr == -1) {
        printf("No process is active, and hence the coordinator cannot be elected\n");
    } else {
        printf("Final message received at the initiator: [ ");
        for(int i=0; i<=ctr; i++){
            printf("%d ",priorities[i]);
        }
        printf("]\n");
        if (newCoord != NULL) {
            coord = newCoord;  
            coord->isAlive = true;  
            printf("Process %d has the maximum priority and is therefore elected as the coordinator\n", coord->pID);
        }

        sendCoordMessage(temp);  
    }
}

void inquiryMessage(process* init){
    process* ptr = init;
    int sendID = -1, rcvrID = -1;

    do{
        if(ptr->isAlive){
            if(sendID==-1){
                sendID = ptr->pID;
            }else{
                rcvrID = ptr->pID;
                printf("Inquiry message sent from process %d to process %d\n",sendID,rcvrID);
                sendID = rcvrID;
            }
        }
        ptr = ptr->next;
    }while(ptr->pID!=coord->pID);
    printf("Inquiry message sent from process %d to process %d\n",sendID,coord->pID);
    printf("Process %d is the coordinator and sends reply message to process %d\n",coord->pID,init->pID);
}

void markAlive(int id) {
    process* temp = head;
    bool foundProc = false;

    do {
        if (temp->pID == id) {
            foundProc = true;
            if (!temp->isAlive) {
                printf("Marked process with ID %d as alive\n", id);
                temp->isAlive = true;
            } else {
                printf("Process with ID %d is already alive!\n", id);
            }
            printf("Sending inquiry message to identify current coordinator...\n");
            inquiryMessage(temp);
            break;
        }
        temp = temp->next;
    } while (temp != head);


    if (!foundProc) {
        printf("Specified process with ID %d does not exist\n", id);
    }
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

void sendRequestToCoord(int n, int id) {
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
        beginElection(n,id);
    }

}

void showCoord(int n){
    if(!coord){
        printf("No coordinator has currently been elected\n");
        printf("Electing coordinator...\n");
        beginElection(n,head->pID);
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

    printf("\n\nMENU:\n");
    printf("1. Make a process alive\n");
    printf("2. Make a process crash\n");
    printf("3. Send request message to coordinator\n");
    printf("4. Show current coordinator\n");
    printf("5. Exit the program\n");

    do{
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
                sendRequestToCoord(n,id);
                break;
            case 4:
                showCoord(n);
                break;
            case 5:
                printf("Exiting the program...\n");
                break;
            default:
                printf("Invalid choice given as input. Choose a valid choice from the menu\n\n");
        }
    }while(choice!=5);
    return 0;
}