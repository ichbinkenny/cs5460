
#include "assignment1.h"
#include <string.h>
#include <assert.h>
#include <ctype.h>

uint64_t byte_sort(uint64_t arg){
    uint64_t result = 0;
    uint64_t mask = 0xFF00000000000000;
    uint64_t values[8];
    uint8_t i, j;
    //First, grab each part and convert it to a more usable form.
    for(i = 0; i < 8; i++){
	uint64_t newMask = mask >> 8 * i;
	uint8_t convertedValue = (arg & newMask) >> 8 * (8 - i - 1);
	values[i] = convertedValue;
    }
    //Now find each individual smallest value and add it to result
    for(i = 0; i < 8; i++){
	for(j = i + 1; j < 8; j++){
	    if(values[i] > values[j]){
		uint8_t temp = values[i];
		values[i] = values[j];
		values[j] = temp;
	    }
	}
    }
    mask = 0x00000000000000FF;
    //write values from the array to the result
    for(i = 0; i < 8; i++){
	result += (uint64_t)(values[i] << (8 * i));
    }
    return result;
}

uint64_t nibble_sort(uint64_t arg){
  uint64_t result = 0;
  uint64_t i, j;
  uint64_t values[16];
  //Get all nibbles in arg
  for(i = 0; i < 16; i++){
    uint64_t newMask = 0xF000000000000000 >> 4 * i;
    uint8_t nibbleValue = (arg & newMask) >> 4 * (16 - i - 1);
    values[i] = nibbleValue;
    //printf("New nibble value is: 0x%x\n", nibbleValue);
  }
  //Now sort all nibble values
  for(i = 0; i < 16; i++){
    for(j = i + 1; j < 16; j++){
      if(values[i] > values[j]){
        uint8_t temp = values[i];
        values[i] = values[j];
        values[j] = temp;
      }
    }
    result += values[i] << (4 * i);
  }
  return result;
}

struct elt* circular_list(const char* str){
  struct elt* firstEntry;
  if(str[0] == '\0'){
    return NULL;
  }
  else{
    //We have at least one character, so lets make an elt for it.
    firstEntry = malloc(sizeof(struct elt));
    uint8_t length = 0;
    uint8_t i, shouldFail = 0;
    if(firstEntry == NULL){
      return NULL;
    }

    firstEntry->val = str[0];
    void* firstLocation = firstEntry; //where the first entry is
    struct elt* entry = firstEntry;
    for(i = 0; str[i] != '\0'; i++){
      length++;
    }
    for(i = 1; i < length; i++){
      struct elt* newEntry;
      newEntry = malloc(sizeof(struct elt));
      if(newEntry == NULL){
        shouldFail = 1;
        free(newEntry);
        entry->link = NULL;
        break;
      }
      newEntry->val = str[i];
      entry->link = newEntry;
      newEntry->link = firstLocation;
      entry = newEntry;
    }
    entry = firstEntry;
    //cleanup if we fail to allocate memory
    if(shouldFail == 1){
      //our list is circular, so we can free using any position
      entry = firstEntry->link;
      while(entry != NULL){
        void* nextEntry = entry->link;
        free(entry);
        entry = nextEntry;
      }
      free(firstEntry);
      return NULL;
    }
    for(i=0; i < length; i++){
      entry = entry->link;
    }
    return firstEntry;
  }
}

