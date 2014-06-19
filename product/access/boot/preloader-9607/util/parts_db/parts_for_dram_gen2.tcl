# variable zq_setting             0x000011be
# 0xDEADDEAD, /* For patching HW bug, Disable ZQ calibration, call plat_dep_* function */
# 0x000010b0, /* ODT 75ohm    OCD 80ohm */
# 0x000010fC, /* ODT 75ohm    OCD 60ohm */
# 0x000011be, /* ODT 50ohm    OCD 50ohm */
# 0x0000107c, /* ODT 60ohm    OCD 60ohm */
# 0x00000530, /* ODT 80ohm    OCD 80ohm */
# 0x000010ef, /* ODT 75ohm    OCD 75ohm */
# 0x00002c9e, /* ODT 85.5ohm  OCD 87.27ohm */
# 0x00002828, /* ODT 92.31ohm OCD 96ohm */
# 0x000008fC, /* ODT 150ohm   OCD 60ohm */
# 0x000036fe, /* ODT 50ohm    OCD 48ohm */
# 0x00002ffc, /* ODT 50ohm    OCD 60ohm */
# 0x000023a3, /* ODT 100ohm   OCD 100ohm */
# 0x000008a1  /* ODT 150ohm   OCD 150ohm */

namespace eval DRAM_GENERIC {
## dram information
    variable ddr_type               DDR2 ;# 1-DDR1, 2-DDR2, 3-DDR3
## MCR
    variable IPREF                  0
    variable DPREF                  0
## DCR
    variable BANKCNT                0
    variable DBUSWID                0
    variable ROWCNT                 0
    variable COLCNT                 0
    variable DCHIPSEL               0
    variable FAST_RX                0
    variable BSTREF                 0
## DTR input
    variable refi_ns                7800
    variable rp_ns                  15
    variable rcd_ns                 15
    variable ras_ns                 45
    variable rfc_ns                 328
    variable wr_ns                  15
    variable rrd_ns                 10
    variable fawg_ns                50
    variable wtr_ns                 8
    variable rtp_ns                 8
## MPMR0
    variable PM_MODE                0x0
    variable T_CKE                  0xf
    variable T_RSD                  0x3ff
    variable T_XSREF                0x3ff
## MPMR1
    variable T_XARD                 0xf
    variable T_AXPD                 0xf
## DIDER
    variable DQS0_EN_HCLK           0x0
    variable DQS0_EN_TAP            0x0
    variable DQS1_EN_HCLK           0x0
    variable DQS1_EN_TAP            0x0
## D23OSCR
    variable ODT_ALWAYS_ON          0x0
    variable TE_ALWAYS_ON           0x0
## DACCR
    variable AC_MODE                0x1
    variable DQS_SE                 0x1
    variable DQS0_GROUP_TAP         0x0
    variable DQS1_GROUP_TAP         0x0
    variable AC_DYN_BPTR_CLR_EN     0x0
    variable AC_BPTR_CLEAR          0x1
    variable AC_DEBUG_SEL           0x0
## DACSPCR
    variable AC_SILEN_PERIOD_EN     0x0
    variable AC_SILEN_TRIG          0x0
    variable AC_SILEN_PERIOD_UNIT   0x0
    variable AC_SILEN_PERIOD        0x0
    variable AC_SILEN_LEN           0x7f
## DACSPAR
    variable dacspar                0xa1a00000

    variable static_cal_data_0      0x0
    variable static_cal_data_1      0x0
    variable static_cal_data_2      0x0
    variable static_cal_data_3      0x0
    variable static_cal_data_4      0x0
    variable static_cal_data_5      0x0
    variable static_cal_data_6      0x0
    variable static_cal_data_7      0x0
    variable static_cal_data_8      0x0
    variable static_cal_data_9      0x0
    variable static_cal_data_10     0x0
    variable static_cal_data_11     0x0
    variable static_cal_data_12     0x0
    variable static_cal_data_13     0x0
    variable static_cal_data_14     0x0
    variable static_cal_data_15     0x0
    variable static_cal_data_16     0x0
    variable static_cal_data_17     0x0
    variable static_cal_data_18     0x0
    variable static_cal_data_19     0x0
    variable static_cal_data_20     0x0
    variable static_cal_data_21     0x0
    variable static_cal_data_22     0x0
    variable static_cal_data_23     0x0
    variable static_cal_data_24     0x0
    variable static_cal_data_25     0x0
    variable static_cal_data_26     0x0
    variable static_cal_data_27     0x0
    variable static_cal_data_28     0x0
    variable static_cal_data_29     0x0
    variable static_cal_data_30     0x0
    variable static_cal_data_31     0x0
    variable static_cal_data_32     0x0

    variable zq_setting             0x000011be

    variable tx_clk_phs_delay       0x0
    variable clkm_delay             0x0
    variable clkm90_delay           0x0
    variable calibration_type       software ;# software or static
    
    variable auto_calibration       disable


    variable mrs_dll_enable         dll_enable ;#dll_enable->0, dll_disable->1
    variable mrs_drv_strength       0          ;#(DDR2) 0->normal(18 ohm), 1->reduced(40 ohm); (DDR3-OCD) 0->240/6=40 ohm, 1->240/7=34 ohm
    variable mrs_odt                120        ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3-rtt_nom) 0, 20, 30, 40, 60, 120
    variable mrs_additive_latency   0          ;#(DDR1) none, (DDR2) 0~5, DDR3 0~2
    variable mrs_rtt_wr             0          ;#(DDR3) 0->disable, 60, 120
}

namespace eval NANYA_NT5CB256M8GN {
## dram information
    variable ddr_type               DDR3 ;# 1-DDR1, 2-DDR2, 3-DDR3
## MCR
    variable IPREF                  0
    variable DPREF                  0
## DCR
    variable BANKCNT                2;#8 banks
    variable DBUSWID                0;#8-bit
    variable ROWCNT                 5;# 64KB
    variable COLCNT                 2;#1KB
    variable DCHIPSEL               0
    variable FAST_RX                0
    variable BSTREF                 0
## DTR input
    variable refi_ns                7800
    variable rp_ns                  15
    variable rcd_ns                 15
    variable ras_ns                 45
    variable rfc_ns                 328
    variable wr_ns                  15
    variable rrd_ns                 10
    variable fawg_ns                50
    variable wtr_ns                 8
    variable rtp_ns                 8
## MPMR0
    variable PM_MODE                0x0
    variable T_CKE                  0xf
    variable T_RSD                  0x3ff
    variable T_XSREF                0x3ff
## MPMR1
    variable T_XARD                 0xf
    variable T_AXPD                 0xf
## DIDER
    variable DQS0_EN_HCLK           0x0
    variable DQS0_EN_TAP            0x0
    variable DQS1_EN_HCLK           0x0
    variable DQS1_EN_TAP            0x0
## D23OSCR
    variable ODT_ALWAYS_ON          0x0
    variable TE_ALWAYS_ON           0x0
## DACCR
    variable AC_MODE                0x1
    variable DQS_SE                 0x1
    variable DQS0_GROUP_TAP         0x0
    variable DQS1_GROUP_TAP         0x0
    variable AC_DYN_BPTR_CLR_EN     0x0
    variable AC_BPTR_CLEAR          0x1
    variable AC_DEBUG_SEL           0x0
## DACSPCR
    variable AC_SILEN_PERIOD_EN     0x0
    variable AC_SILEN_TRIG          0x0
    variable AC_SILEN_PERIOD_UNIT   0x0
    variable AC_SILEN_PERIOD        0x0
    variable AC_SILEN_LEN           0x7f
## DACSPAR
    variable dacspar                0xa1a00000

