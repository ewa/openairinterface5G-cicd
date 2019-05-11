#ifndef OPENAIRINTERFACE5G_LIMITS_H_
#define OPENAIRINTERFACE5G_LIMITS_H_

#if 1 /*defined(CBMIMO1) || defined(EXMIMO) || defined(OAI_USRP) || defined(OAI_LMSSDR) || defined(OAI_ADRV9371_ZC706)*/
#        define NUMBER_OF_eNB_MAX 1
#        define NUMBER_OF_gNB_MAX 1
#        define NUMBER_OF_RU_MAX 2
#        define NUMBER_OF_NR_RU_MAX 2
#        define NUMBER_OF_NR_DLSCH_MAX 16
#        define NUMBER_OF_NR_ULSCH_MAX 16
#        ifndef PHYSIM
#        ifndef UE_EXPANSION
#                define NUMBER_OF_UE_MAX 16
#                define NUMBER_OF_NR_UE_MAX 16
#                define NUMBER_OF_CONNECTED_eNB_MAX 3
#                define NUMBER_OF_CONNECTED_gNB_MAX 3
#        else
#                define NUMBER_OF_UE_MAX 256
#                define NUMBER_OF_NR_UE_MAX 256
#                define NUMBER_OF_CONNECTED_eNB_MAX 1
#                define NUMBER_OF_CONNECTED_gNB_MAX 1
#        endif
#        else
#                define NUMBER_OF_UE_MAX 1
#                define NUMBER_OF_NR_UE_MAX 1
#                define NUMBER_OF_CONNECTED_eNB_MAX 1
#                define NUMBER_OF_CONNECTED_gNB_MAX 1
#        endif
#else
#        define NUMBER_OF_eNB_MAX 7
#        define NUMBER_OF_gNB_MAX 7
#        define NUMBER_OF_RU_MAX 32
#        define NUMBER_OF_NR_RU_MAX 32
#        ifndef UE_EXPANSION
#                define NUMBER_OF_UE_MAX 20
#        	 define NUMBER_OF_NR_UE_MAX 20
#                define NUMBER_OF_CONNECTED_eNB_MAX 3
#        	 define NUMBER_OF_CONNECTED_gNB_MAX 3
#        else
#                define NUMBER_OF_UE_MAX 256
#        	 define NUMBER_OF_NR_UE_MAX 256
#                define NUMBER_OF_CONNECTED_eNB_MAX 1
#        	 define NUMBER_OF_CONNECTED_gNB_MAX 1
#        endif
#        if defined(STANDALONE) && STANDALONE==1
#                undef  NUMBER_OF_eNB_MAX
#                undef  NUMBER_OF_gNB_MAX

#                undef  NUMBER_OF_UE_MAX
#                undef  NUMBER_OF_NR_UE_MAX

#                undef  NUMBER_OF_RU_MAX
#                undef  NUMBER_OF_NR_RU_MAX

#                define NUMBER_OF_eNB_MAX 3
#                define NUMBER_OF_gNB_MAX 3

#                define NUMBER_OF_UE_MAX 3
#                define NUMBER_OF_NR_UE_MAX 3

#                define NUMBER_OF_RU_MAX 3
#                define NUMBER_OF_NR_RU_MAX 3
#        endif
#        if defined(LARGE_SCALE) && LARGE_SCALE
#                undef  NUMBER_OF_eNB_MAX
#                undef  NUMBER_OF_gNB_MAX

#                undef  NUMBER_OF_UE_MAX
#                undef  NUMBER_OF_NR_UE_MAX

#                undef  NUMBER_OF_CONNECTED_eNB_MAX
#                undef  NUMBER_OF_CONNECTED_gNB_MAX

#                undef  NUMBER_OF_RU_MAX
#                undef  NUMBER_OF_NR_RU_MAX

#                define NUMBER_OF_eNB_MAX 2
#                define NUMBER_OF_gNB_MAX 2

#                define NUMBER_OF_UE_MAX 120
#                define NUMBER_OF_NR_UE_MAX 120

#                define NUMBER_OF_RU_MAX 16
#                define NUMBER_OF_NR_RU_MAX 16

#                define NUMBER_OF_CONNECTED_eNB_MAX 1 // to save some memory
#                define NUMBER_OF_CONNECTED_gNB_MAX 1
#        endif
#endif

#endif /* OPENAIRINTERFACE5G_LIMITS_H_ */
