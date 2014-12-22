############################# Note #############################
# 1. pm_method:		  	                               #
#        RWSR:		  MXIC, enter_4Byte_cmd                #
#        RWSR_4BCMDSET:   MXIC, 4Byte_cmd_set                  #
#        CMD:             EON, enter_4Byte_cmd                 #
#        CMD_4BCMDSET:    EON, 4Byte_cmd_set                   #
#        R2W1SR:          Winbond, enter_4Byte_cmd             #
#        R2W1SR_4BCMDSET: Winbond, 4Byte_cmd_set               #
#        EN0:             Micron, enter_4Byte_cmd              #
#        EN0_4BCMDSET:    Micron, 4Byte_cmd_set                #
############################# END  #############################

namespace eval SPI_NOR_GENERIC {
    variable num_chips          1
    variable addr_mode		3
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      16M
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0x03    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  0
    variable prefer_rd_addr_io  SIO
    variable prefer_rd_data_io  SIO

    variable wr_cmd             0x02    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         SIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0x20    ;#prefered erase command to be used in PIO.
    variable erase_unit         4K      ;#bytes per erase can do.

    variable pm_method          NONE  ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x00    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x00    ;#N/A
    variable pm_wrsr_cmd        0x00    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x00    ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      0       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  0   ;#idle time between two rdbusy_cmd in usec.

    variable id                 0xFFFFFF
}

namespace eval MXIC_MX25L12845E {
    variable num_chips          1
    variable addr_mode		3
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      16M
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0xeb    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  6
    variable prefer_rd_addr_io  QIO
    variable prefer_rd_data_io  QIO

    variable wr_cmd             0x02    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         SIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0xD8    ;#prefered erase command to be used in PIO.
    variable erase_unit         64K      ;#bytes per erase can do.

    variable pm_method          RWSR  ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x05    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x00    ;#N/A
    variable pm_wrsr_cmd        0x01    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x40    ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      1       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  1   ;#idle time between two rdbusy_cmd in usec.

    variable id                 0xc22018
}

namespace eval MXIC_MX25L12845E_SIO {
    variable num_chips          1
    variable addr_mode		    3
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      16M
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0x03    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  0
    variable prefer_rd_addr_io  SIO
    variable prefer_rd_data_io  SIO

    variable wr_cmd             0x02    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         SIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0xD8    ;#prefered erase command to be used in PIO.
    variable erase_unit         64K      ;#bytes per erase can do.

    variable pm_method          NONE  ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x05    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x00    ;#N/A
    variable pm_wrsr_cmd        0x01    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x40    ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      1       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  1   ;#idle time between two rdbusy_cmd in usec.

    variable id                 0xc22018
}

namespace eval MXIC_MX25L12835F {
    variable num_chips          1
    variable addr_mode		    3
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      16M
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0xeb    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  6
    variable prefer_rd_addr_io  QIO
    variable prefer_rd_data_io  QIO

    variable wr_cmd             0x02    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         SIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0xD8    ;#prefered erase command to be used in PIO.
    variable erase_unit         64K      ;#bytes per erase can do.

    variable pm_method          RWSR  ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x05    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x00    ;#N/A
    variable pm_wrsr_cmd        0x01    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x40    ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      1       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  1   ;#idle time between two rdbusy_cmd in usec.

    variable id                 0xc22018
}

namespace eval MXIC_MX25L3235D {
    variable num_chips          1
    variable addr_mode		    3
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      4M
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0xeb    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  6
    variable prefer_rd_addr_io  QIO
    variable prefer_rd_data_io  QIO

    variable wr_cmd             0x02    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         SIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0x20    ;#prefered erase command to be used in PIO.
    variable erase_unit         4K      ;#bytes per erase can do.

    variable pm_method          RWSR  ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x05    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x00    ;#N/A
    variable pm_wrsr_cmd        0x01    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x40    ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      1       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  1   ;#idle time between two rdbusy_cmd in usec.

    variable id                 0xc25e16
}

namespace eval EON_EN25Q64 {
    variable num_chips          1
    variable addr_mode		    3
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      8M
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0x03    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  0
    variable prefer_rd_addr_io  SIO
    variable prefer_rd_data_io  SIO

    variable wr_cmd             0x02    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         SIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0xD8    ;#prefered erase command to be used in PIO.
    variable erase_unit         64K      ;#bytes per erase can do.

    variable pm_method          NONE    ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x00    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x00    ;#N/A
    variable pm_wrsr_cmd        0x00    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x00    ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      0       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  1   ;#idle time between two rdbusy_cmd in usec.

    variable id                 0x1c3017
}

namespace eval WINBOND_W25Q32BV {
    variable num_chips          1
    variable addr_mode		    3
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      4M
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0xeb    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  6
    variable prefer_rd_addr_io  QIO
    variable prefer_rd_data_io  QIO

    variable wr_cmd             0x32    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         QIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0xD8    ;#prefered erase command to be used in PIO.
    variable erase_unit         64K      ;#bytes per erase can do.

