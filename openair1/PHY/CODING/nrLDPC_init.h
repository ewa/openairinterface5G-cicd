/*==============================================================================
* nrLDPC_init.h
*
* Defines the function to initialize the LDPC decoder.
* Set correct LUTs
*
* Author: Sebastian Wagner
* Date: 17-11-2017
*
===============================================================================*/

#ifndef _NR_LDPC_INIT_
#define _NR_LDPC_INIT_

#include "./nrLDPC_lut/nrLDPC_lut.h"
#include "nrLDPC_defs.h"

static inline uint32_t nrLDPC_init(t_nrLDPC_dec_params* p_decParams, t_nrLDPC_lut* p_lut)
{
    uint32_t numLLR = 0;
    uint8_t BG = p_decParams->BG;
    uint16_t Z = p_decParams->Z;
    uint8_t R  = p_decParams->R;

    if (BG == 2)
    {
        // LUT that only depend on BG
        p_lut->startAddrCnGroups = lut_startAddrCnGroups_BG2;

        // LUT that only depend on R
        if (R == 15)
        {
            p_lut->numCnInCnGroups = lut_numCnInCnGroups_BG2_R15;
            p_lut->numBnInBnGroups = lut_numBnInBnGroups_BG2_R15;
            p_lut->startAddrBnGroups = lut_startAddrBnGroups_BG2_R15;
            p_lut->startAddrBnGroupsLlr = lut_startAddrBnGroupsLlr_BG2_R15;
            numLLR = NR_LDPC_NCOL_BG2_R15*Z;
        }
        else if (R == 13)
        {
            p_lut->numCnInCnGroups = lut_numCnInCnGroups_BG2_R13;
            p_lut->numBnInBnGroups = lut_numBnInBnGroups_BG2_R13;
            p_lut->startAddrBnGroups = lut_startAddrBnGroups_BG2_R13;
            p_lut->startAddrBnGroupsLlr = lut_startAddrBnGroupsLlr_BG2_R13;
            numLLR = NR_LDPC_NCOL_BG2_R13*Z;
        }
        else if (R == 23)
        {
            p_lut->numCnInCnGroups = lut_numCnInCnGroups_BG2_R23;
            p_lut->numBnInBnGroups = lut_numBnInBnGroups_BG2_R23;
            p_lut->startAddrBnGroups = lut_startAddrBnGroups_BG2_R23;
            p_lut->startAddrBnGroupsLlr = lut_startAddrBnGroupsLlr_BG2_R23;
            numLLR = NR_LDPC_NCOL_BG2_R23*Z;
        }

        // LUT that depend on Z and R
        if (Z == 128)
        {
            if (R == 15)
            {
                p_lut->llr2CnProcBuf  = lut_llr2CnProcBuf_BG2_Z128_R15;
                p_lut->numEdgesPerBn  = lut_numEdgesPerBn_BG2_Z128_R15;
                p_lut->cn2bnProcBuf   = lut_cn2bnProcBuf_BG2_Z128_R15;
                p_lut->llr2llrProcBuf = lut_llr2llrProcBuf_BG2_Z128_R15;
            }
            else if (R == 13)
            {
                p_lut->llr2CnProcBuf  = lut_llr2CnProcBuf_BG2_Z128_R13;
                p_lut->numEdgesPerBn  = lut_numEdgesPerBn_BG2_Z128_R13;
                p_lut->cn2bnProcBuf   = lut_cn2bnProcBuf_BG2_Z128_R13;
                p_lut->llr2llrProcBuf = lut_llr2llrProcBuf_BG2_Z128_R13;
            }
            else if (R == 23)
            {
                p_lut->llr2CnProcBuf  = lut_llr2CnProcBuf_BG2_Z128_R23;
                p_lut->numEdgesPerBn  = lut_numEdgesPerBn_BG2_Z128_R23;
                p_lut->cn2bnProcBuf   = lut_cn2bnProcBuf_BG2_Z128_R23;
                p_lut->llr2llrProcBuf = lut_llr2llrProcBuf_BG2_Z128_R23;
            }

        }
        else if (Z == 384)
        {
            if (R == 15)
            {
                p_lut->llr2CnProcBuf  = lut_llr2CnProcBuf_BG2_Z384_R15;
                p_lut->numEdgesPerBn  = lut_numEdgesPerBn_BG2_Z384_R15;
                p_lut->cn2bnProcBuf   = lut_cn2bnProcBuf_BG2_Z384_R15;
                p_lut->llr2llrProcBuf = lut_llr2llrProcBuf_BG2_Z384_R15;
            }
            else if (R == 13)
            {
                p_lut->llr2CnProcBuf  = lut_llr2CnProcBuf_BG2_Z384_R13;
                p_lut->numEdgesPerBn  = lut_numEdgesPerBn_BG2_Z384_R13;
                p_lut->cn2bnProcBuf   = lut_cn2bnProcBuf_BG2_Z384_R13;
                p_lut->llr2llrProcBuf = lut_llr2llrProcBuf_BG2_Z384_R13;
            }
            else if (R == 23)
            {
                p_lut->llr2CnProcBuf  = lut_llr2CnProcBuf_BG2_Z384_R23;
                p_lut->numEdgesPerBn  = lut_numEdgesPerBn_BG2_Z384_R23;
                p_lut->cn2bnProcBuf   = lut_cn2bnProcBuf_BG2_Z384_R23;
                p_lut->llr2llrProcBuf = lut_llr2llrProcBuf_BG2_Z384_R23;
            }
        }
    }

    return numLLR;
}

#endif
