#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_JOBS 100
#define MAX_NODES 5
#define NAME_SIZE 50
#define GROUP_SIZE 50

/* =========================
   STRUCTURE DEFINITIONS
   ========================= */

/* Nested structure: GPU Node */
struct GPUNode
{
    int nodeId;
    char nodeName[NAME_SIZE];
    float vramGB;
    float totalHours;
    float allocatedHours;
};

/* Nested structure: Research Group */
struct ResearchGroup
{
    int groupId;
    char groupName[GROUP_SIZE];
    char leaderName[NAME_SIZE];
};

/* Main Job structure containing nested structures */
struct Job
{
    int jobId;
    char jobName[NAME_SIZE];
    int priority;
    float gpuHours;
    float requiredVRAM;

    struct ResearchGroup group;
    struct GPUNode node;

    char status[20];
};

/* =========================
   FUNCTION PROTOTYPES
   ========================= */

void initializeNodes(struct GPUNode *nodes, int *nodeCount);
void displayNodes(struct GPUNode *nodes, int nodeCount);

int generateJobId(void);
void synchronizeJobCounter(int nextId);

int searchJob(struct Job *jobs, int jobCount, int jobId);

int checkDuplicateJobId(struct Job *jobs, int jobCount, int jobId);

int checkCapacity(struct Job *job, struct GPUNode *node);

void addJob(struct Job *jobs, int *jobCount,
            struct GPUNode *nodes, int nodeCount);

void updateJob(struct Job *jobs, int jobCount,
               struct GPUNode *nodes, int nodeCount);

void displayJobs(struct Job *jobs, int jobCount);

void searchAndDisplayJob(struct Job *jobs, int jobCount);

void sortJobsByPriority(struct Job *jobs, int jobCount);

void checkNodeCapacity(struct Job *jobs, int jobCount,
                       struct GPUNode *nodes, int nodeCount);

void analyseNodeUtilization(struct GPUNode *nodes, int nodeCount);

void saveJobs(struct Job *jobs, int jobCount);

int loadJobs(struct Job *jobs, int *jobCount);

void rebuildNodeAllocation(struct Job *jobs, int jobCount,
                           struct GPUNode *nodes, int nodeCount);

void generateSchedulingReport(struct Job *jobs, int jobCount,
                              struct GPUNode *nodes, int nodeCount);

void clearInputBuffer(void);

void pauseScreen(void);


/* =========================
   STATIC JOB COUNTER
   ========================= */

/*
   static is used so that the job counter retains
   its value between calls to generateJobId().
*/
int generateJobId(void)
{
    static int jobCounter = 1001;

    return jobCounter++;
}

/*
   Synchronizes the static counter with the
   largest Job ID loaded from the file.
*/
void synchronizeJobCounter(int nextId)
{
    static int jobCounter = 1001;

    if (nextId > jobCounter)
    {
        jobCounter = nextId;
    }
}


/* =========================
   INPUT UTILITIES
   ========================= */

void clearInputBuffer(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        /* Clear remaining input */
    }
}

void pauseScreen(void)
{
    printf("\nPress Enter to continue...");
    getchar();
}


/* =========================
   NODE INITIALIZATION
   ========================= */

void initializeNodes(struct GPUNode *nodes, int *nodeCount)
{
    *nodeCount = 5;

    nodes[0].nodeId = 1;
    strcpy(nodes[0].nodeName, "GPU-Node-01");
    nodes[0].vramGB = 24.0;
    nodes[0].totalHours = 100.0;
    nodes[0].allocatedHours = 0.0;

    nodes[1].nodeId = 2;
    strcpy(nodes[1].nodeName, "GPU-Node-02");
    nodes[1].vramGB = 32.0;
    nodes[1].totalHours = 120.0;
    nodes[1].allocatedHours = 0.0;

    nodes[2].nodeId = 3;
    strcpy(nodes[2].nodeName, "GPU-Node-03");
    nodes[2].vramGB = 16.0;
    nodes[2].totalHours = 80.0;
    nodes[2].allocatedHours = 0.0;

    nodes[3].nodeId = 4;
    strcpy(nodes[3].nodeName, "GPU-Node-04");
    nodes[3].vramGB = 48.0;
    nodes[3].totalHours = 150.0;
    nodes[3].allocatedHours = 0.0;

    nodes[4].nodeId = 5;
    strcpy(nodes[4].nodeName, "GPU-Node-05");
    nodes[4].vramGB = 64.0;
    nodes[4].totalHours = 200.0;
    nodes[4].allocatedHours = 0.0;
}