    variable pm_method          R2W1SR  ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x05    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x35    ;#N/A
    variable pm_wrsr_cmd        0x01    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x0002  ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      2       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  1   ;#idle time between two rdbusy_cmd in usec.

    variable id                 0xef4016
}

namespace eval WINBOND_W25Q32BV_SIO {
    variable num_chips          1
    variable addr_mode		3
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      4M
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0x03    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  0
    variable prefer_rd_addr_io  SIO
    variable prefer_rd_data_io  SIO

    variable wr_cmd             0x02    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         SIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0x20    ;#prefered erase command to be used in PIO.
    variable erase_unit         4K      ;#bytes per erase can do.

    variable pm_method          NONE  ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
#    variable pm_rdsr_cmd        0x05    ;#command to read status register for "RWSR".
#    variable pm_rdsr2_cmd       0x00    ;#N/A
#    variable pm_wrsr_cmd        0x01    ;#command to write status register for "RWSR".
#    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
#    variable pm_enable_bits     0x40  ;#bit mask to enable high performance mode for "RWSR".
#    variable pm_status_len      1       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  1   ;#idle time between two rdbusy_cmd in usec.

    variable id                 0xef4016
}


namespace eval WINBOND_W25Q64FV {
    variable num_chips          1
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      8M
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0xeb    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  6
    variable prefer_rd_addr_io  QIO
    variable prefer_rd_data_io  QIO

    variable wr_cmd             0x32    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         QIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0x20    ;#prefered erase command to be used in PIO.
    variable erase_unit         4K      ;#bytes per erase can do.

    variable pm_method          R2W1SR  ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x05    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x35    ;#N/A
    variable pm_wrsr_cmd        0x01    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x0002  ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      2       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  1   ;#idle time between two rdbusy_cmd in usec.

    variable id                 0xef4017
}

namespace eval MXIC_MX25L25635F {
    variable num_chips          1
    variable addr_mode			4
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      32M
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0xec    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  6
    variable prefer_rd_addr_io  QIO
    variable prefer_rd_data_io  QIO

    variable wr_cmd             0x12    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         SIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0x21    ;#prefered erase command to be used in PIO.
    variable erase_unit         4K      ;#bytes per erase can do.

    variable pm_method          RWSR_4BCMDSET  ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x05    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x00    ;#N/A
    variable pm_wrsr_cmd        0x01    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x40    ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      1       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  0   ;#idle time between two rdbusy_cmd in usec.

    variable id                 0xc22019
}

namespace eval MXIC_MX25L25635F_SIO {
    variable num_chips          1
    variable addr_mode			4
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      32M
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0x0C    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  8
    variable prefer_rd_addr_io  SIO
    variable prefer_rd_data_io  SIO

    variable wr_cmd             0x12    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         SIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0x21    ;#prefered erase command to be used in PIO.
    variable erase_unit         4K      ;#bytes per erase can do.

    variable pm_method          NONE  ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x00    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x00    ;#N/A
    variable pm_wrsr_cmd        0x00    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x00    ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      0       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  0   ;#idle time between two rdbusy_cmd in usec.

    variable id                 0xc22019
}

namespace eval MXIC_MX25L25735E {
    variable num_chips          1
    variable addr_mode		    4
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      32M
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0xeb    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  6
    variable prefer_rd_addr_io  QIO
    variable prefer_rd_data_io  QIO

    variable wr_cmd             0x02    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         SIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0x20    ;#prefered erase command to be used in PIO.
    variable erase_unit         4K      ;#bytes per erase can do.

    variable pm_method          NONE  ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x05    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x00    ;#N/A
    variable pm_wrsr_cmd        0x01    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x40    ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      1       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  0   ;#idle time between two rdbusy_cmd in usec.

    variable id                 0xc22019
}
namespace eval MXIC_MX25L25735E_SIO {
    variable num_chips          1
    variable addr_mode		    4
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      32M
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0x03    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  0
    variable prefer_rd_addr_io  SIO
    variable prefer_rd_data_io  SIO

    variable wr_cmd             0x02    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         SIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0xD8    ;#prefered erase command to be used in PIO.
    variable erase_unit         64K      ;#bytes per erase can do.

    variable pm_method          NONE  ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x05    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x00    ;#N/A
    variable pm_wrsr_cmd        0x01    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x40    ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      1       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  0   ;#idle time between two rdbusy_cmd in usec.

    variable id                 0xc22019
}


namespace_copy WINBOND_W25Q128FV WINBOND_W25Q32BV
namespace eval WINBOND_W25Q128FV {
    variable addr_mode		    3
    variable size_per_chip      16M
    variable id                 0xef4018
}

namespace_copy WINBOND_W25Q128FV_SIO WINBOND_W25Q32BV_SIO
namespace eval WINBOND_W25Q128FV_SIO {
    variable addr_mode		3
    variable size_per_chip      16M
    variable id                 0xef4018
}

