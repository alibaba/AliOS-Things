#include "tftp.h"

/* 
 * this function will return the appropriate packet struct given a buffer 
 * pointer to the next packet recieved
 */
PACKET * unserializePacket(char * buffer, size_t bufferSize, PACKET* packet)
{
  char* dataOffset;
  size_t n;
  if (bufferSize < 4)
  {
    return NULL;
  }

  if (packet == NULL)
  {
    return NULL;
  }
  bzero(packet,sizeof(PACKET));
  packet->optcode = getHostOrderShortFromNetwork(buffer);
  dataOffset = &buffer[2];

  switch (packet->optcode)
  {
    case TFTP_OPTCODE_RRQ: //read
      n = charncpy(packet->read_request.filename,dataOffset,MAX_STRING_SIZE);
      dataOffset += n;
      charncpy(packet->read_request.mode,dataOffset,MAX_MODE_SIZE);
      break;
    case TFTP_OPTCODE_WRQ: //write
      n = charncpy(packet->write_request.filename,dataOffset,MAX_STRING_SIZE);
      dataOffset += n;
      charncpy(packet->write_request.mode,dataOffset,MAX_MODE_SIZE);
      break;
    case TFTP_OPTCODE_DATA: //data
      packet->data.blockNumber = getHostOrderShortFromNetwork(dataOffset);
      dataOffset +=2;
      packet->data.dataSize = (bufferSize-(2*sizeof(uint16_t)));
      memcpy(packet->data.data,dataOffset, bufferSize);
      break;
    case TFTP_OPTCODE_ACK: //ack
      packet->ack.blockNumber = getHostOrderShortFromNetwork(dataOffset);
      break;
    case TFTP_OPTCODE_ERR: //error
      packet->error.errorCode = getHostOrderShortFromNetwork(dataOffset);
      dataOffset +=2;
      charncpy(packet->error.message,dataOffset, MAX_STRING_SIZE);
      break;
    default:
      //free(packet); dont want to do this!
      return NULL;
  }
  //done
  return packet;
}

/* reads data from a packet struct into a buffer to send over the wire
 * returns the size of the buffer to send */
size_t serializePacket(const PACKET* packet, char * buffer)
{
  if (buffer == NULL || packet == NULL)
  {
    return 0;
  }

  size_t n = 0;
  char* dataOffset;
  
  *((uint16_t*)buffer) = getNetworkOrderShortFromHost(packet->optcode,NULL);
  dataOffset = buffer + 2; //skip over the optcode

  switch (packet->optcode)
  {
    case TFTP_OPTCODE_RRQ: //read
      n = charncpy(dataOffset,packet->read_request.filename,MAX_STRING_SIZE);
      dataOffset += n;
      n += charncpy(dataOffset,packet->read_request.mode,MAX_MODE_SIZE);
      break;
   case TFTP_OPTCODE_WRQ: //write
      n = charncpy(dataOffset,packet->read_request.filename,MAX_STRING_SIZE);
      dataOffset += n;
      n += charncpy(dataOffset,packet->read_request.mode,MAX_MODE_SIZE);
      break;
   case TFTP_OPTCODE_DATA: //data
      *((uint16_t*)dataOffset) = getNetworkOrderShortFromHost(packet->data.blockNumber,NULL);
      dataOffset +=2; //skip over block number
      memcpy(dataOffset,packet->data.data,packet->data.dataSize);
      n += (packet->data.dataSize + 2); //add room for the block number
      break;
    case TFTP_OPTCODE_ACK: //ack
      *((uint16_t*)dataOffset) = getNetworkOrderShortFromHost(packet->ack.blockNumber,NULL);
      n = sizeof(ACK);
      break;
    case TFTP_OPTCODE_ERR: //error
      *((uint16_t*)dataOffset) = getNetworkOrderShortFromHost(packet->ack.blockNumber,NULL);
      dataOffset +=2; //skip over errorCode
      n = charncpy(dataOffset,packet->error.message,MAX_STRING_SIZE);
      n += 2; //add room for the block number
      break;
    default:
      return 0;
  }
  //done
  return n + sizeof(uint16_t); //sizeof(PACKET)
}

/* prints the packet and all fields for debugging */
void printPacket(PACKET* packet)
{
  if (packet == NULL)
  {
      printf("Null packet");
      return;
  }
  printf("-------[Packet]-------\n");

  printf("Optcode: [%u] ",packet->optcode);

  switch (packet->optcode)
  {
    case TFTP_OPTCODE_RRQ: //read
      printf("READ\n");
      printf("Filename: %s\n",packet->read_request.filename);
      printf("Mode: %s\n",packet->read_request.mode);
      break;
    case TFTP_OPTCODE_WRQ: //write
      printf("WRITE\n");
      printf("Filename: %s\n",packet->write_request.filename);
      printf("Mode: %s\n",packet->write_request.mode);
      break;
    case TFTP_OPTCODE_DATA: //data
      printf("DATA\n");
      printf("Block #: %u\n",packet->data.blockNumber);
      printf("DataSize: %u\n",packet->data.dataSize);
      break;
    case TFTP_OPTCODE_ACK: //ack
      printf("ACK\n");
      printf("Block #: %u\n",packet->ack.blockNumber);
      break;
    case TFTP_OPTCODE_ERR: //error
      printf("ERROR\n");
      printf("ErrorCode: %u\n",packet->error.errorCode);
      printf("ErrorMessage: %s\n",packet->error.message);
      break;
    default:
      printf("Unknown Packet Type\n");
      break;
  }
  printf("----------------------\n");
}


// this is used as the error handler when we recieve an error packet
void printError(PACKET * packet)
{
  if (packet->optcode != TFTP_OPTCODE_ERR)
  {
    //not error packet
    return;
  }
  printf("Error recieved with CODE: [");
  switch (packet->error.errorCode)
  {
    case TFTP_ERRCODE_UNDEFINED:
      printf("Undefined");
      break;
    case TFTP_ERRCODE_FILE_NOT_FOUND:
      printf("File Not Found");
      break;
    case TFTP_ERRCODE_ACCESS_VIOLATION:
      printf("Access Violation");
      break;
    case TFTP_ERRCODE_DISK_FULL:
      printf("Disk Full");
      break;
    case TFTP_ERRCODE_UNKNOWN_TRANSFER_ID:
      printf("Unknown Transfer ID");
      break;
    case TFTP_ERRCODE_FILE_ALREADY_EXISTS:
      printf("File Already Exists");
      break;
    case TFTP_ERRCODE_NO_SUCH_USER:
      printf("No Such User");
      break;
    case TFTP_ERRCODE_ILLEGAL_OPERATION:
      printf("Illegal Operation");
      break;
    default:
      printf("%u",packet->error.errorCode);
      break;
  }
  printf("] Message: %s\n",packet->error.message);
}
