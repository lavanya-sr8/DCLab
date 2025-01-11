#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct{
    int pID;
    bool isAlive;
}process;

#define MAX_PROC 20

process coord = {-1, false};
process arr[MAX_PROC];

void sendCoordinatorMessage(int n){
    for(int i=0; i<n; i++){
        if(arr[i].pID<coord.pID){
            printf("Coordinator message sent from process %d to process %d\n", coord.pID, arr[i].pID);
        }
    }
}

void beginElection(int n, int init) {
    printf("Process %d initiates election\n", arr[init].pID);

    for (int i = 0; i < n; i++) {
        if (arr[i].pID > arr[init].pID) {
            printf("Sending election message from process %d to process %d\n", arr[init].pID, arr[i].pID);
        }
    }

    bool higherAlive = false;
    for (int i = 0; i < n; i++) {
        if (arr[i].pID > arr[init].pID && arr[i].isAlive) {
            printf("Alive message received from process %d by process %d\n", arr[i].pID, arr[init].pID);
            higherAlive = true;
        }
    }

    if (higherAlive) {
        for (int i = 0; i < n; i++) {
            if (arr[i].pID > arr[init].pID && arr[i].isAlive) {
                beginElection(n, i);  
            }
        }
    } else {
        coord.pID = arr[init].pID;
        coord.isAlive = true;
        printf("Process %d has the highest pID and is hence elected as the coordinator\n", coord.pID);
        sendCoordinatorMessage(n);
    }
}

void markAlive(int n, int id){
    for(int i=0; i<n; i++){
        if(arr[i].pID == id){
            if(!arr[i].isAlive){
                arr[i].isAlive = true;
                printf("Marked process %d as alive\n", id);
                printf("Initiating coordinator election from process %d...\n",id);
                beginElection(n,i);
                return;
            }else{
                printf("The process is already alive!\n");
                return;
            }
        }
    }

    printf("Specified process %d does not exist\n", id);
}

void markCrashed(int n, int id){
    for(int i=0; i<n; i++){
        if(arr[i].pID == id){
            if(arr[i].isAlive){
                arr[i].isAlive = false;
                printf("Marked process %d as crashed\n", id);
                if(arr[i].pID == coord.pID)
                    coord.isAlive = false;
                return;
            }else{
                printf("The process is already crashed!\n");
                return;
            }
        }
    }

    printf("Specified process %d does not exist\n",id);
}

void showCoord(int n){
    if(coord.pID == -1){
        printf("No coordinator has currently been elected\n");
        printf("Electing coordinator from the first process...\n");
        beginElection(n, 0);
    }
    
    printf("Process with pID %d is the coordinator at present\n",coord.pID);
}

void sendRequestToCoord(int n, int id){
    for(int i=0; i<n; i++){
        if(arr[i].pID == id){
            if(arr[i].isAlive){
                for(int j=0; j<n; j++){
                    if(arr[j].pID == coord.pID){
                        if(arr[j].isAlive){
                            printf("Request has been sent to coordinator from process %d\n",id);
                        }else{
                            printf("Coordinator hasn't responded, so process %d initiates election\n",id);
                            beginElection(n,i);
                        }
                    return;
                    }
                }
            }else{
                printf("The requesting process %d is itself not alive and hence cannot send the request to the coordinator\n",id);
            }
        }
    }
}

int main(){
    int n;
    printf("Enter the number of processes: ");
    scanf("%d",&n);
    for(int i=0; i<n; i++){
        printf("Enter the ID of the process: ");
        scanf("%d",&arr[i].pID);
        arr[i].isAlive = true;
        if(coord.pID<arr[i].pID){
            coord.pID = arr[i].pID;
            coord.isAlive = true;
        }
    }

    process temp;

    for(int i=0; i<n-1; i++){
        for(int j=0; j<n-i-1; j++){
            if(arr[j].pID>arr[j+1].pID){
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }

    printf("\nMENU:\n\n");
    printf("1. Make process alive\n");
    printf("2. Make process crash\n");
    printf("3. Show current coordinator\n");
    printf("4. Send request message\n");
    printf("5. Exit program\n\n");

    int choice;
    do{
        printf("\nEnter your choice: ");
        scanf("%d",&choice);
        int id;
        switch(choice){
            case 1:{
                printf("Enter the pID to be marked as alive: ");
                scanf("%d",&id);
                markAlive(n,id);
                break;
            }
            case 2:{
                printf("Enter the pID to be marked as crashed: ");
                scanf("%d",&id);
                markCrashed(n,id);
                break;
            }
            case 3:{
                showCoord(n);
                break;
            }
            case 4:{
                printf("Enter the pID of the process from which you would like to send request message to coordinator: ");
                scanf("%d",&id);
                sendRequestToCoord(n,id);
                break;
            }
            case 5:{
                printf("Exiting the program...\n");
                break;
            }
            default:
                printf("Choice is invalid. Enter a valid one from the menu\n");
        }
    }while(choice!=5);
    return 0;
}