namespace_copy WINBOND_W25Q256FV SPI_NOR_GENERIC
namespace eval WINBOND_W25Q256FV {
    variable num_chips          1
    variable addr_mode          4
    variable size_per_chip      32M
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0xeb    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  6
    variable prefer_rd_addr_io  QIO
    variable prefer_rd_data_io  QIO

    variable wr_cmd             0x02    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         SIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0x20    ;#prefered erase command to be used in PIO.
    variable erase_unit         4K      ;#bytes per erase can do.

    variable pm_method          R2W1SR  ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x05    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x35    ;#N/A
    variable pm_wrsr_cmd        0x01    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x02    ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      2       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  1   ;#idle time between two rdbusy_cmd in usec.
	
    variable id                 0xEF4019
}

namespace_copy GD_GD25Q128BFIG SPI_NOR_GENERIC
namespace eval GD_GD25Q128BFIG {
    variable num_chips          1
    variable addr_mode	        3
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    # This chip requires EN4B cmd to enter 4B mode for 32MB size. Otherwise, its 16MB.
    variable size_per_chip      16M
    variable prefer_rx_delay0   0
    variable prefer_rx_delay1   0
    variable prefer_rx_delay2   0
    variable prefer_rx_delay3   0

    variable prefer_rd_cmd      0xeb    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  6
    variable prefer_rd_addr_io  QIO
    variable prefer_rd_data_io  QIO

    variable wr_cmd             0x02    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         SIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0xD8    ;#prefered erase command to be used in PIO.
    variable erase_unit         64K      ;#bytes per erase can do.

    variable pm_method          R2W1SR    ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x05    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x35    ;#N/A
    variable pm_wrsr_cmd        0x01    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x02    ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      2       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  1   ;#idle time between two rdbusy_cmd in usec.
	
    variable id                 0xC84018
}

namespace_copy GD_GD25Q128BFIG_SIO GD_GD25Q128BFIG
namespace eval GD_GD25Q128BFIG_SIO {
    variable prefer_rd_cmd      0x03    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  0
    variable prefer_rd_addr_io  SIO
    variable prefer_rd_data_io  SIO

    variable pm_method          NONE    ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
}

namespace_copy MICRON_N25Q256A13ESF40G SPI_NOR_GENERIC
namespace eval MICRON_N25Q256A13ESF40G {
    variable num_chips          1
    variable addr_mode			4
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      32M	

    variable prefer_rd_cmd      0xeb    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  10
    variable prefer_rd_addr_io  QIO
    variable prefer_rd_data_io  QIO

    variable wr_cmd             0x02    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         SIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0x20    ;#prefered erase command to be used in PIO.
    variable erase_unit         4K      ;#bytes per erase can do.

    variable pm_method          EN0		;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x05    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x00    ;#N/A
    variable pm_wrsr_cmd        0x01    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x40    ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      1       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  1   ;#idle time between two rdbusy_cmd in usec.	
    
    variable id                 0x20BA19
}

namespace_copy SPANSION_S25FL256S SPI_NOR_GENERIC
namespace eval SPANSION_S25FL256S {
    variable num_chips          1
    variable addr_mode          4
    variable prefer_divisor     16      ;#freq. divisor of SPIF ctrl.
    variable size_per_chip      32M

    variable prefer_rd_cmd      0xEC    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  6
    variable prefer_rd_addr_io  QIO
    variable prefer_rd_data_io  QIO

    variable wr_cmd             0x34    ;#prefered write(program) command to be used in PIO.
    variable wr_cmd_io          SIO
    variable wr_dummy_c         0
    variable wr_addr_io         SIO
    variable wr_data_io         QIO
    variable wr_boundary        256     ;#bytes per write can do.

    variable erase_cmd          0xDC    ;#prefered erase command to be used in PIO.
    variable erase_unit         64K      ;#bytes per erase can do.

    variable pm_method          R2W1SR_4BCMDSET  ;#how to enable high performance mode, "NONE", "RWSR"(MXIC), or "CMD"(EON).
    variable pm_rdsr_cmd        0x05    ;#command to read status register for "RWSR".
    variable pm_rdsr2_cmd       0x35    ;#N/A
    variable pm_wrsr_cmd        0x01    ;#command to write status register for "RWSR".
    variable pm_enable_cmd      0x00    ;#command to enable high performance mode for "CMD".
    variable pm_enable_bits     0x0002  ;#bit mask to enable high performance mode for "RWSR".
    variable pm_status_len      2       ;#byte length that rdsr_cmd retures for "RWSR".

    variable rdbusy_cmd         0x05    ;#command to query busy status.
    variable rdbusy_len         0x2     ;#byte length that rdbusy_cmd returns.
    variable rdbusy_loc         0       ;#busy bit position in the returned byte length.
    variable rdbusy_polling_period  1   ;#idle time between two rdbusy_cmd in usec.

    variable id                 0x010219
}