/* =========================
   DISPLAY NODES
   ========================= */

void displayNodes(struct GPUNode *nodes, int nodeCount)
{
    int i;

    printf("\n");
    printf("====================================================================\n");
    printf("                         GPU NODE DETAILS\n");
    printf("====================================================================\n");

    printf("%-8s %-18s %-12s %-15s %-15s\n",
           "ID", "Node Name", "VRAM(GB)",
           "Total Hours", "Allocated");

    printf("--------------------------------------------------------------------\n");

    for (i = 0; i < nodeCount; i++)
    {
        printf("%-8d %-18s %-12.1f %-15.1f %-15.1f\n",
               nodes[i].nodeId,
               nodes[i].nodeName,
               nodes[i].vramGB,
               nodes[i].totalHours,
               nodes[i].allocatedHours);
    }

    printf("====================================================================\n");
}


/* =========================
   SEARCH JOB
   ========================= */

int searchJob(struct Job *jobs, int jobCount, int jobId)
{
    int i;

    /* Linear search */
    for (i = 0; i < jobCount; i++)
    {
        if (jobs[i].jobId == jobId)
        {
            return i;
        }
    }

    return -1;
}


/* =========================
   DUPLICATE ID CHECK
   ========================= */

int checkDuplicateJobId(struct Job *jobs, int jobCount, int jobId)
{
    if (searchJob(jobs, jobCount, jobId) != -1)
    {
        return 1;
    }

    return 0;
}


/* =========================
   CAPACITY VALIDATION
   ========================= */

int checkCapacity(struct Job *job, struct GPUNode *node)
{
    float remainingHours;

    remainingHours = node->totalHours - node->allocatedHours;

    /* VRAM validation */
    if (job->requiredVRAM > node->vramGB)
    {
        printf("\nERROR: Required VRAM exceeds node capacity.\n");
        printf("Required VRAM : %.1f GB\n", job->requiredVRAM);
        printf("Node VRAM     : %.1f GB\n", node->vramGB);

        return 0;
    }

    /* GPU hour validation */
    if (job->gpuHours > remainingHours)
    {
        printf("\nERROR: GPU hours exceed node availability.\n");
        printf("Requested Hours : %.1f\n", job->gpuHours);
        printf("Remaining Hours : %.1f\n", remainingHours);

        return 0;
    }

    return 1;
}


/* =========================
   ADD JOB
   ========================= */

void addJob(struct Job *jobs, int *jobCount,
            struct GPUNode *nodes, int nodeCount)
{
    struct Job newJob;
    int nodeChoice;
    int selectedNode;
    int i;

    if (*jobCount >= MAX_JOBS)
    {
        printf("\nERROR: Maximum job capacity reached.\n");
        return;
    }

    printf("\n");
    printf("========================================\n");
    printf("            ADD NEW JOB\n");
    printf("========================================\n");

    newJob.jobId = generateJobId();

    printf("Generated Job ID: %d\n", newJob.jobId);

    printf("\nEnter Job Name: ");
    fgets(newJob.jobName, NAME_SIZE, stdin);
    newJob.jobName[strcspn(newJob.jobName, "\n")] = '\0';

    do
    {
        printf("Enter Priority (1-10): ");
        scanf("%d", &newJob.priority);

        if (newJob.priority < 1 || newJob.priority > 10)
        {
            printf("Invalid priority. Enter value between 1 and 10.\n");
        }

    } while (newJob.priority < 1 || newJob.priority > 10);

    do
    {
        printf("Enter GPU Hours: ");
        scanf("%f", &newJob.gpuHours);

        if (newJob.gpuHours <= 0)
        {
            printf("GPU hours must be greater than 0.\n");
        }

    } while (newJob.gpuHours <= 0);