    variable static_cal_data_0     0x001E0F00
    variable static_cal_data_1     0x001E0F00
    variable static_cal_data_2     0x001E0F00
    variable static_cal_data_3     0x001E0F00
    variable static_cal_data_4     0x001E0F00
    variable static_cal_data_5     0x001E0F00
    variable static_cal_data_6     0x001E0F00
    variable static_cal_data_7     0x001E0F00
    variable static_cal_data_8     0x001E0F00
    variable static_cal_data_9     0x001E0F00
    variable static_cal_data_10    0x001E0F00
    variable static_cal_data_11    0x001E0F00
    variable static_cal_data_12    0x001E0F00
    variable static_cal_data_13    0x001E0F00
    variable static_cal_data_14    0x001E0F00
    variable static_cal_data_15    0x001E0F00
    variable static_cal_data_16    0x001E0F00
    variable static_cal_data_17    0x001E0F00
    variable static_cal_data_18    0x001E0F00
    variable static_cal_data_19    0x001E0F00
    variable static_cal_data_20    0x001E0F00
    variable static_cal_data_21    0x001E0F00
    variable static_cal_data_22    0x001E0F00
    variable static_cal_data_23    0x001E0F00
    variable static_cal_data_24    0x001E0F00
    variable static_cal_data_25    0x001E0F00
    variable static_cal_data_26    0x001E0F00
    variable static_cal_data_27    0x001E0F00
    variable static_cal_data_28    0x001E0F00
    variable static_cal_data_29    0x001E0F00
    variable static_cal_data_30    0x001E0F00
    variable static_cal_data_31    0x001E0F00
    variable static_cal_data_32    0x00000000

    variable zq_setting             0x000011be

    variable tx_clk_phs_delay       0x0
    variable clkm_delay             0x0
    variable clkm90_delay           0x0
    variable calibration_type       software ;# software or static
    
    variable auto_calibration       disable


    variable mrs_dll_enable         dll_enable ;#dll_enable->0, dll_disable->1
    variable mrs_drv_strength       normal     ;#normal->0, reduced->1
    variable mrs_odt                120        ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3) 0, 20, 30, 40, 60, 120
    variable mrs_additive_latency   0          ;#(DDR1) none, (DDR2) 0~5, DDR3 0~2
    variable mrs_rtt_wr             0          ;#(DDR3) 0->disable, 60, 120
}

namespace eval NANYA_NT5TU128M8GE-AC {
## dram information
    variable ddr_type               DDR2 ;# 1-DDR1, 2-DDR2, 3-DDR3
## MCR
    variable IPREF                  0
    variable DPREF                  0
## DCR
    variable BANKCNT                2;#8 banks
    variable DBUSWID                0;#8-bit
    variable ROWCNT                 3;#16KB
    variable COLCNT                 2;#1KB
    variable DCHIPSEL               0
    variable FAST_RX                0
    variable BSTREF                 0
## DTR input
    variable refi_ns                7800
    variable rp_ns                  15
    variable rcd_ns                 15
    variable ras_ns                 45
    variable rfc_ns                 328
    variable wr_ns                  15
    variable rrd_ns                 10
    variable fawg_ns                50
    variable wtr_ns                 8
    variable rtp_ns                 8

## MPMR0
    variable PM_MODE                0x0
    variable T_CKE                  0xf
    variable T_RSD                  0x3ff
    variable T_XSREF                0x3ff
## MPMR1
    variable T_XARD                 0xf
    variable T_AXPD                 0xf
## DIDER
    variable DQS0_EN_HCLK           0x1
    variable DQS0_EN_TAP            0x0
    variable DQS1_EN_HCLK           0x0
    variable DQS1_EN_TAP            0x0
## D23OSCR
    variable ODT_ALWAYS_ON          0x0
    variable TE_ALWAYS_ON           0x0
## DACCR
    variable AC_MODE                0x1
    variable DQS_SE                 0x1
    variable DQS0_GROUP_TAP         0x0
    variable DQS1_GROUP_TAP         0x0
    variable AC_DYN_BPTR_CLR_EN     0x0
    variable AC_BPTR_CLEAR          0x1
    variable AC_DEBUG_SEL           0x0
## DACSPCR
    variable AC_SILEN_PERIOD_EN     0x0
    variable AC_SILEN_TRIG          0x0
    variable AC_SILEN_PERIOD_UNIT   0x0
    variable AC_SILEN_PERIOD        0x0
    variable AC_SILEN_LEN           0x7f
## DACSPAR
    variable dacspar                0xa1a00000

    variable static_cal_data_0     0x0A1E0F00
    variable static_cal_data_1     0x0A1E0F00
    variable static_cal_data_2     0x0A1E0F00
    variable static_cal_data_3     0x0A1E0F00
    variable static_cal_data_4     0x0A1E0F00
    variable static_cal_data_5     0x0A1E0F00
    variable static_cal_data_6     0x0A1E0F00
    variable static_cal_data_7     0x0A1E0F00
    variable static_cal_data_8     0x001E0F00
    variable static_cal_data_9     0x001E0F00
    variable static_cal_data_10    0x001E0F00
    variable static_cal_data_11    0x001E0F00
    variable static_cal_data_12    0x001E0F00
    variable static_cal_data_13    0x001E0F00
    variable static_cal_data_14    0x001E0F00
    variable static_cal_data_15    0x001E0F00
    variable static_cal_data_16    0x001E0F00
    variable static_cal_data_17    0x001E0F00
    variable static_cal_data_18    0x001E0F00
    variable static_cal_data_19    0x001E0F00
    variable static_cal_data_20    0x001E0F00
    variable static_cal_data_21    0x001E0F00
    variable static_cal_data_22    0x001E0F00
    variable static_cal_data_23    0x001E0F00
    variable static_cal_data_24    0x001E0F00
    variable static_cal_data_25    0x001E0F00
    variable static_cal_data_26    0x001E0F00
    variable static_cal_data_27    0x001E0F00
    variable static_cal_data_28    0x001E0F00
    variable static_cal_data_29    0x001E0F00
    variable static_cal_data_30    0x001E0F00
    variable static_cal_data_31    0x001E0F00
    variable static_cal_data_32    0x08000000

    variable zq_setting             0x0000107C

    variable tx_clk_phs_delay       19
    variable clkm_delay             13
    variable clkm90_delay           10
    variable calibration_type       software ;# software or static
    
    variable auto_calibration       disable


    variable mrs_dll_enable         dll_enable ;#dll_enable->0, dll_disable->1
    variable mrs_drv_strength       normal     ;#normal->0, reduced->1
    variable mrs_odt                150        ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3) 0, 20, 30, 40, 60, 120
    variable mrs_additive_latency   0          ;#(DDR1) none, (DDR2) 0~5, DDR3 0~2
    variable mrs_rtt_wr             120        ;#(DDR3) 0->disable, 60, 120
}

namespace eval NANYA_NT5CB128MHP {
## dram information
    variable ddr_type               DDR3 ;# 1-DDR1, 2-DDR2, 3-DDR3
## MCR
    variable IPREF                  0
    variable DPREF                  0
## DCR
    variable BANKCNT                2
    variable DBUSWID                1
    variable ROWCNT                 3;# 16K
    variable COLCNT                 2
    variable DCHIPSEL               0
    variable FAST_RX                0
    variable BSTREF                 0
## DTR input
    variable refi_ns                7800
    variable rp_ns                  15
    variable rcd_ns                 15
    variable ras_ns                 45
    variable rfc_ns                 328
    variable wr_ns                  15
    variable rrd_ns                 10
    variable fawg_ns                50
    variable wtr_ns                 8
    variable rtp_ns                 8

## MPMR0
    variable PM_MODE                0x0
    variable T_CKE                  0xf
    variable T_RSD                  0x3ff
    variable T_XSREF                0x3ff
## MPMR1
    variable T_XARD                 0xf
    variable T_AXPD                 0xf
## DIDER
    variable DQS0_EN_HCLK           0x0
    variable DQS0_EN_TAP            0x0
    variable DQS1_EN_HCLK           0x0
    variable DQS1_EN_TAP            0x0
## D23OSCR
    variable ODT_ALWAYS_ON          0x0
    variable TE_ALWAYS_ON           0x0
## DACCR
    variable AC_MODE                0x1
    variable DQS_SE                 0x1
    variable DQS0_GROUP_TAP         0x0
    variable DQS1_GROUP_TAP         0x0
    variable AC_DYN_BPTR_CLR_EN     0x0
    variable AC_BPTR_CLEAR          0x1
    variable AC_DEBUG_SEL           0x0
## DACSPCR
    variable AC_SILEN_PERIOD_EN     0x0
    variable AC_SILEN_TRIG          0x0
    variable AC_SILEN_PERIOD_UNIT   0x0
    variable AC_SILEN_PERIOD        0x0
    variable AC_SILEN_LEN           0x7f
## DACSPAR
    variable dacspar                0xa1a00000