int convert(enum format_t mode, const char* str, uint64_t* out){
  if(out == NULL){
    return -1;
  }
  int i;
  uint8_t length = 0;
  unsigned int data = 0;
  char letter;
  int currentPosition = 1;
  //calculate string length
  for(i = 0; str[i] != '\0'; i++){
    length++;
  }
  switch(mode){
  case OCT:
    for(i = 0; i < length; i++){
      letter = str[i];
      if(letter >= '0' && letter <= '7'){

      }
      else{
        return -1;
      }
    }
    //TODO change the way out is calculated so we don't use external functions.
    for(i = length - 1; i >= 0; i--){
      data += (str[i] - 48) * currentPosition;
      currentPosition *= 8;
    }
    *out = data;
    break;
  case HEX:
    for(i = 0; i < length; i++){
      letter = str[i];
      if(((letter >= 'a' && letter <= 'f') || (letter >= 'A' && letter <= 'F')) || (letter >= '0' && letter <= '9')){

        }
        else{
          return -1;
        }
    }
    for(i = length - 1; i >= 0; i--){
      if(str[i] > '9'){
        switch(str[i]){
        case 'A':
        case 'a':
        data +=  10 * currentPosition;
        break;
        case 'B':
        case 'b':
          data += 11 * currentPosition;
          break;
        case 'C':
        case 'c':
          data += 12 * currentPosition;
          break;
        case 'D':
        case 'd':
          data += 13 * currentPosition;
          break;
        case 'E':
        case 'e':
          data += 14 * currentPosition;
          break;
        case 'F':
        case 'f':
          data += 15 * currentPosition;
          break;
        }
      }
      else{
        data += (str[i] - 48) * currentPosition;
      }
      currentPosition *= 16;
    }
    *out = data;
    break;
  case BIN:
    for(i = 0; i < length; i++){
      letter = str[i];
      if(letter != '0' && letter != '1'){
        return -1;
      }
      else{

      }
    }
    //Return the numeric value represented in binary.
    for(i = 0; i < length; i++){
      data += (str[i] - 48) << i;
    }
    *out = data;
    break;
  default:
    return -1;
  }
  return 0;
}

void log_pid(void){
  int pid = syscall(SYS_getpid);
  uint8_t length = 7;
  if(pid == -1){
    return;
  }
  int file = syscall(SYS_open, "pid.log", 0102 /*O_CREAT VALUE With write*/, 00600 /*give read and write access to user*/);
  if(file == -1){
    return;
  }
  //Convert the pid number to ascii code
  int val = 10000000;
  char data[9] = {0, 0, 0, 0, 0, 0, 0, 0 , 0};
  //TODO create this without needing sprintf
  //get each number in pid
  int i;
  printf("PID is: %d\n", pid);
  for(i = 0; i < 7 /*Max number of digits in linux x86_64 process id*/; i++){
    if((pid / val) == 0){
      if(pid % val == 0){
          val /= 10;
          length--;
          printf("Minusing! PID: %d and VAL: %d\n", pid, val);
          continue;
      }
    }
    data[6 - i] = (48 + (pid % 10));
    printf("Val: %c\n", data[6-i]);
    pid /= 10;
    val /= 10;
    length++;
  }
  data[length] = '\n';
  //data[length + 1] = '\0';
  printf("Data is: %s\n", data);
  //sprintf(data, "%d\n", pid);
  int status = syscall(SYS_write, file, &data, length + 1);
  if(status == -1){
    printf("WRITE ERR!\n");
    syscall(SYS_unlink, "pid.log");
    return;
  }
  syscall(SYS_close, file);
}
//TODO Remove this main function before submitting
int main(){
    assert(byte_sort(0x0102030405060708) == 0x0807060504030201);
    assert(byte_sort(0x0403deadbeef0201) == 0xefdebead04030201);
    assert(nibble_sort(0x0403deadbeef0201) == 0xfeeeddba43210000);
    struct elt* data = circular_list("cat");
    assert(data->val == 'c');
    assert(data->link->val == 'a');
    assert(data->link->link->val == 't');
    assert(data->link->link->link->val == 'c');
    assert(circular_list("") == NULL);
    uint64_t out;
    assert(convert(0, "0x01010101010", &out) == -1);
    assert(convert(HEX, "0x01111111", NULL) == -1);
    assert(convert(HEX, "deadbeeg", &out) == -1);
    assert(convert(OCT, "01001008", &out) == -1);
    assert(convert(BIN, "010010012", &out) == -1);
    assert(convert(HEX, "deadbeef", &out) == 0);
    assert(out == 3735928559);
    assert(convert(OCT, "01423176", &out) == 0);
    assert(out == 403070);
    assert(convert(BIN, "11111111", &out) == 0);
    assert(out == 255);
    log_pid();
    return 0;
}