    do
    {
        printf("Enter Required VRAM (GB): ");
        scanf("%f", &newJob.requiredVRAM);

        if (newJob.requiredVRAM <= 0)
        {
            printf("VRAM must be greater than 0.\n");
        }

    } while (newJob.requiredVRAM <= 0);

    clearInputBuffer();

    printf("\nEnter Research Group ID: ");
    scanf("%d", &newJob.group.groupId);

    clearInputBuffer();

    printf("Enter Research Group Name: ");
    fgets(newJob.group.groupName, GROUP_SIZE, stdin);
    newJob.group.groupName[strcspn(newJob.group.groupName, "\n")] = '\0';

    printf("Enter Group Leader Name: ");
    fgets(newJob.group.leaderName, NAME_SIZE, stdin);
    newJob.group.leaderName[strcspn(newJob.group.leaderName, "\n")] = '\0';

    printf("\nAvailable GPU Nodes:\n");
    displayNodes(nodes, nodeCount);

    printf("\nEnter Node ID: ");
    scanf("%d", &nodeChoice);

    selectedNode = -1;

    for (i = 0; i < nodeCount; i++)
    {
        if (nodes[i].nodeId == nodeChoice)
        {
            selectedNode = i;
            break;
        }
    }

    if (selectedNode == -1)
    {
        printf("\nERROR: Invalid Node ID.\n");
        return;
    }

    newJob.node = nodes[selectedNode];

    strcpy(newJob.status, "Pending");

    /*
       Capacity validation before scheduling.
       Temporary copy is used so the actual node
       allocation is changed only after validation.
    */
    if (checkCapacity(&newJob, &nodes[selectedNode]) == 0)
    {
        printf("\nJob was NOT scheduled.\n");
        return;
    }

    /* Add job to array */
    jobs[*jobCount] = newJob;

    /* Update actual node allocation */
    nodes[selectedNode].allocatedHours += newJob.gpuHours;

    /*
       Store updated node details inside job
       so nested structure information is maintained.
    */
    jobs[*jobCount].node = nodes[selectedNode];

    (*jobCount)++;

    printf("\n----------------------------------------\n");
    printf("JOB ADDED SUCCESSFULLY\n");
    printf("----------------------------------------\n");
    printf("Job ID       : %d\n", newJob.jobId);
    printf("Job Name     : %s\n", newJob.jobName);
    printf("Priority     : %d\n", newJob.priority);
    printf("GPU Hours    : %.1f\n", newJob.gpuHours);
    printf("Required VRAM: %.1f GB\n", newJob.requiredVRAM);
    printf("Node         : %s\n", nodes[selectedNode].nodeName);
    printf("Status       : %s\n", newJob.status);
}


/* =========================
   DISPLAY ALL JOBS
   ========================= */

void displayJobs(struct Job *jobs, int jobCount)
{
    int i;

    if (jobCount == 0)
    {
        printf("\nNo job records available.\n");
        return;
    }

    printf("\n");
    printf("========================================================================================\n");
    printf("                              ALL GPU JOBS\n");
    printf("========================================================================================\n");

    printf("%-7s %-22s %-9s %-10s %-10s %-18s %-12s\n",
           "ID", "Job Name", "Priority",
           "GPU Hrs", "VRAM", "Node", "Status");

    printf("----------------------------------------------------------------------------------------\n");

    for (i = 0; i < jobCount; i++)
    {
        printf("%-7d %-22s %-9d %-10.1f %-10.1f %-18s %-12s\n",
               jobs[i].jobId,
               jobs[i].jobName,
               jobs[i].priority,
               jobs[i].gpuHours,
               jobs[i].requiredVRAM,
               jobs[i].node.nodeName,
               jobs[i].status);
    }

    printf("========================================================================================\n");
}


/* =========================
   SEARCH AND DISPLAY
   ========================= */

