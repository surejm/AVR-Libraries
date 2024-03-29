/**
 ******************************************************************************
 * @file	nrf24l01_register_map.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-04-27
 * @brief	
 ******************************************************************************
 */

/* Register Map --------------------------------------------------------------*/
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17

#define IS_VALID_REGISTER(REGISTER)		((REGISTER) >= CONFIG && (REGISTER) <= FIFO_STATUS)

/* Bit Names -----------------------------------------------------------------*/
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define PLL_LOCK    4
#define RF_DR       3
#define RF_PWR      1
#define LNA_HCURR   0        
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0

/* Pipes ---------------------------------------------------------------------*/
#define PIPE_0		0x01
#define PIPE_1		0x02
#define PIPE_2		0x04
#define PIPE_3		0x08
#define PIPE_4		0x10
#define PIPE_5		0x20
#define ALL_PIPES	(PIPE_0 | PIPE_1 | PIPE_2 | PIPE_3 | PIPE_4 | PIPE_5)

#define NO_DATA_IN_PIPE	0x07

#define DEFAULT_TX		0xE7E7E7E7E7
#define DEFAULT_RX0		0xE7E7E7E7E7
#define DEFAULT_RX1		0xC2C2C2C2C2
#define DEFAULT_RX2		0xC2C2C2C2C3
#define DEFAULT_RX3		0xC2C2C2C2C4
#define DEFAULT_RX4		0xC2C2C2C2C5
#define DEFAULT_RX5		0xC2C2C2C2C6

/* Commands ------------------------------------------------------------------*/
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF

#define IS_VALID_COMMAND(COMMAND)	((COMMAND) == R_REGISTER || (COMMAND) == W_REGISTER || \
									(COMMAND) == REGISTER_MASK || (COMMAND) == R_RX_PAYLOAD || \
									(COMMAND) == W_TX_PAYLOAD || (COMMAND) == FLUSH_TX || \
									(COMMAND) == FLUSH_RX || (COMMAND) == REUSE_TX_PL || \
									(COMMAND) == NOP)