    variable static_cal_data_0     0x001E0F00
    variable static_cal_data_1     0x001E0F00
    variable static_cal_data_2     0x001E0F00
    variable static_cal_data_3     0x001E0F00
    variable static_cal_data_4     0x001E0F00
    variable static_cal_data_5     0x001E0F00
    variable static_cal_data_6     0x001E0F00
    variable static_cal_data_7     0x001E0F00
    variable static_cal_data_8     0x001E0F00
    variable static_cal_data_9     0x001E0F00
    variable static_cal_data_10    0x001E0F00
    variable static_cal_data_11    0x001E0F00
    variable static_cal_data_12    0x001E0F00
    variable static_cal_data_13    0x001E0F00
    variable static_cal_data_14    0x001E0F00
    variable static_cal_data_15    0x001E0F00
    variable static_cal_data_16    0x001E0F00
    variable static_cal_data_17    0x001E0F00
    variable static_cal_data_18    0x001E0F00
    variable static_cal_data_19    0x001E0F00
    variable static_cal_data_20    0x001E0F00
    variable static_cal_data_21    0x001E0F00
    variable static_cal_data_22    0x001E0F00
    variable static_cal_data_23    0x001E0F00
    variable static_cal_data_24    0x001E0F00
    variable static_cal_data_25    0x001E0F00
    variable static_cal_data_26    0x001E0F00
    variable static_cal_data_27    0x001E0F00
    variable static_cal_data_28    0x001E0F00
    variable static_cal_data_29    0x001E0F00
    variable static_cal_data_30    0x001E0F00
    variable static_cal_data_31    0x001E0F00
    variable static_cal_data_32    0x00000000

    variable zq_setting             0x000011be

    variable tx_clk_phs_delay       0x0
    variable clkm_delay             0x0
    variable clkm90_delay           0x0
    variable calibration_type       software ;# software or static
    
    variable auto_calibration       disable


    variable mrs_dll_enable         dll_enable ;#dll_enable->0, dll_disable->1
    variable mrs_drv_strength       normal     ;#normal->0, reduced->1
    variable mrs_odt                120        ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3) 0, 20, 30, 40, 60, 120
    variable mrs_additive_latency   0          ;#(DDR1) none, (DDR2) 0~5, DDR3 0~2
    variable mrs_rtt_wr             0          ;#(DDR3) 0->disable, 60, 120
}

namespace eval SAMSUNG_K4B2G0846D-HCK0 {
    variable ddr_type              DDR3 ;# 1-DDR1, 2-DDR2, 3-DDR3
    ## MCR
    variable IPREF                 0
    variable DPREF                 0
    ## DCR
    variable BANKCNT               2 ;#8 banks
    variable DBUSWID               0 ;#0:8bit
    variable ROWCNT                4 ;# 32K
    variable COLCNT                2 ;#1K
    variable DCHIPSEL              0
    variable FAST_RX               0
    variable BSTREF                0
    ## DTR input
    variable refi_ns               7800
    variable rp_ns                 15
    variable rcd_ns                15
    variable ras_ns                45
    variable rfc_ns                328
    variable wr_ns                 15
    variable rrd_ns                10
    variable fawg_ns               50
    variable wtr_ns                10
    variable rtp_ns                8

    ## MPMR0
    variable PM_MODE               0x0
    variable T_CKE                 0xf
    variable T_RSD                 0x3ff
    variable T_XSREF               0x3ff
    ## MPMR1
    variable T_XARD                0xf
    variable T_AXPD                0xf
    ## DIDER
    variable DQS0_EN_HCLK          0x1
    variable DQS0_EN_TAP           0x0
    variable DQS1_EN_HCLK          0x0
    variable DQS1_EN_TAP           0x0
    ## D23OSCR
    variable ODT_ALWAYS_ON         0x0
    variable TE_ALWAYS_ON          0x0
    ## DACCR
    variable AC_MODE               0x1
    variable DQS_SE                0x0
    variable DQS0_GROUP_TAP        0x0
    variable DQS1_GROUP_TAP        0x0
    variable AC_DYN_BPTR_CLR_EN    0x0
    variable AC_BPTR_CLEAR         0x1
    variable AC_DEBUG_SEL          0x0
    ## DACSPCR
    variable AC_SILEN_PERIOD_EN    0x0
    variable AC_SILEN_TRIG         0x0
    variable AC_SILEN_PERIOD_UNIT  0x0
    variable AC_SILEN_PERIOD       0x0
    variable AC_SILEN_LEN          0x7f
    ## DACSPAR
    variable dacspar                0xa1a00000

    variable static_cal_data_0     0x0a1E0F00
    variable static_cal_data_1     0x0a1E0F00
    variable static_cal_data_2     0x0a1E0F00
    variable static_cal_data_3     0x0a1E0F00
    variable static_cal_data_4     0x0a1E0F00
    variable static_cal_data_5     0x0a1E0F00
    variable static_cal_data_6     0x0a1E0F00
    variable static_cal_data_7     0x0a1E0F00
    variable static_cal_data_8     0x001E0F00
    variable static_cal_data_9     0x001E0F00
    variable static_cal_data_10    0x001E0F00
    variable static_cal_data_11    0x001E0F00
    variable static_cal_data_12    0x001E0F00
    variable static_cal_data_13    0x001E0F00
    variable static_cal_data_14    0x001E0F00
    variable static_cal_data_15    0x001E0F00
    variable static_cal_data_16    0x001E0F00
    variable static_cal_data_17    0x001E0F00
    variable static_cal_data_18    0x001E0F00
    variable static_cal_data_19    0x001E0F00
    variable static_cal_data_20    0x001E0F00
    variable static_cal_data_21    0x001E0F00
    variable static_cal_data_22    0x001E0F00
    variable static_cal_data_23    0x001E0F00
    variable static_cal_data_24    0x001E0F00
    variable static_cal_data_25    0x001E0F00
    variable static_cal_data_26    0x001E0F00
    variable static_cal_data_27    0x001E0F00
    variable static_cal_data_28    0x001E0F00
    variable static_cal_data_29    0x001E0F00
    variable static_cal_data_30    0x001E0F00
    variable static_cal_data_31    0x001E0F00
    variable static_cal_data_32    0x08000000

    variable zq_setting             0x0000107c

    variable tx_clk_phs_delay      17
    variable clkm_delay            13
    variable clkm90_delay          10
    variable calibration_type      software ;#static or software

    variable auto_calibration      disable

    variable mrs_dll_enable        0   ;#dll_enable->0, dll_disable->1
    variable mrs_drv_strength      1   ;#RZQ/6->0, RZQ/7->1
    variable mrs_odt               120 ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3) 0, 20, 30, 40, 60, 120
    variable mrs_additive_latency  0   ;#(DDR1) none, (DDR2) 0~5, DDR3 0~2
    variable mrs_rtt_wr            120 ;#(DDR3) 0->disable, 60, 120
}

namespace eval SAMSUNG_K4B1G0846G-BCH9 {
    variable ddr_type              DDR3 ;# 1-DDR1, 2-DDR2, 3-DDR3
    ## MCR
    variable IPREF                 0
    variable DPREF                 0
    ## DCR
    variable BANKCNT               2 ;#8 banks
    variable DBUSWID               0 ;#0:8bit
    variable ROWCNT                3 ;# 16K
    variable COLCNT                2 ;#1K
    variable DCHIPSEL              0
    variable FAST_RX               0
    variable BSTREF                0
    ## DTR input
    variable refi_ns               7800
    variable rp_ns                 15
    variable rcd_ns                15
    variable ras_ns                45
    variable rfc_ns                328
    variable wr_ns                 15
    variable rrd_ns                10
    variable fawg_ns               50
    variable wtr_ns                10
    variable rtp_ns                8