void searchAndDisplayJob(struct Job *jobs, int jobCount)
{
    int id;
    int index;

    if (jobCount == 0)
    {
        printf("\nNo jobs available for searching.\n");
        return;
    }

    printf("\nEnter Job ID to search: ");
    scanf("%d", &id);

    index = searchJob(jobs, jobCount, id);

    if (index == -1)
    {
        printf("\nERROR: Job ID %d not found.\n", id);
        return;
    }

    printf("\n========================================\n");
    printf("             JOB FOUND\n");
    printf("========================================\n");

    printf("Job ID          : %d\n", jobs[index].jobId);
    printf("Job Name        : %s\n", jobs[index].jobName);
    printf("Priority        : %d\n", jobs[index].priority);
    printf("GPU Hours       : %.1f\n", jobs[index].gpuHours);
    printf("Required VRAM   : %.1f GB\n", jobs[index].requiredVRAM);

    printf("\nResearch Group\n");
    printf("------------------------------\n");
    printf("Group ID        : %d\n", jobs[index].group.groupId);
    printf("Group Name      : %s\n", jobs[index].group.groupName);
    printf("Group Leader    : %s\n", jobs[index].group.leaderName);

    printf("\nGPU Node\n");
    printf("------------------------------\n");
    printf("Node ID         : %d\n", jobs[index].node.nodeId);
    printf("Node Name       : %s\n", jobs[index].node.nodeName);
    printf("Node VRAM       : %.1f GB\n", jobs[index].node.vramGB);
    printf("Node Capacity   : %.1f Hours\n", jobs[index].node.totalHours);

    printf("\nStatus          : %s\n", jobs[index].status);
}


/* =========================
   SORT BY PRIORITY
   ========================= */

void sortJobsByPriority(struct Job *jobs, int jobCount)
{
    int i, j;
    struct Job temp;

    if (jobCount == 0)
    {
        printf("\nNo jobs available for sorting.\n");
        return;
    }

    /*
       Bubble sort in descending priority order.
       Higher priority jobs appear first.
    */
    for (i = 0; i < jobCount - 1; i++)
    {
        for (j = 0; j < jobCount - i - 1; j++)
        {
            if (jobs[j].priority < jobs[j + 1].priority)
            {
                temp = jobs[j];
                jobs[j] = jobs[j + 1];
                jobs[j + 1] = temp;
            }
        }
    }

    printf("\nJobs sorted successfully by priority in descending order.\n");

    displayJobs(jobs, jobCount);
}


/* =========================
   CHECK NODE CAPACITY
   ========================= */

void checkNodeCapacity(struct Job *jobs, int jobCount,
                       struct GPUNode *nodes, int nodeCount)
{
    int nodeId;
    int i;
    float requestedHours;
    float requestedVRAM;
    float remainingHours;

    printf("\nEnter Node ID: ");
    scanf("%d", &nodeId);

    for (i = 0; i < nodeCount; i++)
    {
        if (nodes[i].nodeId == nodeId)
        {
            printf("\nNode: %s\n", nodes[i].nodeName);
            printf("VRAM Capacity: %.1f GB\n", nodes[i].vramGB);
            printf("Total Hours : %.1f\n", nodes[i].totalHours);
            printf("Allocated    : %.1f\n", nodes[i].allocatedHours);

            remainingHours =
                nodes[i].totalHours - nodes[i].allocatedHours;

            printf("Remaining    : %.1f\n", remainingHours);

            printf("\nEnter requested GPU Hours: ");
            scanf("%f", &requestedHours);

            printf("Enter required VRAM: ");
            scanf("%f", &requestedVRAM);

            if (requestedVRAM > nodes[i].vramGB)
            {
                printf("\nCAPACITY CHECK: FAILED\n");
                printf("Required VRAM exceeds node VRAM.\n");
                return;
            }

            if (requestedHours > remainingHours)
            {
                printf("\nCAPACITY CHECK: FAILED\n");
                printf("Requested GPU hours exceed remaining capacity.\n");
                return;
            }

            printf("\nCAPACITY CHECK: PASSED\n");
            printf("Job can be accommodated on this node.\n");

            return;
        }
    }

    printf("\nERROR: Node ID not found.\n");
}


/* =========================
   NODE UTILIZATION ANALYSIS
   ========================= */

