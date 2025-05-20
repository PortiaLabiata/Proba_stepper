#include "driver/enc28j60.h"

// Dummy variable
static uint8_t dummy = 0;
static uint32_t _next_packet_ptr = ETH_RX_BUFFER_START;

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
    //SPI_START_OP();
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
    /* Set memory address to read */
    ENC_SEL_BANK0();
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
    GPIOB->BSRR |= GPIO_BSRR_BR3;
    delay(1);
    GPIOB->BSRR |= GPIO_BSRR_BS3;
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
    /* 
    ENC_WriteReg(ETXNDL, (ETH_TX_BUFFER_START + ETH_TX_BUFFER_SIZE) & 0xFF);
    ENC_WriteReg(ETXNDL, (ETH_TX_BUFFER_START + ETH_TX_BUFFER_SIZE) >> 8); // 2KByte buffer
    */
    ENC_BitSet(ECON2, ECON2_AUTOINC);
    ENC_WriteReg(EIE, EIE_INTIE | EIE_PKTIE);

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
    ENC_BitSet(MACON3, MACON3_PADCFG1 | MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
    ENC_ReadMacMIIReg(MACON3, &dummy);
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

    ENC_BitSet(ECON1, ECON1_RXEN); // RX enable
    return SPI_OK;
}

SPI_Status_t ENC_SendPacket(uint8_t *dst_addr, uint8_t *type_len, uint8_t *data, uint32_t size) {
    ENC_BitSet(ECON1, ECON1_TXRTS);
    ENC_BitClear(ECON1, ECON1_TXRTS);

    uint8_t tx_buffer[ETH_HEAD_SIZE + size + 1];
    uint8_t my_mac[ETH_MAC_LEN] = {ETH_MAC1, ETH_MAC2, ETH_MAC3, ETH_MAC4, ETH_MAC5, ETH_MAC6};
    tx_buffer[0] = 0;
    memcpy(tx_buffer + 1, dst_addr, ETH_MAC_LEN);
    memcpy(tx_buffer + ETH_MAC_LEN + 1, my_mac, ETH_MAC_LEN);
    memcpy(tx_buffer + 2*ETH_MAC_LEN + 1, type_len, 2);
    memcpy(tx_buffer + 2*ETH_MAC_LEN + 3, data, size);

    //ENC_WriteBufferMemory(ETH_TX_BUFFER_START + 1, header, ETH_HEAD_SIZE);
    ENC_WriteBufferMemory(ETH_TX_BUFFER_START, tx_buffer, ETH_HEAD_SIZE + size + 1);

    uint16_t ptr = ETH_TX_BUFFER_START + ETH_HEAD_SIZE + size;
    ENC_WriteReg(ETXNDL, ptr & 0xFF);
    ENC_WriteReg(ETXNDH, ptr >> 8);
    ENC_BitSet(ECON1, ECON1_TXRTS);

    uint8_t status = 0;
    ENC_ReadReg(ECON1, &status);
    while (status & ECON1_TXRTS) {
        ENC_ReadReg(ECON1, &status);
    }
    ENC_ReadReg(EIR, &status);
    if (status & EIR_TXERIF) {
        //GPIOC->ODR ^= GPIO_ODR_ODR13;
        return SPI_ERR;
    }
    return SPI_OK;
}

SPI_Status_t ENC_SendPacketRaw(uint8_t *data, uint32_t size) {
    //ENC_BitSet(ECON1, ECON1_TXRTS);
    //ENC_BitClear(ECON1, ECON1_TXRTS);

    //ENC_WriteBufferMemory(ETH_TX_BUFFER_START + 1, header, ETH_HEAD_SIZE);
    uint8_t zero = 0x00;
    ENC_WriteBufferMemory(ETH_TX_BUFFER_START, &zero, 1);
    ENC_WriteBufferMemory(ETH_TX_BUFFER_START + 1, data, size);

    uint16_t ptr = ETH_TX_BUFFER_START + size + 1;
    ENC_WriteReg(ETXNDL, ptr & 0xFF);
    ENC_WriteReg(ETXNDH, ptr >> 8);
    ENC_BitSet(ECON1, ECON1_TXRTS);

    uint8_t status = 0;
    ENC_ReadReg(ECON1, &status);
    while (status & ECON1_TXRTS) {
        ENC_ReadReg(ECON1, &status);
    }
    ENC_ReadReg(EIR, &status);
    if (status & EIR_TXERIF) {
        //GPIOC->ODR ^= GPIO_ODR_ODR13;
        return SPI_ERR;
    }
    return SPI_OK;
}

uint32_t ENC_RecievePacket(uint8_t data[]) {
    uint8_t status = 0;
    ENC_SEL_BANK1();
    ENC_ReadReg(EPKTCNT, &status);
    if (status == 0) {
        return 0;
    }
    uint8_t rx_vector[ETH_RX_VEC_LEN];
    // Potential bug, if vector is right at the edge of the buffer
    ENC_ReadBufferMemory(_next_packet_ptr, rx_vector, ETH_RX_VEC_LEN);
    uint32_t size = 0;
    size = (rx_vector[3] << 8) | rx_vector[2]; // Exclude CRC

    ENC_ReadBufferMemory(_next_packet_ptr + ETH_RX_VEC_LEN, data, size);
    _next_packet_ptr = (rx_vector[1] << 8) | rx_vector[0];

    uint16_t erxrdpt = 0;
    if (_next_packet_ptr - 1 < ETH_RX_BUFFER_START || \
        _next_packet_ptr - 1 > ETH_RX_BUFFER_START + ETH_RX_BUFFER_SIZE) {
            erxrdpt = ETH_RX_BUFFER_START + ETH_RX_BUFFER_SIZE;
    } else {
        erxrdpt = _next_packet_ptr - 1;
    }
    ENC_WriteReg(ERXRDPTL, erxrdpt & 0xFF);
    ENC_WriteReg(ERXRDPTH, erxrdpt >> 8);

    ENC_BitSet(ECON2, ECON2_PKTDEC);
    return size;
}