    ## MPMR0
    variable PM_MODE               0x0
    variable T_CKE                 0xf
    variable T_RSD                 0x3ff
    variable T_XSREF               0x3ff
    ## MPMR1
    variable T_XARD                0xf
    variable T_AXPD                0xf
    ## DIDER
    variable DQS0_EN_HCLK          0x1
    variable DQS0_EN_TAP           0x0
    variable DQS1_EN_HCLK          0x0
    variable DQS1_EN_TAP           0x0
    ## D23OSCR
    variable ODT_ALWAYS_ON         0x0
    variable TE_ALWAYS_ON          0x0
    ## DACCR
    variable AC_MODE               0x1
    variable DQS_SE                0x0
    variable DQS0_GROUP_TAP        0x0
    variable DQS1_GROUP_TAP        0x0
    variable AC_DYN_BPTR_CLR_EN    0x0
    variable AC_BPTR_CLEAR         0x1
    variable AC_DEBUG_SEL          0x0
    ## DACSPCR
    variable AC_SILEN_PERIOD_EN    0x0
    variable AC_SILEN_TRIG         0x0
    variable AC_SILEN_PERIOD_UNIT  0x0
    variable AC_SILEN_PERIOD       0x0
    variable AC_SILEN_LEN          0x7f
    ## DACSPAR
    variable dacspar                0xa1a00000

    variable static_cal_data_0     0x0a1E0F00
    variable static_cal_data_1     0x0a1E0F00
    variable static_cal_data_2     0x0a1E0F00
    variable static_cal_data_3     0x0a1E0F00
    variable static_cal_data_4     0x0a1E0F00
    variable static_cal_data_5     0x0a1E0F00
    variable static_cal_data_6     0x0a1E0F00
    variable static_cal_data_7     0x0a1E0F00
    variable static_cal_data_8     0x001E0F00
    variable static_cal_data_9     0x001E0F00
    variable static_cal_data_10    0x001E0F00
    variable static_cal_data_11    0x001E0F00
    variable static_cal_data_12    0x001E0F00
    variable static_cal_data_13    0x001E0F00
    variable static_cal_data_14    0x001E0F00
    variable static_cal_data_15    0x001E0F00
    variable static_cal_data_16    0x001E0F00
    variable static_cal_data_17    0x001E0F00
    variable static_cal_data_18    0x001E0F00
    variable static_cal_data_19    0x001E0F00
    variable static_cal_data_20    0x001E0F00
    variable static_cal_data_21    0x001E0F00
    variable static_cal_data_22    0x001E0F00
    variable static_cal_data_23    0x001E0F00
    variable static_cal_data_24    0x001E0F00
    variable static_cal_data_25    0x001E0F00
    variable static_cal_data_26    0x001E0F00
    variable static_cal_data_27    0x001E0F00
    variable static_cal_data_28    0x001E0F00
    variable static_cal_data_29    0x001E0F00
    variable static_cal_data_30    0x001E0F00
    variable static_cal_data_31    0x001E0F00
    variable static_cal_data_32    0x08000000

    variable zq_setting             0x0000107c

    variable tx_clk_phs_delay      17
    variable clkm_delay            13
    variable clkm90_delay          10
    variable calibration_type      software ;#static or software

    variable auto_calibration      disable

    variable mrs_dll_enable        0   ;#dll_enable->0, dll_disable->1
    variable mrs_drv_strength      1   ;#RZQ/6->0, RZQ/7->1
    variable mrs_odt               120 ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3) 0, 20, 30, 40, 60, 120
    variable mrs_additive_latency  0   ;#(DDR1) none, (DDR2) 0~5, DDR3 0~2
    variable mrs_rtt_wr            120 ;#(DDR3) 0->disable, 60, 120
}

namespace eval SAMSUNG_K4B1G1646G-BCH9 {
    variable ddr_type              DDR3 ;# 1-DDR1, 2-DDR2, 3-DDR3
    ## MCR
    variable IPREF                 0
    variable DPREF                 0
    ## DCR
    variable BANKCNT               2
    variable DBUSWID               1 ;#0:8bit, 1:16bit
    variable ROWCNT                2 ;# 16K
    variable COLCNT                2
    variable DCHIPSEL              0
    variable FAST_RX               0
    variable BSTREF                0
    ## DTR input
    variable refi_ns               7800
    variable rp_ns                 15
    variable rcd_ns                15
    variable ras_ns                45
    variable rfc_ns                328
    variable wr_ns                 15
    variable rrd_ns                10
    variable fawg_ns               50
    variable wtr_ns                8
    variable rtp_ns                8

    ## MPMR0
    variable PM_MODE               0x0
    variable T_CKE                 0xf
    variable T_RSD                 0x3ff
    variable T_XSREF               0x3ff
    ## MPMR1
    variable T_XARD                0xf
    variable T_AXPD                0xf
    ## DIDER
    variable DQS0_EN_HCLK          0x0
    variable DQS0_EN_TAP           0x0
    variable DQS1_EN_HCLK          0x0
    variable DQS1_EN_TAP           0x0
    ## D23OSCR
    variable ODT_ALWAYS_ON         0x0
    variable TE_ALWAYS_ON          0x0
    ## DACCR
    variable AC_MODE               0x1
    variable DQS_SE                0x0
    variable DQS0_GROUP_TAP        0x0
    variable DQS1_GROUP_TAP        0x0
    variable AC_DYN_BPTR_CLR_EN    0x0
    variable AC_BPTR_CLEAR         0x1
    variable AC_DEBUG_SEL          0x0
    ## DACSPCR
    variable AC_SILEN_PERIOD_EN    0x0
    variable AC_SILEN_TRIG         0x0
    variable AC_SILEN_PERIOD_UNIT  0x0
    variable AC_SILEN_PERIOD       0x0
    variable AC_SILEN_LEN          0x7f
    ## DACSPAR
    variable dacspar                0xa1a00000

    variable static_cal_data_0     0x001E0F00
    variable static_cal_data_1     0x001E0F00
    variable static_cal_data_2     0x001E0F00
    variable static_cal_data_3     0x001E0F00
    variable static_cal_data_4     0x001E0F00
    variable static_cal_data_5     0x001E0F00
    variable static_cal_data_6     0x001E0F00
    variable static_cal_data_7     0x001E0F00
    variable static_cal_data_8     0x001E0F00
    variable static_cal_data_9     0x001E0F00
    variable static_cal_data_10    0x001E0F00
    variable static_cal_data_11    0x001E0F00
    variable static_cal_data_12    0x001E0F00
    variable static_cal_data_13    0x001E0F00
    variable static_cal_data_14    0x001E0F00
    variable static_cal_data_15    0x001E0F00
    variable static_cal_data_16    0x001E0F00
    variable static_cal_data_17    0x001E0F00
    variable static_cal_data_18    0x001E0F00
    variable static_cal_data_19    0x001E0F00
    variable static_cal_data_20    0x001E0F00
    variable static_cal_data_21    0x001E0F00
    variable static_cal_data_22    0x001E0F00
    variable static_cal_data_23    0x001E0F00
    variable static_cal_data_24    0x001E0F00
    variable static_cal_data_25    0x001E0F00
    variable static_cal_data_26    0x001E0F00
    variable static_cal_data_27    0x001E0F00
    variable static_cal_data_28    0x001E0F00
    variable static_cal_data_29    0x001E0F00
    variable static_cal_data_30    0x001E0F00
    variable static_cal_data_31    0x001E0F00
    variable static_cal_data_32    0x00000000

    variable zq_setting             0x000011be

    variable tx_clk_phs_delay      19
    variable clkm_delay            13
    variable clkm90_delay          10
    variable calibration_type      software ;#static or software

    variable auto_calibration      disable

    variable mrs_dll_enable        dll_enable  ;#dll_enable->0, dll_disable->1
    variable mrs_drv_strength      reduced     ;#normal->0, reduced->1
    variable mrs_odt               120         ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3) 0, 20, 30, 40, 60, 120
    variable mrs_additive_latency  0           ;#(DDR1) none, (DDR2) 0~5, DDR3 0~2
    variable mrs_rtt_wr             0          ;#(DDR3) 0->disable, 60, 120
}