void analyseNodeUtilization(struct GPUNode *nodes, int nodeCount)
{
    int i;
    float utilization;

    printf("\n");
    printf("====================================================================\n");
    printf("                    NODE UTILIZATION ANALYSIS\n");
    printf("====================================================================\n");

    for (i = 0; i < nodeCount; i++)
    {
        if (nodes[i].totalHours > 0)
        {
            utilization =
                (nodes[i].allocatedHours /
                 nodes[i].totalHours) * 100.0;
        }
        else
        {
            utilization = 0;
        }

        printf("\nNode %d - %s\n",
               nodes[i].nodeId,
               nodes[i].nodeName);

        printf("Allocated Hours : %.1f\n",
               nodes[i].allocatedHours);

        printf("Total Hours     : %.1f\n",
               nodes[i].totalHours);

        printf("Utilization     : %.2f%%\n",
               utilization);

        /*
           Classification:
           0%          -> Available
           0-99.99%    -> Busy
           >=100%      -> Fully Booked
        */

        if (utilization <= 0)
        {
            printf("Classification   : Available\n");
        }
        else if (utilization < 100)
        {
            printf("Classification   : Busy\n");
        }
        else
        {
            printf("Classification   : Fully Booked\n");
        }

        if (utilization >= 80 && utilization < 100)
        {
            printf("Warning          : Node is nearing full utilization.\n");
        }
    }

    printf("\n====================================================================\n");
}


/* =========================
   UPDATE JOB
   ========================= */

void updateJob(struct Job *jobs, int jobCount,
               struct GPUNode *nodes, int nodeCount)
{
    int id;
    int index;
    int choice;
    int newPriority;
    int i;
    int nodeIndex;
    float newHours;
    float newVRAM;
    char newStatus[20];

    if (jobCount == 0)
    {
        printf("\nNo jobs available for update.\n");
        return;
    }

    printf("\nEnter Job ID to update: ");
    scanf("%d", &id);

    index = searchJob(jobs, jobCount, id);

    if (index == -1)
    {
        printf("\nERROR: Job ID not found.\n");
        return;
    }

    printf("\n========================================\n");
    printf("              UPDATE JOB\n");
    printf("========================================\n");

    printf("1. Update Priority\n");
    printf("2. Update GPU Hours\n");
    printf("3. Update Required VRAM\n");
    printf("4. Update Status\n");
    printf("5. Update Priority and Status\n");

    printf("\nEnter choice: ");
    scanf("%d", &choice);

