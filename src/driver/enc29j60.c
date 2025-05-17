#include "driver/enc28j60.h"

// Dummy variable
static uint8_t dummy = 0;

/* Basic IO */

SPI_Status_t ENC_ReadReg(uint8_t address, uint8_t *value) {
    SPI_START_OP();
    uint8_t tx_buffer = (ENC_OP_RCR << 5) | (uint8_t)address;
    SPI_Transfer(tx_buffer, value);
    SPI_Transfer(0x00, value);
    SPI_STOP_OP();
    return SPI_OK;
}

SPI_Status_t ENC_WriteReg(uint8_t address, uint8_t value) {
    SPI_START_OP();
    uint8_t tx_buffer[2];
    tx_buffer[0] = (ENC_OP_WCR << 5) | (uint8_t)address;
    tx_buffer[1] = value;
    SPI_Transfer(tx_buffer[0], &dummy);
    SPI_Transfer(tx_buffer[1], &dummy);
    SPI_STOP_OP();
    return SPI_OK;
}

SPI_Status_t ENC_BitSet(uint8_t address, uint8_t mask) {
    SPI_START_OP();
    SPI_Transfer((ENC_OP_BFS << 5) | (uint8_t)address, &dummy);
    SPI_Transfer(mask, &dummy);
    SPI_STOP_OP();
    return SPI_OK;
}

SPI_Status_t ENC_BitClear(uint8_t address, uint8_t mask) {
    SPI_START_OP();
    SPI_Transfer((ENC_OP_BFC << 5) | (uint8_t)address, &dummy);
    SPI_Transfer(mask, &dummy);
    SPI_STOP_OP();
    return SPI_OK;
}

SPI_Status_t ENC_SoftReset(void) {
    SPI_START_OP();
    SPI_Transfer(ENC_OP_SRC, &dummy);
    SPI_STOP_OP();
    return SPI_OK;
}

SPI_Status_t ENC_WritePhyReg(uint8_t address, uint16_t value) {
    SPI_START_OP();
    ENC_SEL_BANK2();
    ENC_WriteReg(MIREGADR, address);
    ENC_WriteReg(MIWRL, value & 0xFF);
    ENC_WriteReg(MIWRH, value >> 8);

    ENC_SEL_BANK3();
    uint8_t rx_buffer = 0;
    ENC_ReadMacMIIReg(MISTAT, &rx_buffer);
    while (rx_buffer & MISTAT_BUSY) {
        ENC_ReadMacMIIReg(MISTAT, &rx_buffer);
    }
    SPI_STOP_OP();
    return SPI_OK;
}

SPI_Status_t ENC_ReadMacMIIReg(uint8_t address, uint8_t *value) {
    SPI_START_OP();
    uint8_t tx_buffer = (ENC_OP_RCR << 5) | (uint8_t)address;
    SPI_Transfer(tx_buffer, value);
    SPI_Transfer(0x00, &dummy);
    SPI_Transfer(0x00, value);
    SPI_STOP_OP();
    return SPI_OK;
}

SPI_Status_t ENC_WriteBufferMemory(uint16_t address, uint8_t *data, uint32_t size) {
    SPI_START_OP();
    /* Set memory address to write */
    ENC_SEL_BANK0();
    ENC_WriteReg(EWRPTL, address & 0xFF);
    ENC_WriteReg(EWRPTH, address >> 8);
    SPI_START_OP();
    SPI_Transfer((ENC_OP_WBM << 5) | ENC_OP_BM_SUFF, &dummy);
    for (uint8_t *value = data; value - data < size; value++) {
        SPI_Transfer(*value, &dummy);
    }
    //SPI_WAIT_ON_BSY();
    SPI_STOP_OP();
    return SPI_OK;
}

SPI_Status_t ENC_ReadBufferMemory(uint16_t address, uint8_t *data, uint32_t size) {
    SPI_START_OP();
    /* Set memory address to read */
    ENC_WriteReg(ERDPTL, address & 0xFF);
    ENC_WriteReg(ERDPTH, address >> 8);
    SPI_START_OP();
    SPI_Transfer((ENC_OP_RBM << 5) | ENC_OP_BM_SUFF, &dummy);
    for (uint8_t *value = data; value - data < size; value++) {
        SPI_Transfer(0x00, value);
    }
    //SPI_WAIT_ON_BSY();
    SPI_STOP_OP();
    return SPI_OK;
}

/* Control functions */

SPI_Status_t ENC_PendClkrdy(void) {
    uint8_t rx_buffer;
    ENC_ReadReg(ESTAT, &rx_buffer);
    while (!(rx_buffer & ESTAT_CLKRDY_Msk)) {
        delay(1);
        ENC_ReadReg(ESTAT, &rx_buffer);
    }
    return SPI_OK;
}