namespace eval SAMSUNG_K4T1G084QF-BCF8 {
    variable ddr_type              DDR2 ;# 1-DDR1, 2-DDR2, 3-DDR3
    ## MCR
    variable IPREF                 0
    variable DPREF                 0
    ## DCR
    variable BANKCNT               2
    variable DBUSWID               0 ;#0:8bit, 1:16bit
    variable ROWCNT                3 ;# 16K
    variable COLCNT                2
    variable DCHIPSEL              0
    variable FAST_RX               0
    variable BSTREF                0
    ## DTR input
    variable refi_ns               7800
    variable rp_ns                 15
    variable rcd_ns                15
    variable ras_ns                45
    variable rfc_ns                328
    variable wr_ns                 15
    variable rrd_ns                10
    variable fawg_ns               50
    variable wtr_ns                8
    variable rtp_ns                8

## MPMR0
    variable PM_MODE                0x0
    variable T_CKE                  0xf
    variable T_RSD                  0x3ff
    variable T_XSREF                0x3ff
## MPMR1
    variable T_XARD                 0xf
    variable T_AXPD                 0xf
## DIDER
    variable DQS0_EN_HCLK           0x1
    variable DQS0_EN_TAP            0x0
    variable DQS1_EN_HCLK           0x0
    variable DQS1_EN_TAP            0x0
## D23OSCR
    variable ODT_ALWAYS_ON          0x0
    variable TE_ALWAYS_ON           0x0
## DACCR
    variable AC_MODE                0x1
    variable DQS_SE                 0x1
    variable DQS0_GROUP_TAP         0x0
    variable DQS1_GROUP_TAP         0x0
    variable AC_DYN_BPTR_CLR_EN     0x0
    variable AC_BPTR_CLEAR          0x1
    variable AC_DEBUG_SEL           0x0
## DACSPCR
    variable AC_SILEN_PERIOD_EN     0x0
    variable AC_SILEN_TRIG          0x0
    variable AC_SILEN_PERIOD_UNIT   0x0
    variable AC_SILEN_PERIOD        0x0
    variable AC_SILEN_LEN           0x7f
## DACSPAR
    variable dacspar                0xa1a00000

    variable static_cal_data_0     0x001E0F00
    variable static_cal_data_1     0x001E0F00
    variable static_cal_data_2     0x001E0F00
    variable static_cal_data_3     0x001E0F00
    variable static_cal_data_4     0x001E0F00
    variable static_cal_data_5     0x001E0F00
    variable static_cal_data_6     0x001E0F00
    variable static_cal_data_7     0x001E0F00
    variable static_cal_data_8     0x001E0F00
    variable static_cal_data_9     0x001E0F00
    variable static_cal_data_10    0x001E0F00
    variable static_cal_data_11    0x001E0F00
    variable static_cal_data_12    0x001E0F00
    variable static_cal_data_13    0x001E0F00
    variable static_cal_data_14    0x001E0F00
    variable static_cal_data_15    0x001E0F00
    variable static_cal_data_16    0x001E0F00
    variable static_cal_data_17    0x001E0F00
    variable static_cal_data_18    0x001E0F00
    variable static_cal_data_19    0x001E0F00
    variable static_cal_data_20    0x001E0F00
    variable static_cal_data_21    0x001E0F00
    variable static_cal_data_22    0x001E0F00
    variable static_cal_data_23    0x001E0F00
    variable static_cal_data_24    0x001E0F00
    variable static_cal_data_25    0x001E0F00
    variable static_cal_data_26    0x001E0F00
    variable static_cal_data_27    0x001E0F00
    variable static_cal_data_28    0x001E0F00
    variable static_cal_data_29    0x001E0F00
    variable static_cal_data_30    0x001E0F00
    variable static_cal_data_31    0x001E0F00
    variable static_cal_data_32    0x00000000  

    variable zq_setting             0x0000107C

    variable tx_clk_phs_delay       19
    variable clkm_delay             13
    variable clkm90_delay           10
    variable calibration_type       software ;# software or static
    
    variable auto_calibration       disable

    variable mrs_dll_enable         dll_enable ;#dll_enable->0, dll_disable->1
    variable mrs_drv_strength       normal     ;#normal->0, reduced->1
    variable mrs_odt                150        ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3) 0, 20, 30, 40, 60, 120
    variable mrs_additive_latency   0          ;#(DDR1) none, (DDR2) 0~5, DDR3 0~2
    variable mrs_rtt_wr             120        ;#(DDR3) 0->disable, 60, 120
}

namespace eval HYNIX_H5PS5182GFRS6C {
    variable ddr_type              DDR2 ;# 1-DDR1, 2-DDR2, 3-DDR3
    ## MCR
    variable IPREF                 0
    variable DPREF                 0
    ## DCR
    variable BANKCNT               1
    variable DBUSWID               0 ;#0:8bit, 1:16bit
    variable ROWCNT                3 ;# 16K
    variable COLCNT                2
    variable DCHIPSEL              0
    variable FAST_RX               0
    variable BSTREF                0
    ## DTR input
    variable refi_ns               7800
    variable rp_ns                 15
    variable rcd_ns                15
    variable ras_ns                45
    variable rfc_ns                328
    variable wr_ns                 15
    variable rrd_ns                10
    variable fawg_ns               50
    variable wtr_ns                8
    variable rtp_ns                8

## MPMR0
    variable PM_MODE                0x0
    variable T_CKE                  0xf
    variable T_RSD                  0x3ff
    variable T_XSREF                0x3ff
## MPMR1
    variable T_XARD                 0xf
    variable T_AXPD                 0xf
## DIDER
    variable DQS0_EN_HCLK           0x1
    variable DQS0_EN_TAP            0x0
    variable DQS1_EN_HCLK           0x0
    variable DQS1_EN_TAP            0x0
## D23OSCR
    variable ODT_ALWAYS_ON          0x0
    variable TE_ALWAYS_ON           0x0
## DACCR
    variable AC_MODE                0x1
    variable DQS_SE                 0x1
    variable DQS0_GROUP_TAP         0x0
    variable DQS1_GROUP_TAP         0x0
    variable AC_DYN_BPTR_CLR_EN     0x0
    variable AC_BPTR_CLEAR          0x1
    variable AC_DEBUG_SEL           0x0
## DACSPCR
    variable AC_SILEN_PERIOD_EN     0x0
    variable AC_SILEN_TRIG          0x0
    variable AC_SILEN_PERIOD_UNIT   0x0
    variable AC_SILEN_PERIOD        0x0
    variable AC_SILEN_LEN           0x7f
## DACSPAR
    variable dacspar                0xa1a00000

    variable static_cal_data_0     0x001E0F00
    variable static_cal_data_1     0x001E0F00
    variable static_cal_data_2     0x001E0F00
    variable static_cal_data_3     0x001E0F00
    variable static_cal_data_4     0x001E0F00
    variable static_cal_data_5     0x001E0F00
    variable static_cal_data_6     0x001E0F00
    variable static_cal_data_7     0x001E0F00
    variable static_cal_data_8     0x001E0F00
    variable static_cal_data_9     0x001E0F00
    variable static_cal_data_10    0x001E0F00
    variable static_cal_data_11    0x001E0F00
    variable static_cal_data_12    0x001E0F00
    variable static_cal_data_13    0x001E0F00
    variable static_cal_data_14    0x001E0F00
    variable static_cal_data_15    0x001E0F00
    variable static_cal_data_16    0x001E0F00
    variable static_cal_data_17    0x001E0F00
    variable static_cal_data_18    0x001E0F00
    variable static_cal_data_19    0x001E0F00
    variable static_cal_data_20    0x001E0F00
    variable static_cal_data_21    0x001E0F00
    variable static_cal_data_22    0x001E0F00
    variable static_cal_data_23    0x001E0F00
    variable static_cal_data_24    0x001E0F00
    variable static_cal_data_25    0x001E0F00
    variable static_cal_data_26    0x001E0F00
    variable static_cal_data_27    0x001E0F00
    variable static_cal_data_28    0x001E0F00
    variable static_cal_data_29    0x001E0F00
    variable static_cal_data_30    0x001E0F00
    variable static_cal_data_31    0x001E0F00
    variable static_cal_data_32    0x00000000
    