    switch (choice)
    {
        case 1:

            printf("Current Priority: %d\n",
                   jobs[index].priority);

            printf("Enter New Priority (1-10): ");
            scanf("%d", &newPriority);

            if (newPriority < 1 || newPriority > 10)
            {
                printf("ERROR: Priority must be between 1 and 10.\n");
                return;
            }

            jobs[index].priority = newPriority;

            printf("Priority updated successfully.\n");

            break;


        case 2:

            printf("Current GPU Hours: %.1f\n",
                   jobs[index].gpuHours);

            printf("Enter New GPU Hours: ");
            scanf("%f", &newHours);

            if (newHours <= 0)
            {
                printf("ERROR: GPU hours must be positive.\n");
                return;
            }

            /*
               Find node corresponding to this job.
            */
            nodeIndex = -1;

            for (i = 0; i < nodeCount; i++)
            {
                if (nodes[i].nodeId ==
                    jobs[index].node.nodeId)
                {
                    nodeIndex = i;
                    break;
                }
            }

            if (nodeIndex == -1)
            {
                printf("ERROR: Associated node not found.\n");
                return;
            }

            /*
               Temporarily remove old allocation.
            */
            nodes[nodeIndex].allocatedHours -=
                jobs[index].gpuHours;

            if (newHours >
                nodes[nodeIndex].totalHours -
                nodes[nodeIndex].allocatedHours)
            {
                printf("\nERROR: New GPU hours exceed node capacity.\n");

                /*
                   Restore old allocation.
                */
                nodes[nodeIndex].allocatedHours +=
                    jobs[index].gpuHours;

                return;
            }

            jobs[index].gpuHours = newHours;

            nodes[nodeIndex].allocatedHours += newHours;

            jobs[index].node = nodes[nodeIndex];

            printf("GPU hours updated successfully.\n");

            break;


        case 3:

            printf("Current VRAM: %.1f GB\n",
                   jobs[index].requiredVRAM);

            printf("Enter New Required VRAM: ");
            scanf("%f", &newVRAM);

            if (newVRAM <= 0)
            {
                printf("ERROR: VRAM must be positive.\n");
                return;
            }

            if (newVRAM > jobs[index].node.vramGB)
            {
                printf("\nERROR: Required VRAM exceeds node VRAM.\n");
                return;
            }

            jobs[index].requiredVRAM = newVRAM;

            printf("Required VRAM updated successfully.\n");

            break;


        case 4:

            clearInputBuffer();

            printf("Enter new status (Pending/Completed): ");
            fgets(newStatus, sizeof(newStatus), stdin);

            newStatus[strcspn(newStatus, "\n")] = '\0';

            if (strcmp(newStatus, "Pending") != 0 &&
                strcmp(newStatus, "Completed") != 0)
            {
                printf("ERROR: Status must be Pending or Completed.\n");
                return;
            }

            /*
               If changing Pending to Completed,
               release allocated GPU hours.
            */
            if (strcmp(jobs[index].status, "Pending") == 0 &&
                strcmp(newStatus, "Completed") == 0)
            {
                for (i = 0; i < nodeCount; i++)
                {
                    if (nodes[i].nodeId ==
                        jobs[index].node.nodeId)
                    {
                        nodes[i].allocatedHours -=
                            jobs[index].gpuHours;

                        jobs[index].node = nodes[i];

                        break;
                    }
                }
            }

            /*
               If changing Completed back to Pending,
               verify capacity before allocating.
            */
            else if (strcmp(jobs[index].status, "Completed") == 0 &&
                     strcmp(newStatus, "Pending") == 0)
            {
                for (i = 0; i < nodeCount; i++)
                {
                    if (nodes[i].nodeId ==
                        jobs[index].node.nodeId)
                    {
                        if (nodes[i].allocatedHours +
                            jobs[index].gpuHours >
                            nodes[i].totalHours)
                        {
                            printf("\nERROR: Cannot reactivate job.\n");
                            printf("Node does not have enough capacity.\n");
                            return;
                        }

                        nodes[i].allocatedHours +=
                            jobs[index].gpuHours;

                        jobs[index].node = nodes[i];

                        break;
                    }
                }
            }

            strcpy(jobs[index].status, newStatus);

            printf("Status updated successfully.\n");

            break;


        case 5:

            printf("Enter New Priority (1-10): ");
            scanf("%d", &newPriority);

            if (newPriority < 1 || newPriority > 10)
            {
                printf("ERROR: Invalid priority.\n");
                return;
            }

            clearInputBuffer();

            printf("Enter new status (Pending/Completed): ");
            fgets(newStatus, sizeof(newStatus), stdin);

            newStatus[strcspn(newStatus, "\n")] = '\0';

            if (strcmp(newStatus, "Pending") != 0 &&
                strcmp(newStatus, "Completed") != 0)
            {
                printf("ERROR: Invalid status.\n");
                return;
            }

            jobs[index].priority = newPriority;

            strcpy(jobs[index].status, newStatus);

            printf("Priority and status updated successfully.\n");

            break;


        default:

            printf("\nInvalid update choice.\n");
    }
}


/* =========================
   SAVE JOBS TO FILE
   ========================= */

void saveJobs(struct Job *jobs, int jobCount)
{
    FILE *file;

    file = fopen("jobs.dat", "wb");

    if (file == NULL)
    {
        printf("\nERROR: Unable to open jobs.dat for writing.\n");
        return;
    }

    if (fwrite(&jobCount, sizeof(int), 1, file) != 1)
    {
        printf("\nERROR: Unable to save job count.\n");
        fclose(file);
        return;
    }

    if (jobCount > 0)
    {
        if (fwrite(jobs, sizeof(struct Job),
                   jobCount, file) != (size_t)jobCount)
        {
            printf("\nERROR: Unable to save job records.\n");
            fclose(file);
            return;
        }
    }

    fclose(file);

    printf("\nJobs saved successfully to jobs.dat\n");
}


/* =========================
   LOAD JOBS FROM FILE
   ========================= */