SPI_Status_t ENC_Init(void) {
    ENC_PendClkrdy(); // Wait for the oscillator to stabilize 
    ENC_SoftReset(); // Perform a software reset

    ENC_SEL_BANK0();
    /* Program RX buffer start and stop pointers */
    ENC_WriteReg(ERXSTL, ETH_RX_BUFFER_START & 0xFF);
    ENC_WriteReg(ERXSTH, ETH_RX_BUFFER_START >> 8);
    ENC_WriteReg(ERXNDL, (ETH_RX_BUFFER_START + ETH_RX_BUFFER_SIZE) & 0xFF);
    ENC_WriteReg(ERXNDH, (ETH_RX_BUFFER_START + ETH_RX_BUFFER_SIZE) >> 8); // 6KByte buffer

    ENC_WriteReg(ETXSTL, ETH_TX_BUFFER_START & 0xFF);
    ENC_WriteReg(ETXSTH, ETH_TX_BUFFER_START >> 8);
    ENC_WriteReg(ETXNDL, (ETH_TX_BUFFER_START + ETH_TX_BUFFER_SIZE) & 0xFF);
    ENC_WriteReg(ETXNDL, (ETH_TX_BUFFER_START + ETH_TX_BUFFER_SIZE) >> 8); // 2KByte buffer

    ENC_BitSet(ECON1, ECON1_RXEN);
    ENC_BitSet(ECON2, ECON2_AUTOINC);

    ENC_SEL_BANK2();
    /* Enable CRC, broadcast and unicast filters */
    ENC_BitSet(ERXFCON, ERXFCON_BCEN | ERXFCON_CRCEN | ERXFCON_UCEN);

    ENC_SEL_BANK2();
    // Full-duplex or not?
#if ETH_FULL_DPX==1
    ENC_BitSet(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
    ENC_BitSet(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX);
    ENC_WriteReg(MABBIPG, 0x15);
#else
    ENC_BitSet(MACON1, MACON1_MARXEN);
    ENC_BitSet(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
    ENC_WriteReg(MABBIPG, 0x12);
    ENC_WriteReg(MAIPGH, 0x0C);
#endif
    ENC_BitSet(MACON4, MACON4_DEFER);
    ENC_WriteReg(MAMXFLL, ETH_MAX_FRAME_LEN & 0xFF);
    ENC_WriteReg(MAMXFLH, ETH_MAX_FRAME_LEN >> 8);
    ENC_WriteReg(MAIPGL, 0x15);

    ENC_SEL_BANK3();
    ENC_WriteReg(MAADR1, ETH_MAC1);
    ENC_WriteReg(MAADR2, ETH_MAC2);
    ENC_WriteReg(MAADR3, ETH_MAC3);
    ENC_WriteReg(MAADR4, ETH_MAC4);
    ENC_WriteReg(MAADR5, ETH_MAC5);
    ENC_WriteReg(MAADR6, ETH_MAC6);

#if ETH_FULL_DPX==1
    ENC_WritePhyReg(PHCON1, PHCON1_PDPXMD);
#else
    ENC_WritePhyReg(PHCON2, PHCON2_HDLDIS); // Disable loopback
#endif
    return SPI_OK;
}

SPI_Status_t ENC_SendPacket(uint8_t *dst_addr, uint8_t *type_len, uint8_t *data, uint32_t size) {
    ENC_WriteBufferMemory(ETH_TX_BUFFER_START, 0x00, 1); // Write per packet config byte

    uint8_t header[ETH_HEAD_SIZE];
    uint8_t my_mac[6] = {ETH_MAC1, ETH_MAC2, ETH_MAC3, ETH_MAC4, ETH_MAC5, ETH_MAC6};
    memcpy(header, dst_addr, ETH_MAC_LEN);
    memcpy(header + ETH_MAC_LEN, my_mac, ETH_MAC_LEN);
    memcpy(header + 2*ETH_MAC_LEN, type_len, 2);

    ENC_WriteBufferMemory(ETH_TX_BUFFER_START + 1, header, ETH_HEAD_SIZE);
    ENC_WriteBufferMemory(ETH_TX_BUFFER_START + ETH_HEAD_SIZE + 1, data, size);

    uint16_t ptr = ETH_TX_BUFFER_START + ETH_HEAD_SIZE + size;
    ENC_WriteReg(ETXNDL, ptr & 0xFF);
    ENC_WriteReg(ETXNDH, ptr >> 8);
    ENC_BitSet(ECON1, ECON1_TXRTS);
    return SPI_OK;
}