    variable zq_setting             0x0000107C

    variable tx_clk_phs_delay       19
    variable clkm_delay             13
    variable clkm90_delay           10
    variable calibration_type       software ;# software or static
    
    variable auto_calibration       disable

    variable mrs_dll_enable         dll_enable ;#dll_enable->0, dll_disable->1
    variable mrs_drv_strength       normal     ;#normal->0, reduced->1
    variable mrs_odt                150        ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3) 0, 20, 30, 40, 60, 120
    variable mrs_additive_latency   0          ;#(DDR1) none, (DDR2) 0~5, DDR3 0~2
    variable mrs_rtt_wr             120        ;#(DDR3) 0->disable, 60, 120
}

namespace eval WINDBOND_W9751G8KB-25 {
    variable ddr_type              DDR2 ;# 1-DDR1, 2-DDR2, 3-DDR3
    ## MCR
    variable IPREF                 0
    variable DPREF                 0
    ## DCR
    variable BANKCNT               1
    variable DBUSWID               0 ;#0:8bit, 1:16bit
    variable ROWCNT                3 ;# 16K
    variable COLCNT                2
    variable DCHIPSEL              0
    variable FAST_RX               0
    variable BSTREF                0
    ## DTR input
    variable refi_ns               7800
    variable rp_ns                 15
    variable rcd_ns                15
    variable ras_ns                45
    variable rfc_ns                328
    variable wr_ns                 15
    variable rrd_ns                10
    variable fawg_ns               50
    variable wtr_ns                8
    variable rtp_ns                8

## MPMR0
    variable PM_MODE                0x0
    variable T_CKE                  0xf
    variable T_RSD                  0x3ff
    variable T_XSREF                0x3ff
## MPMR1
    variable T_XARD                 0xf
    variable T_AXPD                 0xf
## DIDER
    variable DQS0_EN_HCLK           0x0
    variable DQS0_EN_TAP            0x0
    variable DQS1_EN_HCLK           0x0
    variable DQS1_EN_TAP            0x0
## D23OSCR
    variable ODT_ALWAYS_ON          0x0
    variable TE_ALWAYS_ON           0x0
## DACCR
    variable AC_MODE                0x1
    variable DQS_SE                 0x1
    variable DQS0_GROUP_TAP         0x0
    variable DQS1_GROUP_TAP         0x0
    variable AC_DYN_BPTR_CLR_EN     0x0
    variable AC_BPTR_CLEAR          0x1
    variable AC_DEBUG_SEL           0x0
## DACSPCR
    variable AC_SILEN_PERIOD_EN     0x0
    variable AC_SILEN_TRIG          0x0
    variable AC_SILEN_PERIOD_UNIT   0x0
    variable AC_SILEN_PERIOD        0x0
    variable AC_SILEN_LEN           0x7f
## DACSPAR
    variable dacspar                0xa1a00000

    variable static_cal_data_0     0x001E0F00
    variable static_cal_data_1     0x001E0F00
    variable static_cal_data_2     0x001E0F00
    variable static_cal_data_3     0x001E0F00
    variable static_cal_data_4     0x001E0F00
    variable static_cal_data_5     0x001E0F00
    variable static_cal_data_6     0x001E0F00
    variable static_cal_data_7     0x001E0F00
    variable static_cal_data_8     0x001E0F00
    variable static_cal_data_9     0x001E0F00
    variable static_cal_data_10    0x001E0F00
    variable static_cal_data_11    0x001E0F00
    variable static_cal_data_12    0x001E0F00
    variable static_cal_data_13    0x001E0F00
    variable static_cal_data_14    0x001E0F00
    variable static_cal_data_15    0x001E0F00
    variable static_cal_data_16    0x001E0F00
    variable static_cal_data_17    0x001E0F00
    variable static_cal_data_18    0x001E0F00
    variable static_cal_data_19    0x001E0F00
    variable static_cal_data_20    0x001E0F00
    variable static_cal_data_21    0x001E0F00
    variable static_cal_data_22    0x001E0F00
    variable static_cal_data_23    0x001E0F00
    variable static_cal_data_24    0x001E0F00
    variable static_cal_data_25    0x001E0F00
    variable static_cal_data_26    0x001E0F00
    variable static_cal_data_27    0x001E0F00
    variable static_cal_data_28    0x001E0F00
    variable static_cal_data_29    0x001E0F00
    variable static_cal_data_30    0x001E0F00
    variable static_cal_data_31    0x001E0F00
    variable static_cal_data_32    0x00000000
    
    variable zq_setting             0x0000107C

    variable tx_clk_phs_delay       19
    variable clkm_delay             13
    variable clkm90_delay           10
    variable calibration_type       software ;# software or static
    
    variable auto_calibration       disable

    variable mrs_dll_enable         dll_enable ;#dll_enable->0, dll_disable->1
    variable mrs_drv_strength       normal     ;#normal->0, reduced->1
    variable mrs_odt                150        ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3) 0, 20, 30, 40, 60, 120
    variable mrs_additive_latency   0          ;#(DDR1) none, (DDR2) 0~5, DDR3 0~2
    variable mrs_rtt_wr             120        ;#(DDR3) 0->disable, 60, 120
}

namespace eval WINBOND_W971GG8JB25 {
## dram information
    variable ddr_type               DDR2 ;# 1-DDR1, 2-DDR2, 3-DDR3
## MCR
    variable IPREF                  0
    variable DPREF                  0
## DCR
    variable BANKCNT                2
    variable DBUSWID                0
    variable ROWCNT                 3;# 16K
    variable COLCNT                 2
    variable DCHIPSEL               0
    variable FAST_RX                0
    variable BSTREF                 0
## DTR input
    variable refi_ns                7800
    variable rp_ns                  15
    variable rcd_ns                 15
    variable ras_ns                 45
    variable rfc_ns                 328
    variable wr_ns                  15
    variable rrd_ns                 10
    variable fawg_ns                50
    variable wtr_ns                 8
    variable rtp_ns                 8

## MPMR0
    variable PM_MODE                0x0
    variable T_CKE                  0xf
    variable T_RSD                  0x3ff
    variable T_XSREF                0x3ff
## MPMR1
    variable T_XARD                 0xf
    variable T_AXPD                 0xf
## DIDER
    variable DQS0_EN_HCLK           0x0
    variable DQS0_EN_TAP            0x0
    variable DQS1_EN_HCLK           0x0
    variable DQS1_EN_TAP            0x0
## D23OSCR
    variable ODT_ALWAYS_ON          0x0
    variable TE_ALWAYS_ON           0x0
## DACCR
    variable AC_MODE                0x1
    variable DQS_SE                 0x1
    variable DQS0_GROUP_TAP         0x0
    variable DQS1_GROUP_TAP         0x0
    variable AC_DYN_BPTR_CLR_EN     0x0
    variable AC_BPTR_CLEAR          0x1
    variable AC_DEBUG_SEL           0x0
## DACSPCR
    variable AC_SILEN_PERIOD_EN     0x0
    variable AC_SILEN_TRIG          0x0
    variable AC_SILEN_PERIOD_UNIT   0x0
    variable AC_SILEN_PERIOD        0x0
    variable AC_SILEN_LEN           0x7f
## DACSPAR
    variable dacspar                0xa1a00000