int loadJobs(struct Job *jobs, int *jobCount)
{
    FILE *file;
    int i;
    int maxJobId = 1000;

    file = fopen("jobs.dat", "rb");

    if (file == NULL)
    {
        *jobCount = 0;

        printf("\nNo previous job file found.\n");
        printf("Starting with an empty job list.\n");

        return 0;
    }

    if (fread(jobCount, sizeof(int), 1, file) != 1)
    {
        printf("\nERROR: Unable to read job count.\n");

        fclose(file);

        *jobCount = 0;

        return 0;
    }

    if (*jobCount < 0 || *jobCount > MAX_JOBS)
    {
        printf("\nERROR: Invalid job file data.\n");

        fclose(file);

        *jobCount = 0;

        return 0;
    }

    if (*jobCount > 0)
    {
        if (fread(jobs, sizeof(struct Job),
                  *jobCount, file) != (size_t)(*jobCount))
        {
            printf("\nERROR: Unable to read job records.\n");

            fclose(file);

            *jobCount = 0;

            return 0;
        }
    }

    fclose(file);

    /*
       Find highest existing Job ID and synchronize
       the static counter so newly generated IDs
       remain unique.
    */
    for (i = 0; i < *jobCount; i++)
    {
        if (jobs[i].jobId > maxJobId)
        {
            maxJobId = jobs[i].jobId;
        }
    }

    synchronizeJobCounter(maxJobId + 1);

    printf("\n%d previous job record(s) loaded successfully.\n",
           *jobCount);

    return 1;
}


/* =========================
   REBUILD NODE ALLOCATION
   ========================= */

void rebuildNodeAllocation(struct Job *jobs, int jobCount,
                           struct GPUNode *nodes, int nodeCount)
{
    int i;
    int j;

    /*
       Reset node allocations.
    */
    for (i = 0; i < nodeCount; i++)
    {
        nodes[i].allocatedHours = 0;
    }

    /*
       Recalculate allocations from pending jobs.
    */
    for (i = 0; i < jobCount; i++)
    {
        if (strcmp(jobs[i].status, "Pending") == 0)
        {
            for (j = 0; j < nodeCount; j++)
            {
                if (jobs[i].node.nodeId ==
                    nodes[j].nodeId)
                {
                    nodes[j].allocatedHours +=
                        jobs[i].gpuHours;

                    /*
                       Keep nested node information updated.
                    */
                    jobs[i].node = nodes[j];

                    break;
                }
            }
        }
    }
}


/* =========================
   GENERATE SCHEDULING REPORT
   ========================= */

void generateSchedulingReport(struct Job *jobs, int jobCount,
                              struct GPUNode *nodes, int nodeCount)
{
    FILE *file;
    int i;
    float totalAvailable = 0;
    float totalAllocated = 0;
    float utilization;

    file = fopen("scheduling_report.txt", "w");

    if (file == NULL)
    {
        printf("\nERROR: Unable to create scheduling_report.txt\n");
        return;
    }

    fprintf(file, "============================================================\n");
    fprintf(file, "             CLOUD GPU SCHEDULING REPORT\n");
    fprintf(file, "============================================================\n\n");

    /* High priority jobs */
    fprintf(file, "PENDING HIGH-PRIORITY JOBS\n");
    fprintf(file, "------------------------------------------------------------\n");

    for (i = 0; i < jobCount; i++)
    {
        if (jobs[i].priority >= 8 &&
            strcmp(jobs[i].status, "Pending") == 0)
        {
            fprintf(file,
                    "Job ID: %d | Job: %s | Priority: %d | GPU Hours: %.1f | Node: %s\n",
                    jobs[i].jobId,
                    jobs[i].jobName,
                    jobs[i].priority,
                    jobs[i].gpuHours,
                    jobs[i].node.nodeName);
        }
    }

    /* GPU utilization */
    fprintf(file, "\n");
    fprintf(file, "GPU HOURS SUMMARY\n");
    fprintf(file, "------------------------------------------------------------\n");

    for (i = 0; i < nodeCount; i++)
    {
        totalAvailable += nodes[i].totalHours;
        totalAllocated += nodes[i].allocatedHours;
    }

    fprintf(file, "Total GPU Hours Available : %.1f\n",
            totalAvailable);

    fprintf(file, "Total GPU Hours Allocated : %.1f\n",
            totalAllocated);

    fprintf(file, "Total GPU Hours Remaining : %.1f\n",
            totalAvailable - totalAllocated);

