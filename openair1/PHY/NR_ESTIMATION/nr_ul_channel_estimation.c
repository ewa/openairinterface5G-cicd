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


#include <string.h>

#include "nr_ul_estimation.h"
#include "PHY/NR_REFSIG/nr_refsig.h"
#include "PHY/NR_UE_ESTIMATION/filt16a_32.h"

//#define DEBUG_CH


int nr_pusch_channel_estimation(PHY_VARS_gNB *gNB,
                                uint8_t gNB_offset,
                                unsigned char Ns,
                                unsigned short p,
                                unsigned char symbol,
                                unsigned short bwp_start_subcarrier,
                                unsigned short nb_rb_pusch)
{
  int pilot[1320] __attribute__((aligned(16)));
  unsigned char aarx;
  unsigned short k;
  int16_t ch[10],*pil,*rxF,*ul_ch;
  int ch_offset,symbol_offset, length_dmrs, UE_id = 0;
  unsigned short n_idDMRS[2] = {0,1}; //to update from pusch config
  int32_t temp_in_ifft_0[8192*2] __attribute__((aligned(16)));
  int32_t **ul_ch_estimates_time =  gNB->pusch_vars[UE_id]->ul_ch_estimates_time;

#ifdef DEBUG_CH
  FILE *debug_ch_est;
  debug_ch_est = fopen("debug_ch_est.txt","w");
#endif

  //uint16_t Nid_cell = (eNB_offset == 0) ? gNB->frame_parms.Nid_cell : gNB->measurements.adj_cell_id[eNB_offset-1];

  uint8_t nushift;
  int **ul_ch_estimates  = gNB->pusch_vars[UE_id]->ul_ch_estimates;
  int **rxdataF = gNB->common_vars.rxdataF;

  nushift = (p>>1)&1;
  gNB->frame_parms.nushift = nushift;

  ch_offset     = gNB->frame_parms.ofdm_symbol_size*symbol;

  symbol_offset = gNB->frame_parms.ofdm_symbol_size*symbol;

  k = bwp_start_subcarrier;
  int re_offset = k;


#ifdef DEBUG_CH
  printf("PUSCH Channel Estimation : gNB_offset %d ch_offset %d, symbol_offset %d OFDM size %d, Ncp=%d, l=%d, Ns=%d, k=%d symbol %d\n", gNB_offset,ch_offset,symbol_offset,gNB->frame_parms.ofdm_symbol_size,
         gNB->frame_parms.Ncp,l,Ns,k, symbol);
#endif


  //------------------generate DMRS------------------//

  length_dmrs = 1; //to update from pusch config

  nr_gold_pusch(gNB, symbol, n_idDMRS, length_dmrs);

  nr_pusch_dmrs_rx(gNB, Ns, gNB->nr_gold_pusch[gNB_offset][Ns][0], &pilot[0], 1000, 0, nb_rb_pusch);

  //------------------------------------------------//

  for (aarx=0; aarx<gNB->frame_parms.nb_antennas_rx; aarx++) {

    pil   = (int16_t *)&pilot[0];
    rxF   = (int16_t *)&rxdataF[aarx][(symbol_offset+k+nushift)];
    ul_ch = (int16_t *)&ul_ch_estimates[aarx][ch_offset];

    memset(ul_ch,0,4*(gNB->frame_parms.ofdm_symbol_size));

#ifdef DEBUG_PUSCH
    printf("ch est pilot addr %p RB_DL %d\n",&pilot[0], gNB->frame_parms.N_RB_UL);
    printf("k %d, first_carrier %d\n",k,gNB->frame_parms.first_carrier_offset);
    printf("rxF addr %p p %d\n", rxF,p);
    printf("ul_ch addr %p nushift %d\n",ul_ch,nushift);
#endif
    for (uint16_t idx8Sym=0; idx8Sym<(nb_rb_pusch*12)/8; idx8Sym++ ) {

      for (uint8_t idxPil=0; idxPil<10; idxPil+=2 ) {
        rxF   = (int16_t *)&rxdataF[aarx][(symbol_offset+nushift+re_offset)];
        ch[idxPil]   = (int16_t)(((int32_t)pil[idx8Sym*8+idxPil]*rxF[0] - (int32_t)pil[idx8Sym*8+idxPil+1]*rxF[1])>>15);
        ch[idxPil+1] = (int16_t)(((int32_t)pil[idx8Sym*8+idxPil]*rxF[1] + (int32_t)pil[idx8Sym*8+idxPil+1]*rxF[0])>>15);
        re_offset = (re_offset+2)&(gNB->frame_parms.ofdm_symbol_size-1);
      }
      
      simple_lerp(ch, ul_ch);
#ifdef DEBUG_PDSCH
      for(uint8_t i=0; i<16; i++) {
        printf("%d\t",ul_ch[i]);
      }
      printf("\n");
#endif
      ul_ch += 16;
      re_offset = (re_offset-2)&(gNB->frame_parms.ofdm_symbol_size-1);
    }

    // Do the same for the 2 left over pilots if any (nb_rb_pusch*12 mod 8)
    if ((nb_rb_pusch*12)%8) {
      uint16_t used_pils = (nb_rb_pusch*12);

      for (uint8_t idxPil=0; idxPil<8; idxPil+=2 ) {
        rxF   = (int16_t *)&rxdataF[aarx][(symbol_offset+nushift+re_offset)];
        ch[idxPil]   = (int16_t)(((int32_t)pil[used_pils+idxPil]*rxF[0] - (int32_t)pil[used_pils+idxPil+1]*rxF[1])>>15);
        ch[idxPil+1] = (int16_t)(((int32_t)pil[used_pils+idxPil]*rxF[1] + (int32_t)pil[used_pils+idxPil+1]*rxF[0])>>15);
        re_offset = (re_offset+2)&(gNB->frame_parms.ofdm_symbol_size-1);
      }

      ul_ch[2] = (ch[0]+ch[2]) >> 1;
      ul_ch[3] = (ch[1]+ch[3]) >> 1;
      ul_ch[6] = (ch[4]+ch[6]) >> 1;
      ul_ch[7] = (ch[5]+ch[7]) >> 1;

      ul_ch[0] = ch[0]; ul_ch[1] = ch[1];
      ul_ch[4] = ch[4]; ul_ch[5] = ch[5];
    }

    // check if PRB crosses DC and improve estimates around DC
    if ((bwp_start_subcarrier >= gNB->frame_parms.ofdm_symbol_size/2) && (bwp_start_subcarrier+nb_rb_pusch*12 >= gNB->frame_parms.ofdm_symbol_size)) {
      ul_ch = (int16_t *)&ul_ch_estimates[aarx][ch_offset];
      uint16_t idxDC = 2*(gNB->frame_parms.ofdm_symbol_size - bwp_start_subcarrier);

      // before DC
      ul_ch[idxDC-2] = ((ul_ch[idxDC-4] * 3072) + (ul_ch[idxDC+4] * 1024)) >> 12;
      ul_ch[idxDC-1] = ((ul_ch[idxDC-3] * 3072) + (ul_ch[idxDC+5] * 1024)) >> 12;

      //after DC
      ul_ch[idxDC+2] = ((ul_ch[idxDC+4] * 3072) + (ul_ch[idxDC-4] * 1024)) >> 12;
      ul_ch[idxDC+3] = ((ul_ch[idxDC+5] * 3072) + (ul_ch[idxDC-3] * 1024)) >> 12;

      // at DC
      ul_ch[idxDC]   = (ul_ch[idxDC-2] + ul_ch[idxDC+2]) >> 1;
      ul_ch[idxDC+1] = (ul_ch[idxDC-1] + ul_ch[idxDC+3]) >> 1;
    }

    // Convert to time domain
    memset(temp_in_ifft_0, 0, gNB->frame_parms.ofdm_symbol_size*sizeof(int32_t));
    memcpy(temp_in_ifft_0, &ul_ch_estimates[aarx][symbol_offset], nb_rb_pusch * NR_NB_SC_PER_RB * sizeof(int32_t));

    switch (gNB->frame_parms.ofdm_symbol_size) {
      case 128:
        idft128((int16_t*) temp_in_ifft_0,
               (int16_t*) ul_ch_estimates_time[aarx],
               1);
        break;

      case 256:
        idft256((int16_t*) temp_in_ifft_0,
               (int16_t*) ul_ch_estimates_time[aarx],
               1);
        break;

      case 512:
        idft512((int16_t*) temp_in_ifft_0,
               (int16_t*) ul_ch_estimates_time[aarx],
               1);
        break;

      case 1024:
        idft1024((int16_t*) temp_in_ifft_0,
               (int16_t*) ul_ch_estimates_time[aarx],
               1);
        break;

      case 1536:
        idft1536((int16_t*) temp_in_ifft_0,
               (int16_t*) ul_ch_estimates_time[aarx],
               1);
        break;

      case 2048:
        idft2048((int16_t*) temp_in_ifft_0,
               (int16_t*) ul_ch_estimates_time[aarx],
               1);
        break;

      case 4096:
        idft4096((int16_t*) temp_in_ifft_0,
               (int16_t*) ul_ch_estimates_time[aarx],
               1);
        break;

      case 8192:
        idft8192((int16_t*) temp_in_ifft_0,
               (int16_t*) ul_ch_estimates_time[aarx],
               1);
        break;

      default:
        idft512((int16_t*) temp_in_ifft_0,
               (int16_t*) ul_ch_estimates_time[aarx],
               1);
        break;
    }
  }

#ifdef DEBUG_CH
  fclose(debug_ch_est);
#endif

  return(0);
}

