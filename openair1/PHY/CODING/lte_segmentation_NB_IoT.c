/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.0  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/* file: lte_segmentation.c
   purpose: Procedures for transport block segmentation for LTE (turbo-coded transport channels)
   author: raymond.knopp@eurecom.fr
   date: 21.10.2009
*/
#include "PHY/defs_L1_NB_IoT.h"
#include "SCHED_NBIOT/sched_common_extern_NB_IoT.h"

//#define DEBUG_SEGMENTATION

int lte_segmentation_NB_IoT(unsigned char *input_buffer,
                            unsigned char **output_buffers,
                            unsigned int B,
                            unsigned int *C,
                            unsigned int *Cplus,
                            unsigned int *Cminus,
                            unsigned int *Kplus,
                            unsigned int *Kminus,
                            unsigned int *F)
{

  unsigned int L,Bprime,Bprime_by_C,r,Kr,k,s,crc;

  if (B<=6144) {
    L=0;
    *C=1;
    Bprime=B;
  } else {
    L=24;
    *C = B/(6144-L);

    if ((6144-L)*(*C) < B)
      *C=*C+1;

    Bprime = B+((*C)*L);
  }

  if ((*C)>MAX_NUM_DLSCH_SEGMENTS_NB_IoT) {
      LOG_E(PHY,"lte_segmentation.c: too many segments %d, B %d, L %d, Bprime %d\n",*C,B,L,Bprime);
    return(-1);
  }

  // Find K+
  Bprime_by_C  = Bprime/(*C);

  if (Bprime_by_C <= 40) {
    *Kplus = 40;
    *Kminus = 0;
  } else if (Bprime_by_C<=512) { // increase by 1 byte til here
    *Kplus = (Bprime_by_C>>3)<<3;
    *Kminus = Bprime_by_C-8;
  } else if (Bprime_by_C <=1024) { // increase by 2 bytes til here
    *Kplus = (Bprime_by_C>>4)<<4;

    if (*Kplus < Bprime_by_C)
      *Kplus = *Kplus + 16;

    *Kminus = (*Kplus - 16);
  } else if (Bprime_by_C <= 2048) { // increase by 4 bytes til here
    *Kplus = (Bprime_by_C>>5)<<5;

    if (*Kplus < Bprime_by_C)
      *Kplus = *Kplus + 32;

    *Kminus = (*Kplus - 32);
  } else if (Bprime_by_C <=6144 ) { // increase by 8 bytes til here

    *Kplus = (Bprime_by_C>>6)<<6;


    if (*Kplus < Bprime_by_C)
      *Kplus = *Kplus + 64;

    *Kminus = (*Kplus - 64);
  } else {
    msg("lte_segmentation.c: Illegal codeword size !!!\n");
    return(-1);
  }

  if (*C == 1) {
    *Cplus = *C;
    *Kminus = 0;
    *Cminus = 0;
  } else {

    //    printf("More than one segment (%d), exiting \n",*C);
    //    exit(-1);
    *Cminus = ((*C)*(*Kplus) - (Bprime))/((*Kplus) - (*Kminus));
    *Cplus  = (*C) - (*Cminus);
  }

  *F = ((*Cplus)*(*Kplus) + (*Cminus)*(*Kminus) - (Bprime));

  if ((input_buffer) && (output_buffers)) {

    for (k=0; k<*F>>3; k++) {
      output_buffers[0][k] = 0;
    }

    s=0;

    for (r=0; r<*C; r++) {

      if (r<*Cminus)
        Kr = *Kminus;
      else
        Kr = *Kplus;

      while (k<((Kr - L)>>3)) {
        output_buffers[r][k] = input_buffer[s];
	//	printf("encoding segment %d : byte %d (%d) => %d\n",r,k,Kr>>3,input_buffer[s]);
        k++;
        s++;
      }

      if (*C > 1) { // add CRC
        crc = crc24b_NB_IoT(output_buffers[r],Kr-24)>>8;
        output_buffers[r][(Kr-24)>>3] = ((uint8_t*)&crc)[2];
        output_buffers[r][1+((Kr-24)>>3)] = ((uint8_t*)&crc)[1];
        output_buffers[r][2+((Kr-24)>>3)] = ((uint8_t*)&crc)[0];

      }

      k=0;
    }
  }

  return(0);
}