    /* Node status */
    fprintf(file, "\n");
    fprintf(file, "NODE UTILIZATION\n");
    fprintf(file, "------------------------------------------------------------\n");

    for (i = 0; i < nodeCount; i++)
    {
        utilization =
            (nodes[i].allocatedHours /
             nodes[i].totalHours) * 100.0;

        fprintf(file,
                "%s | Allocated: %.1f / %.1f | Utilization: %.2f%% | Status: ",
                nodes[i].nodeName,
                nodes[i].allocatedHours,
                nodes[i].totalHours,
                utilization);

        if (utilization <= 0)
        {
            fprintf(file, "Available");
        }
        else if (utilization < 100)
        {
            fprintf(file, "Busy");
        }
        else
        {
            fprintf(file, "Fully Booked");
        }

        if (utilization >= 80 && utilization < 100)
        {
            fprintf(file, " | NEARING FULL UTILIZATION");
        }

        fprintf(file, "\n");
    }

    fprintf(file, "\n");
    fprintf(file, "============================================================\n");
    fprintf(file, "Report generated successfully.\n");
    fprintf(file, "============================================================\n");

    fclose(file);

    printf("\nScheduling report generated successfully.\n");
    printf("File: scheduling_report.txt\n");
}


/* =========================
   MAIN FUNCTION
   ========================= */

int main(void)
{
    struct Job jobs[MAX_JOBS];
    struct GPUNode nodes[MAX_NODES];

    int jobCount = 0;
    int nodeCount = 0;
    int choice;

    /*
       Initialize GPU nodes.
       Data is local to main, so there are
       no global variables.
    */
    initializeNodes(nodes, &nodeCount);

    /*
       Load previously stored jobs when program starts.
    */
    loadJobs(jobs, &jobCount);

    /*
       Recalculate node allocations from loaded jobs.
    */
    rebuildNodeAllocation(jobs, jobCount,
                          nodes, nodeCount);

    do
    {
        printf("\n\n");
        printf("============================================================\n");
        printf("           CLOUD GPU COMPUTE JOB SCHEDULING SYSTEM\n");
        printf("============================================================\n");

        printf("1.  Add New Job\n");
        printf("2.  Update Job\n");
        printf("3.  Display All Jobs\n");
        printf("4.  Search Job by ID\n");
        printf("5.  Sort Jobs by Priority\n");
        printf("6.  Check Node Capacity\n");
        printf("7.  Analyse Node Utilization\n");
        printf("8.  Generate Scheduling Report\n");
        printf("9.  Save Jobs to File\n");
        printf("10. Load Jobs from File\n");
        printf("11. Display GPU Nodes\n");
        printf("0.  Exit\n");

        printf("============================================================\n");
        printf("Enter your choice: ");

        scanf("%d", &choice);
        clearInputBuffer();

        switch (choice)
        {
            case 1:

                addJob(jobs, &jobCount,
                       nodes, nodeCount);

                break;


            case 2:

                updateJob(jobs, jobCount,
                          nodes, nodeCount);

                break;


            case 3:

                displayJobs(jobs, jobCount);

                break;


            case 4:

                searchAndDisplayJob(jobs, jobCount);

                break;


            case 5:

                sortJobsByPriority(jobs, jobCount);

                break;


            case 6:

                checkNodeCapacity(jobs, jobCount,
                                  nodes, nodeCount);

                break;


            case 7:

                analyseNodeUtilization(nodes, nodeCount);

                break;


            case 8:

                generateSchedulingReport(jobs, jobCount,
                                         nodes, nodeCount);

                break;


            case 9:

                saveJobs(jobs, jobCount);

                break;


            case 10:

                loadJobs(jobs, &jobCount);

                rebuildNodeAllocation(jobs, jobCount,
                                      nodes, nodeCount);

                break;


            case 11:

                displayNodes(nodes, nodeCount);

                break;


            case 0:

                /*
                   Automatically save before exiting
                   so data persists between runs.
                */
                saveJobs(jobs, jobCount);

                printf("\nThank you for using the Cloud GPU Job Scheduler.\n");
                printf("Program terminated successfully.\n");

                break;


            default:

                printf("\nERROR: Invalid menu choice.\n");
                printf("Please select a valid option.\n");
        }

    } while (choice != 0);

    return 0;
}