    variable static_cal_data_0     0x001E0F00
    variable static_cal_data_1     0x001E0F00
    variable static_cal_data_2     0x001E0F00
    variable static_cal_data_3     0x001E0F00
    variable static_cal_data_4     0x001E0F00
    variable static_cal_data_5     0x001E0F00
    variable static_cal_data_6     0x001E0F00
    variable static_cal_data_7     0x001E0F00
    variable static_cal_data_8     0x001E0F00
    variable static_cal_data_9     0x001E0F00
    variable static_cal_data_10    0x001E0F00
    variable static_cal_data_11    0x001E0F00
    variable static_cal_data_12    0x001E0F00
    variable static_cal_data_13    0x001E0F00
    variable static_cal_data_14    0x001E0F00
    variable static_cal_data_15    0x001E0F00
    variable static_cal_data_16    0x001E0F00
    variable static_cal_data_17    0x001E0F00
    variable static_cal_data_18    0x001E0F00
    variable static_cal_data_19    0x001E0F00
    variable static_cal_data_20    0x001E0F00
    variable static_cal_data_21    0x001E0F00
    variable static_cal_data_22    0x001E0F00
    variable static_cal_data_23    0x001E0F00
    variable static_cal_data_24    0x001E0F00
    variable static_cal_data_25    0x001E0F00
    variable static_cal_data_26    0x001E0F00
    variable static_cal_data_27    0x001E0F00
    variable static_cal_data_28    0x001E0F00
    variable static_cal_data_29    0x001E0F00
    variable static_cal_data_30    0x001E0F00
    variable static_cal_data_31    0x001E0F00
    variable static_cal_data_32    0x00000000

## ODT=75, OCD=80
    variable zq_setting             0x000010b0
# 0xDEADDEAD, /* For patching HW bug, Disable ZQ calibration, call plat_dep_* function */
# 0x000010fC, /* ODT 75ohm    OCD 60ohm */
# 0x000011be, /* ODT 50ohm    OCD 50ohm */
# 0x0000107c, /* ODT 60ohm    OCD 60ohm */
# 0x00000530, /* ODT 80ohm    OCD 80ohm */
# 0x000010ef, /* ODT 75ohm    OCD 75ohm */
# 0x00002c9e, /* ODT 85.5ohm  OCD 87.27ohm */
# 0x00002828, /* ODT 92.31ohm OCD 96ohm */
# 0x000008fC, /* ODT 150ohm   OCD 60ohm */
# 0x000036fe, /* ODT 50ohm    OCD 48ohm */
# 0x00002ffc, /* ODT 50ohm    OCD 60ohm */
# 0x000023a3, /* ODT 100ohm   OCD 100ohm */
# 0x000008a1  /* ODT 150ohm   OCD 150ohm */
 

    variable tx_clk_phs_delay       0x00
    variable clkm_delay             0x00
    variable clkm90_delay           0x00
    variable calibration_type       software ;# software or static

    variable auto_calibration       disable ;# This is for window sliding

    variable mrs_dll_enable         dll_enable ;#dll_enable->0, dll_disable->1
    variable mrs_drv_strength       normal     ;#normal->0, reduced->1
    variable mrs_odt                75        ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3) 0, 20, 30, 40, 60, 120
    variable mrs_additive_latency   0          ;#(DDR1) none, (DDR2) 0~5, DDR3 0~2
    variable mrs_rtt_wr             0          ;#(DDR3) 0->disable, 60, 120
}

namespace_copy WINBOND_W971GG6JB25 WINBOND_W971GG8JB25
namespace eval WINBOND_W971GG6JB25 {
    variable BANKCNT 2
    variable DBUSWID 1
    variable ROWCNT  2
    variable COLCNT  2
## DACSPCR
    variable AC_SILEN_PERIOD_EN     0x0
    variable AC_SILEN_TRIG          0x0
    variable AC_SILEN_PERIOD_UNIT   0x0
    variable AC_SILEN_PERIOD        0x0
    variable AC_SILEN_LEN           0x3
## DACSPAR
    variable dacspar                0xa4000000
}

namespace_copy ETRON_EM68C16CWQD WINBOND_W971GG6JB25
namespace_copy NANYA_NT5TU64M16GG-AC WINBOND_W971GG6JB25

namespace eval NANYA_NT5CB64M16DP {
## dram information
    variable ddr_type               DDR3 ;# 1-DDR1, 2-DDR2, 3-DDR3
## MCR
    variable IPREF                  0
    variable DPREF                  0
## DCR
    variable BANKCNT                2
    variable DBUSWID                1
    variable ROWCNT                 2;# 8K
    variable COLCNT                 2
    variable DCHIPSEL               0
    variable FAST_RX                0
    variable BSTREF                 0
## DTR input
    variable refi_ns                7800
    variable rp_ns                  15
    variable rcd_ns                 15
    variable ras_ns                 45
    variable rfc_ns                 328
    variable wr_ns                  15
    variable rrd_ns                 10
    variable fawg_ns                50
    variable wtr_ns                 8
    variable rtp_ns                 8

## MPMR0
    variable PM_MODE                0x0
    variable T_CKE                  0xf
    variable T_RSD                  0x3ff
    variable T_XSREF                0x3ff
## MPMR1
    variable T_XARD                 0xf
    variable T_AXPD                 0xf
## DIDER
    variable DQS0_EN_HCLK           0x0
    variable DQS0_EN_TAP            0x0
    variable DQS1_EN_HCLK           0x0
    variable DQS1_EN_TAP            0x0
## D23OSCR
    variable ODT_ALWAYS_ON          0x0
    variable TE_ALWAYS_ON           0x0
## DACCR
    variable AC_MODE                0x1
    variable DQS_SE                 0x1
    variable DQS0_GROUP_TAP         0x0
    variable DQS1_GROUP_TAP         0x0
    variable AC_DYN_BPTR_CLR_EN     0x0
    variable AC_BPTR_CLEAR          0x1
    variable AC_DEBUG_SEL           0x0
## DACSPCR
    variable AC_SILEN_PERIOD_EN     0x0
    variable AC_SILEN_TRIG          0x0
    variable AC_SILEN_PERIOD_UNIT   0x0
    variable AC_SILEN_PERIOD        0x0
    variable AC_SILEN_LEN           0x7f
## DACSPAR
    variable dacspar                0xa1a00000

    variable static_cal_data_0     0x001E0F00
    variable static_cal_data_1     0x001E0F00
    variable static_cal_data_2     0x001E0F00
    variable static_cal_data_3     0x001E0F00
    variable static_cal_data_4     0x001E0F00
    variable static_cal_data_5     0x001E0F00
    variable static_cal_data_6     0x001E0F00
    variable static_cal_data_7     0x001E0F00
    variable static_cal_data_8     0x001E0F00
    variable static_cal_data_9     0x001E0F00
    variable static_cal_data_10    0x001E0F00
    variable static_cal_data_11    0x001E0F00
    variable static_cal_data_12    0x001E0F00
    variable static_cal_data_13    0x001E0F00
    variable static_cal_data_14    0x001E0F00
    variable static_cal_data_15    0x001E0F00
    variable static_cal_data_16    0x001E0F00
    variable static_cal_data_17    0x001E0F00
    variable static_cal_data_18    0x001E0F00
    variable static_cal_data_19    0x001E0F00
    variable static_cal_data_20    0x001E0F00
    variable static_cal_data_21    0x001E0F00
    variable static_cal_data_22    0x001E0F00
    variable static_cal_data_23    0x001E0F00
    variable static_cal_data_24    0x001E0F00
    variable static_cal_data_25    0x001E0F00
    variable static_cal_data_26    0x001E0F00
    variable static_cal_data_27    0x001E0F00
    variable static_cal_data_28    0x001E0F00
    variable static_cal_data_29    0x001E0F00
    variable static_cal_data_30    0x001E0F00
    variable static_cal_data_31    0x001E0F00
    variable static_cal_data_32    0x00000000

    variable zq_setting             0x000011be

    variable tx_clk_phs_delay       0x0
    variable clkm_delay             0x0
    variable clkm90_delay           0x0
    variable calibration_type       software ;# software or static

    variable auto_calibration       disable

    variable mrs_dll_enable         dll_enable ;#dll_enable->0, dll_disable->1
    variable mrs_drv_strength       normal     ;#normal->0, reduced->1
    variable mrs_odt                120        ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3) 0, 20, 30, 40, 60, 120
    variable mrs_additive_latency   0          ;#(DDR1) none, (DDR2) 0~5, DDR3 0~2
    variable mrs_rtt_wr             0          ;#(DDR3) 0->disable, 60, 120
}

