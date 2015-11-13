#include <stdio.h>
#include <stdlib.h>


//globabl variables for the parameters to the program
static int num_lines, 
    lines_per_set,
    bytes_per_block;


int mylog2(int input) {
    int ret = -1;
    while(input != 0) {
        input >>=1;
        ret++;
    }

    return ret;
}
int requestRead(int block, int set, int tag, int lines_per_set, int cache[][lines_per_set][bytes_per_block + 3]) {
    //printf("the fist element: %d   ", cache[set][0][0]);
    //Looping through all of the items
    static int timestamp = 0; //timestamp 
    int i = 0; //for the loop
    int oldestTimestamp = timestamp; //keeps track of the lowest timetamp
    int oldestTimestampLoc = 0; //keeps track of location of lowest timestamp
    for(i = 0; i < lines_per_set; i++) {
        if(cache[set][i][0] && cache[set][i][1] == tag) {
            return 1;
            
            cache[set][i][2] = timestamp++;
        } else {
          if(cache[set][i][2] <= oldestTimestamp) {
               oldestTimestamp = cache[set][i][2];
               oldestTimestampLoc = i;
          }
            
        }          
    }

    //No Hit, adding all necessary info to the, oldest location in the cache. 
    timestamp++;
    cache[set][oldestTimestampLoc][1] = tag;
    cache[set][oldestTimestampLoc][0] = 1;
    cache[set][oldestTimestampLoc][2] = timestamp;

    return 0;
} 
int main (int argc, char *argv[]) {
        num_lines = atoi(argv[1]);
        lines_per_set = atoi(argv[2]);
        bytes_per_block = atoi(argv[3]);

    char line[80];

    long addr_req;

    //Creating the cache


    printf("Simulating cache with:\n");
    printf(" - Total lines   = %d\n", num_lines);
    printf(" - Lines per set = %d\n", lines_per_set);
    printf(" - Block size    = %d bytes\n", bytes_per_block);
    
    //variables for keeping track of cache hits and such.
    int cacheHits = 0;
    int totalOperations = 0;
    
    //I was going to calculate the number of bits to AND in order to figure out where 
    //an address goes in the cache, but I can just do AND (number - 1) to figure out what the neccessary bits are
    //but..... I may need to figure it out anyways in order to convert 10100 to 101
    int blockNumberBits = mylog2(bytes_per_block);
    int setNumberBits = mylog2(num_lines/lines_per_set);
    int blockNumberMask = bytes_per_block - 1;
    int setNumberMask = ((num_lines/lines_per_set) - 1) << blockNumberBits;
    int remainderMask = ~((1 << (blockNumberBits + setNumberBits)) - 1);

    //printf("blockbits: %x - setBits: %x blockMask: %x  setMask: %x remainder:%x \n", 
            //blockNumberBits, setNumberBits, blockNumberMask, setNumberMask, remainderMask);

    //creating the datastructure that will hold the simulated cache
    int cache[num_lines/lines_per_set][lines_per_set][3 + bytes_per_block]; //the 3 is 
             //for the 3 extra parts of data:
             //0: valid
             //1: tag
             //2: how recently used

    //int *cache = malloc(sizeof(cache));

    //Clearing the cache's valid bits
    int i = 0;
    int n = 0;
    for(i = 0; i < num_lines/lines_per_set; i++) {
        for(n = 0; n < lines_per_set; n++) {
            cache[i][n][0] = 0;
            cache[i][n][2] = 0;
        }
    }

    //printf("wtf.... %d  ", cache[0][0][0]);

    while (fgets(line, 80, stdin)) {
        addr_req = strtol(line, NULL, 0);

        //How the cache reading works:
        //Block size: way right
        //total lines => number of sets => lg total_lines = number of bits to use in the address
        //               set number uses the bits to the left of the block size 
        //lines per set => is what it sounds like. Uses the rest of the bits.
        //all of the addesses that are able to be in a given set, are staggered the size of S
        
        //Gameplan:
        //split up an address into it's respective parts so that each variable
        //below will be the index in the array for the respective part of the datastructure
        int blockNumber = (addr_req & blockNumberMask);
        int setNumber = (addr_req & setNumberMask) >> blockNumberBits;
        int remainderNumber = ((addr_req & remainderMask) >> (blockNumberBits + setNumberBits));

        //printf("a: %x b: %x c: %x  ", blockNumber, setNumber, remainderNumber);

        //requesting a read from the cache
        int a = requestRead(blockNumber, setNumber, remainderNumber, lines_per_set, cache);
        //printf("o: %d  ", a);
        if(a == 1) {
            cacheHits++;
        }
        totalOperations++;

        /* simulate cache fetch with address `addr_req` */
        //printf("Processing request: 0x%lX\n", addr_req);
    }
    printf("hitrate:%f | missrate:%f", 100.0*cacheHits/totalOperations, 100.0*(totalOperations - cacheHits)/totalOperations);
    return 0;
}
