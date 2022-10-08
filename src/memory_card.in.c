#define XOR8(a) (a[0] ^ a[1] ^ a[2] ^ a[3] ^ a[4] ^ a[5] ^ a[6] ^ a[7])
#define ARG8(a) a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]

/* resp to 0x81 */
send(0xFF);

/* sub cmd */
ch = recv_cmd();
if (ch == 0x11) {
    send(0x2B); recv_cmd();
    send(term);
} else if (ch == 0x12) {
    send(0x2B); recv_cmd();
    send(term);
} else if (ch == 0x21) {
    /* set address for erase */
    union {
        uint8_t a[4];
        uint32_t addr;
    } raw;
    uint8_t ck;
    send(0xFF); raw.a[0] = recv_cmd();
    send(0xFF); raw.a[1] = recv_cmd();
    send(0xFF); raw.a[2] = recv_cmd();
    send(0xFF); raw.a[3] = recv_cmd();
    send(0xFF); ck = recv_cmd();
    send(0x2B); recv_cmd();
    erase_sector = raw.addr;
    send(term);
} else if (ch == 0x22) {
    /* set address for write */
    union {
        uint8_t a[4];
        uint32_t addr;
    } raw;
    uint8_t ck;
    send(0xFF); raw.a[0] = recv_cmd();
    send(0xFF); raw.a[1] = recv_cmd();
    send(0xFF); raw.a[2] = recv_cmd();
    send(0xFF); raw.a[3] = recv_cmd();
    send(0xFF); ck = recv_cmd();
    send(0x2B); recv_cmd();
    write_sector = raw.addr;
    is_write = 1;
    writeptr = 0;
    send(term);
} else if (ch == 0x23) {
    /* set address for read */
    union {
        uint8_t a[4];
        uint32_t addr;
    } raw;
    uint8_t ck;
    send(0xFF); raw.a[0] = recv_cmd();
    send(0xFF); raw.a[1] = recv_cmd();
    send(0xFF); raw.a[2] = recv_cmd();
    send(0xFF); raw.a[3] = recv_cmd();
    send(0xFF); ck = recv_cmd();
    send(0x2B); recv_cmd();
    read_sector = raw.addr;
    if (read_sector * 512 + 512 <= CARD_SIZE) {
        dirty_lockout_renew();
        /* the spinlock will be unlocked by the DMA irq once all data is tx'd */
        dirty_lock();
        psram_read_dma(read_sector * 512, &readtmp, 512+4);
        // dma_channel_wait_for_finish_blocking(0);
        // dma_channel_wait_for_finish_blocking(1);
        // printf("first %02X %02X %02X %02X\n", readtmp.buf[0], readtmp.buf[1], readtmp.buf[2], readtmp.buf[3]);
    }
    readptr = 0;

    eccptr = &readtmp.buf[512];
    memset(eccptr, 0, 16);

    send(term);
} else if (ch == 0x26) {
    /* GET_SPECS ? */
    send(0x2B); recv_cmd();

    uint8_t specs[] = { 0x00, 0x02, ERASE_SECTORS, 0x00, 0x00, 0x40, 0x00, 0x00 };

    send(specs[0]); recv_cmd();
    send(specs[1]); recv_cmd();
    send(specs[2]); recv_cmd();
    send(specs[3]); recv_cmd();
    send(specs[4]); recv_cmd();
    send(specs[5]); recv_cmd();
    send(specs[6]); recv_cmd();
    send(specs[7]); recv_cmd();
    send(XOR8(specs)); recv_cmd();
    send(term);
} else if (ch == 0x27) {
    /* SET_TERMINATOR */
    send(0xFF);
    term = recv_cmd();
    send(0x2B); recv_cmd();
    send(term);
} else if (ch == 0x28) {
    /* GET_TERMINATOR */
    send(0x2B); recv_cmd();
    send(term); recv_cmd();
    send(term);
} else if (ch == 0x42) {
    /* write data */
    uint8_t sz;
    send(0xFF); sz = recv_cmd();
    send(0xFF);

    uint8_t ck = 0;
    uint8_t b;

    for (int i = 0; i < sz; ++i) {
        b = recv_cmd();
        if (writeptr < sizeof(writetmp)) {
            writetmp[writeptr] = b;
            ++writeptr;
        }
        ck ^= b;
        send(0xFF);
    }
    // this should be checksum?
    uint8_t ck2 = recv_cmd();

    send(0x2B); recv_cmd();
    send(term);
} else if (ch == 0x43) {
    /* read data */
    uint8_t sz;
    send(0xFF); sz = recv_cmd();
    send(0x2B); recv_cmd();

    uint8_t ck = 0;
    uint8_t b = 0xFF;

    for (int i = 0; i < sz; ++i) {
        if (readptr < sizeof(readtmp.buf)) {
            b = readtmp.buf[readptr];
            ++readptr;

            if (readptr <= 512) {
                uint8_t c = Table[b];
                eccptr[0] ^= c;
                if (c & 0x80) {
                    eccptr[1] ^= ~i;
                    eccptr[2] ^= i;
                }

                if ((readptr & 0x7F) == 0) {
                    eccptr[0] = ~eccptr[0];
                    eccptr[0] &= 0x77;

                    eccptr[1] = ~eccptr[1];
                    eccptr[1] &= 0x7f;

                    eccptr[2] = ~eccptr[2];
                    eccptr[2] &= 0x7f;

                    eccptr += 3;
                }
            }
        }
        ck ^= b;
        send(b); recv_cmd();
    }

    send(ck); recv_cmd();
    send(term);
} else if (ch == 0x81) {
    /* commit for read/write? */
    if (is_write) {
        is_write = 0;
        if (write_sector * 512 + 512 <= CARD_SIZE) {
            dirty_lockout_renew();
            dirty_lock();
            psram_write(write_sector * 512, writetmp, 512);
            dirty_mark(write_sector);
            dirty_unlock();
        }
    }

    send(0x2B); recv_cmd();
    send(term);
} else if (ch == 0x82) {
    /* do erase */
    if (erase_sector * 512 + 512 * ERASE_SECTORS <= CARD_SIZE) {
        memset(readtmp.buf, 0xFF, 512);
        dirty_lockout_renew();
        dirty_lock();
        for (int i = 0; i < ERASE_SECTORS; ++i) {
            psram_write((erase_sector + i) * 512, readtmp.buf, 512);
            dirty_mark(erase_sector + i);
        }
        dirty_unlock();
    }
    send(0x2B); recv_cmd();
    send(term);
} else if (ch == 0xBF) {
    send(0xFF); recv_cmd();
    send(0x2B); recv_cmd();
    send(term);
} else if (ch == 0xF3) {
    send(0xFF); recv_cmd();
    send(0x2B); recv_cmd();
    send(term);
} else if (ch == 0xF7) {
    /* SIO_MEMCARD_KEY_SELECT */
    send(0xFF); recv_cmd();
    send(0x2B); recv_cmd();
    send(term);
} else if (ch == 0xF0) {
    /* auth stuff */
    send(0xFF);
    int subcmd = recv_cmd();
    if (subcmd == 0) {
        /* probe support ? */
        send(0x2B); recv_cmd();
        send(term);
    } else if (subcmd == 1) {
        debug_printf("iv : %02X %02X %02X %02X %02X %02X %02X %02X\n", ARG8(iv));

        /* get IV */
        send(0x2B); recv_cmd();
        send(iv[7]); recv_cmd();
        send(iv[6]); recv_cmd();
        send(iv[5]); recv_cmd();
        send(iv[4]); recv_cmd();
        send(iv[3]); recv_cmd();
        send(iv[2]); recv_cmd();
        send(iv[1]); recv_cmd();
        send(iv[0]); recv_cmd();
        send(XOR8(iv)); recv_cmd();
        send(term);
    } else if (subcmd == 2) {
        debug_printf("seed : %02X %02X %02X %02X %02X %02X %02X %02X\n", ARG8(seed));

        /* get seed */
        send(0x2B); recv_cmd();
        send(seed[7]); recv_cmd();
        send(seed[6]); recv_cmd();
        send(seed[5]); recv_cmd();
        send(seed[4]); recv_cmd();
        send(seed[3]); recv_cmd();
        send(seed[2]); recv_cmd();
        send(seed[1]); recv_cmd();
        send(seed[0]); recv_cmd();
        send(XOR8(seed)); recv_cmd();
        send(term);
    } else if (subcmd == 3) {
        /* dummy 3 */
        send(0x2B); recv_cmd();
        send(term);
    } else if (subcmd == 4) {
        debug_printf("nonce : %02X %02X %02X %02X %02X %02X %02X %02X\n", ARG8(nonce));

        /* get nonce */
        send(0x2B); recv_cmd();
        send(nonce[7]); recv_cmd();
        send(nonce[6]); recv_cmd();
        send(nonce[5]); recv_cmd();
        send(nonce[4]); recv_cmd();
        send(nonce[3]); recv_cmd();
        send(nonce[2]); recv_cmd();
        send(nonce[1]); recv_cmd();
        send(nonce[0]); recv_cmd();
        send(XOR8(nonce)); recv_cmd();
        send(term);
    } else if (subcmd == 5) {
        /* dummy 5 */
        send(0x2B); recv_cmd();
        send(term);
    } else if (subcmd == 6) {
        /* MechaChallenge3 */
        send(0xFF);
        MechaChallenge3[7] = recv_cmd();
        send(0xFF);
        MechaChallenge3[6] = recv_cmd();
        send(0xFF);
        MechaChallenge3[5] = recv_cmd();
        send(0xFF);
        MechaChallenge3[4] = recv_cmd();
        send(0xFF);
        MechaChallenge3[3] = recv_cmd();
        send(0xFF);
        MechaChallenge3[2] = recv_cmd();
        send(0xFF);
        MechaChallenge3[1] = recv_cmd();
        send(0xFF);
        MechaChallenge3[0] = recv_cmd();
        /* TODO: checksum below */
        send(0xFF); recv_cmd();
        send(0x2B); recv_cmd();
        send(term);

        debug_printf("MechaChallenge3 : %02X %02X %02X %02X %02X %02X %02X %02X\n", ARG8(MechaChallenge3));
    } else if (subcmd == 7) {
        /* MechaChallenge2 */
        send(0xFF);
        MechaChallenge2[7] = recv_cmd();
        send(0xFF);
        MechaChallenge2[6] = recv_cmd();
        send(0xFF);
        MechaChallenge2[5] = recv_cmd();
        send(0xFF);
        MechaChallenge2[4] = recv_cmd();
        send(0xFF);
        MechaChallenge2[3] = recv_cmd();
        send(0xFF);
        MechaChallenge2[2] = recv_cmd();
        send(0xFF);
        MechaChallenge2[1] = recv_cmd();
        send(0xFF);
        MechaChallenge2[0] = recv_cmd();
        /* TODO: checksum below */
        send(0xFF); recv_cmd();
        send(0x2B); recv_cmd();
        send(term);

        debug_printf("MechaChallenge2 : %02X %02X %02X %02X %02X %02X %02X %02X\n", ARG8(MechaChallenge2));
    } else if (subcmd == 8) {
        /* dummy 8 */
        send(0x2B); recv_cmd();
        send(term);
    } else if (subcmd == 9) {
        /* dummy 9 */
        send(0x2B); recv_cmd();
        send(term);
    } else if (subcmd == 0xA) {
        /* dummy A */
        send(0x2B); recv_cmd();
        send(term);
    } else if (subcmd == 0xB) {
        /* MechaChallenge1 */
        send(0xFF);
        MechaChallenge1[7] = recv_cmd();
        send(0xFF);
        MechaChallenge1[6] = recv_cmd();
        send(0xFF);
        MechaChallenge1[5] = recv_cmd();
        send(0xFF);
        MechaChallenge1[4] = recv_cmd();
        send(0xFF);
        MechaChallenge1[3] = recv_cmd();
        send(0xFF);
        MechaChallenge1[2] = recv_cmd();
        send(0xFF);
        MechaChallenge1[1] = recv_cmd();
        send(0xFF);
        MechaChallenge1[0] = recv_cmd();
        /* TODO: checksum below */
        send(0xFF); recv_cmd();
        send(0x2B); recv_cmd();
        send(term);

        debug_printf("MechaChallenge1 : %02X %02X %02X %02X %02X %02X %02X %02X\n", ARG8(MechaChallenge1));
    } else if (subcmd == 0xC) {
        /* dummy C */
        send(0x2B); recv_cmd();
        send(term);
    } else if (subcmd == 0xD) {
        /* dummy D */
        send(0x2B); recv_cmd();
        send(term);
    } else if (subcmd == 0xE) {
        /* dummy E */
        generateResponse();
        debug_printf("CardResponse1 : %02X %02X %02X %02X %02X %02X %02X %02X\n", ARG8(CardResponse1));
        debug_printf("CardResponse2 : %02X %02X %02X %02X %02X %02X %02X %02X\n", ARG8(CardResponse2));
        debug_printf("CardResponse3 : %02X %02X %02X %02X %02X %02X %02X %02X\n", ARG8(CardResponse3));
        send(0x2B); recv_cmd();
        send(term);
    } else if (subcmd == 0xF) {
        /* CardResponse1 */
        send(0x2B); recv_cmd();
        send(CardResponse1[7]); recv_cmd();
        send(CardResponse1[6]); recv_cmd();
        send(CardResponse1[5]); recv_cmd();
        send(CardResponse1[4]); recv_cmd();
        send(CardResponse1[3]); recv_cmd();
        send(CardResponse1[2]); recv_cmd();
        send(CardResponse1[1]); recv_cmd();
        send(CardResponse1[0]); recv_cmd();
        send(XOR8(CardResponse1)); recv_cmd();
        send(term);
    } else if (subcmd == 0x10) {
        /* dummy 10 */
        send(0x2B); recv_cmd();
        send(term);
    } else if (subcmd == 0x11) {
        /* CardResponse2 */
        send(0x2B); recv_cmd();
        send(CardResponse2[7]); recv_cmd();
        send(CardResponse2[6]); recv_cmd();
        send(CardResponse2[5]); recv_cmd();
        send(CardResponse2[4]); recv_cmd();
        send(CardResponse2[3]); recv_cmd();
        send(CardResponse2[2]); recv_cmd();
        send(CardResponse2[1]); recv_cmd();
        send(CardResponse2[0]); recv_cmd();
        send(XOR8(CardResponse2)); recv_cmd();
        send(term);
    } else if (subcmd == 0x12) {
        /* dummy 12 */
        send(0x2B); recv_cmd();
        send(term);
    } else if (subcmd == 0x13) {
        /* CardResponse3 */
        send(0x2B); recv_cmd();
        send(CardResponse3[7]); recv_cmd();
        send(CardResponse3[6]); recv_cmd();
        send(CardResponse3[5]); recv_cmd();
        send(CardResponse3[4]); recv_cmd();
        send(CardResponse3[3]); recv_cmd();
        send(CardResponse3[2]); recv_cmd();
        send(CardResponse3[1]); recv_cmd();
        send(CardResponse3[0]); recv_cmd();
        send(XOR8(CardResponse3)); recv_cmd();
        send(term);
    } else if (subcmd == 0x14) {
        /* dummy 14 */
        send(0x2B); recv_cmd();
        send(term);
    }
} else if (ch == 0xF1 || ch == 0xF2) {
    static uint8_t hostkey[9];
    /* session key encrypt */
    send(0xFF);
    int subcmd = recv_cmd();
    if (subcmd == 0x50 || subcmd == 0x40) {
        send(0x2B); recv_cmd();
        send(term);
    } else if (subcmd == 0x51 || subcmd == 0x41) {
        /* host sends key to us */
        for (int i = 0; i < sizeof(hostkey); ++i) {
            send(0xFF);
            hostkey[i] = recv_cmd();
        }
        send(0x2B); recv_cmd();
        send(term);
    } else if (subcmd == 0x52 || subcmd == 0x42) {
        /* now we "encrypt" the key */
        send(0x2B); recv_cmd();
        send(term);
    } else if (subcmd == 0x53 || subcmd == 0x43) {
        send(0x2B); recv_cmd();
        /* we send key to the host */
        for (int i = 0; i < sizeof(hostkey); ++i) {
            send(hostkey[i]);
            recv_cmd();
        }
        send(term);
    } else {
        debug_printf("!! unhandled subcmd %02X -> %02X\n", 0xF2, subcmd);
    }
} else {
    debug_printf("!! unknown %02X\n", ch);
}

#undef XOR8