namespace eval WINBOND_W9751G6KB25 {
## dram information
    variable ddr_type               DDR2 ;# 1-DDR1, 2-DDR2, 3-DDR3
## MCR
    variable IPREF                  0
    variable DPREF                  0
## DCR
    variable BANKCNT                1;# BA0-BA1----------0x0-0x11 = 4 banks
    variable DBUSWID                1;# 0:8bit 1:16bit
    variable ROWCNT                 2;# A0-A12-----------0x0-0x1fff = 8K 1:4K 2:8K 3:16K 4:32K 5:64K
    variable COLCNT                 2;# A0-A9------------0x0-0x3ff = 1K
    variable DCHIPSEL               0
    variable FAST_RX                0
    variable BSTREF                 0
## DTR input
    variable refi_ns                7800
    variable rp_ns                  15
    variable rcd_ns                 15
    variable ras_ns                 45
    variable rfc_ns                 328
    variable wr_ns                  15
    variable rrd_ns                 10
    variable fawg_ns                50
    variable wtr_ns                 8
    variable rtp_ns                 8

## MPMR0
    variable PM_MODE                0x0
    variable T_CKE                  0xf
    variable T_RSD                  0x3ff
    variable T_XSREF                0x3ff
## MPMR1
    variable T_XARD                 0xf
    variable T_AXPD                 0xf
## DIDER
    variable DQS0_EN_HCLK           0x0
    variable DQS0_EN_TAP            0x0
    variable DQS1_EN_HCLK           0x0
    variable DQS1_EN_TAP            0x0
## D23OSCR
    variable ODT_ALWAYS_ON          0x0
    variable TE_ALWAYS_ON           0x0
## DACCR
    variable AC_MODE                0x1
    variable DQS_SE                 0x1
    variable DQS0_GROUP_TAP         0x0
    variable DQS1_GROUP_TAP         0x0
    variable AC_DYN_BPTR_CLR_EN     0x0
    variable AC_BPTR_CLEAR          0x1
    variable AC_DEBUG_SEL           0x0
## DACSPCR
    variable AC_SILEN_PERIOD_EN     0x0
    variable AC_SILEN_TRIG          0x0
    variable AC_SILEN_PERIOD_UNIT   0x0
    variable AC_SILEN_PERIOD        0x0
    variable AC_SILEN_LEN           0x7f
## DACSPAR
    variable dacspar                0xa1a00000

    variable static_cal_data_0     0x001E0F00
    variable static_cal_data_1     0x001E0F00
    variable static_cal_data_2     0x001E0F00
    variable static_cal_data_3     0x001E0F00
    variable static_cal_data_4     0x001E0F00
    variable static_cal_data_5     0x001E0F00
    variable static_cal_data_6     0x001E0F00
    variable static_cal_data_7     0x001E0F00
    variable static_cal_data_8     0x001E0F00
    variable static_cal_data_9     0x001E0F00
    variable static_cal_data_10    0x001E0F00
    variable static_cal_data_11    0x001E0F00
    variable static_cal_data_12    0x001E0F00
    variable static_cal_data_13    0x001E0F00
    variable static_cal_data_14    0x001E0F00
    variable static_cal_data_15    0x001E0F00
    variable static_cal_data_16    0x001E0F00
    variable static_cal_data_17    0x001E0F00
    variable static_cal_data_18    0x001E0F00
    variable static_cal_data_19    0x001E0F00
    variable static_cal_data_20    0x001E0F00
    variable static_cal_data_21    0x001E0F00
    variable static_cal_data_22    0x001E0F00
    variable static_cal_data_23    0x001E0F00
    variable static_cal_data_24    0x001E0F00
    variable static_cal_data_25    0x001E0F00
    variable static_cal_data_26    0x001E0F00
    variable static_cal_data_27    0x001E0F00
    variable static_cal_data_28    0x001E0F00
    variable static_cal_data_29    0x001E0F00
    variable static_cal_data_30    0x001E0F00
    variable static_cal_data_31    0x001E0F00
    variable static_cal_data_32    0x00000000

    variable zq_setting             0x000011be
# 0xDEADDEAD, /* For patching HW bug, Disable ZQ calibration, call plat_dep_* function */
# 0x000010fC, /* ODT 75ohm    OCD 60ohm */
# 0x000011be, /* ODT 50ohm    OCD 50ohm */
# 0x0000107c, /* ODT 60ohm    OCD 60ohm */
# 0x00000530, /* ODT 80ohm    OCD 80ohm */
# 0x000010ef, /* ODT 75ohm    OCD 75ohm */
# 0x00002c9e, /* ODT 85.5ohm  OCD 87.27ohm */
# 0x00002828, /* ODT 92.31ohm OCD 96ohm */
# 0x000008fC, /* ODT 150ohm   OCD 60ohm */
# 0x000036fe, /* ODT 50ohm    OCD 48ohm */
# 0x00002ffc, /* ODT 50ohm    OCD 60ohm */
# 0x000023a3, /* ODT 100ohm   OCD 100ohm */
# 0x000008a1  /* ODT 150ohm   OCD 150ohm */
 

    variable tx_clk_phs_delay       0x00
    variable clkm_delay             0x00
    variable clkm90_delay           0x00
    variable calibration_type       software ;# software or static

    variable auto_calibration       disable ;# This is for window sliding

    variable mrs_dll_enable         dll_enable ;#dll_enable->0, dll_disable->1
    variable mrs_drv_strength       normal     ;#normal->0, reduced->1
    variable mrs_odt                75        ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3) 0, 20, 30, 40, 60, 120
    variable mrs_additive_latency   0          ;#(DDR1) none, (DDR2) 0~5, DDR3 0~2
    variable mrs_rtt_wr             0          ;#(DDR3) 0->disable, 60, 120
}

### to illustrate, will be removed soon
#namespace_copy WINBOND_test WINBOND_W971GG8JB25 
#variable WINBOND_test::mrs_odr 150
#namespace_dump WINBOND_test

namespace_copy MICRON_MT41J64M16 DRAM_GENERIC
namespace eval MICRON_MT41J64M16 {
    variable ddr_type               DDR3 ;# 1-DDR1, 2-DDR2, 3-DDR3
    variable BANKCNT 2
    variable DBUSWID 1
    variable ROWCNT  2
    variable COLCNT  2
}

namespace_copy WINBOND_W631GG6KB DRAM_GENERIC
namespace eval WINBOND_W631GG6KB {
    variable ddr_type               DDR3 ;# 1-DDR1, 2-DDR2, 3-DDR3
    variable BANKCNT 2
    variable DBUSWID 1
    variable ROWCNT  2
    variable COLCNT  2
## DIDER
    variable DQS0_EN_HCLK           0x1
    variable DQS0_EN_TAP            0x0
    variable DQS1_EN_HCLK           0x1
    variable DQS1_EN_TAP            0x0
## ODT=75, OCD=80
    variable zq_setting             0x000010b0
    variable tx_clk_phs_delay       0x10
    variable clkm_delay             0x04
    variable clkm90_delay           0x00
    variable mrs_odt                60        ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3-ntt_nom) 0, 20, 30, 40, 60, 120
    variable mrs_rtt_wr             60          ;#(DDR3) 0->disable, 60, 120
}

namespace_copy MICRON_MT41K128M16JT WINBOND_W631GG6KB
namespace eval MICRON_MT41K128M16JT {
    variable BANKCNT 2
    variable DBUSWID 1
    variable ROWCNT  3
    variable COLCNT  2
    variable tx_clk_phs_delay       0x00
    variable clkm_delay             0x00
    variable clkm90_delay           0x00
}

namespace_copy NANYA_NT5TU64M8DE-AC NT5TU128M8GE-AC
namespace eval NANYA_NT5TU64M8DE-AC {
    variable BANKCNT 1
    variable DBUSWID 0
    variable ROWCNT  3
    variable COLCNT  2
}

namespace_copy SKHYNIX_H5PS5162GFR WINBOND_W9751G6KB25
