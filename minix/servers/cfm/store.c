#include "inc.h"
#include <minix/aes256.h>

/*===========================================================================*
 *		            sef_cb_init_fresh                                *
 *===========================================================================*/
int sef_cb_init_fresh(int UNUSED(type), sef_init_info_t* UNUSED(info))
{
	return(OK);
}

/*===========================================================================*
 *				do_cfm_get_hash				     *
 *===========================================================================*/
int do_cfm_get_hash(message *m)
{
  // Get the data to be hashed from the message
  unsigned int dataLen = m->m_cfm_sendrecv.dataLen;
  uint8_t data[dataLen];
  memcpy(data, m->m_cfm_sendrecv.data, dataLen);

  printf("do_cfm_get_hash received %c\n", data[0]);

  // Hash here and update data and dataLen respectively.
  uint8_t hash[16];
  unsigned int hashLen = sizeof(hash);

  aes256_context_t ctx;
  aes256_key_t key = {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
        };

  aes256_blk_t buf = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  aes256_init(&ctx, &key);
  //printf("Adress: %li\n", *address);

  //Handle longer messages spanning multiple blocks
  if(dataLen > 16){
      //Calculate number of rounds
      unsigned int rounds = dataLen/16;
      if((dataLen % 16) != 0){
          rounds = rounds + 1;
      }

      memcpy(&buf, data, sizeof(buf));
      aes256_encrypt_ecb(&ctx, &buf); //Encrypt first block

      //Encrypt remaining blocks
      unsigned int counter = 16;
      for(int i = 1; i < rounds; i++){
          //XOR Encrypted block with next block
          for(int j = 0; (j < 16) && (counter < dataLen); j++){
              counter = counter + 1;
              buf.raw[j] = buf.raw[j] ^ data[counter];
          }
          aes256_encrypt_ecb(&ctx, &buf);//Encrypt block again
      }
  }
  //Handle single block messages
  else{
      memcpy(&buf, data, dataLen);
      aes256_encrypt_ecb(&ctx, &buf);
  }
  
  //Copy final hash from buffer to hash
  memcpy(hash, &buf, sizeof(hash));

  // Update the message
  memset(m, 0, sizeof(message));
  memcpy(m->m_cfm_sendrecv.data, hash, hashLen);
  m->m_cfm_sendrecv.dataLen = hashLen;

  return (OK);
}
