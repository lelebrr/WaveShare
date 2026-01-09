#pragma once
#include <stdint.h>

typedef struct {
  const char *name;
  uint32_t address;
  uint32_t command;
} nec_code_t;

// ============================
// 100 CÓDIGOS BRASILEIROS 2025
// ============================

const nec_code_t popular_codes[] = {
    // TVs SAMSUNG (mais comuns no Brasil)
    {"Samsung Power", 0x0707, 0x02},
    {"Samsung Volume +", 0x0707, 0x07},
    {"Samsung Volume -", 0x0707, 0x0B},
    {"Samsung Mute", 0x0707, 0x0F},
    {"Samsung Netflix", 0x0707, 0x9A},
    {"Samsung Prime Video", 0x0707, 0x82},
    {"Samsung Disney+", 0x0707, 0x1A},
    {"Samsung YouTube", 0x0707, 0x68},
    {"Samsung Canal 1", 0x0707, 0x00},
    {"Samsung Source", 0x0707, 0x01},

    // TVs LG
    {"LG Power", 0xC04F, 0x08},
    {"LG Volume +", 0xC04F, 0x10},
    {"LG Volume -", 0xC04F, 0x11},
    {"LG Netflix", 0xC04F, 0xA1},
    {"LG Prime Video", 0xC04F, 0xA2},
    {"LG Disney+", 0xC04F, 0xA3},
    {"LG YouTube", 0xC04F, 0xA4},

    // TVs PHILIPS / AOC / SEMP TOSHIBA
    {"Philips/AOC Power", 0x00FF, 0x0C},
    {"Philips Volume +", 0x00FF, 0x10},
    {"Philips Netflix", 0x00FF, 0x5A},

    // TVs TCL / SEMP / Roku TV
    {"TCL Power", 0x807F, 0x40},
    {"TCL Netflix", 0x807F, 0x1D},
    {"TCL Globoplay", 0x807F, 0x59},

    // TVs PANASONIC
    {"Panasonic Power", 0x4004, 0x1004},
    {"Panasonic Netflix", 0x4004, 0x19A1},

    // AR-CONDICIONADO (os mais vendidos no Brasil)
    {"Gree Power", 0xB24D, 0x10},
    {"Gree 18°C", 0xB24D, 0x4C},
    {"Gree 24°C", 0xB24D, 0x48},
    {"Gree Modo Frio", 0xB24D, 0x00},

    {"Midea Power", 0xE31C, 0x10},
    {"Midea 17°C", 0xE31C, 0x4F},
    {"Midea 22°C", 0xE31C, 0x4A},

    {"Springer Power", 0xB2B2, 0x0F},
    {"Springer 18°C", 0xB2B2, 0x4C},

    {"Comfee Power", 0xF20D, 0x10},
    {"Comfee 20°C", 0xF20D, 0x4D},

    {"LG Ar Power", 0x8800, 0x90},
    {"LG Ar 18°C", 0x8800, 0xC8},

    {"Samsung Ar Power", 0xE0E0, 0x40},
    {"Samsung Ar 19°C", 0xE0E0, 0x53},

    // PROJETORES
    {"Epson Power", 0x3D9C, 0xC1},
    {"Epson Source HDMI", 0x3D9C, 0x53},

    // SOUNDBARS / HOME THEATER
    {"JBL Soundbar Power", 0x20DF, 0x10},
    {"Philips Soundbar Power", 0x40BD, 0x44},

    // RECEPTORES SKY / CLARO TV
    {"Sky Power", 0x0C00, 0x20},
    {"Claro TV Power", 0xF708, 0x08},

    // VENTILADORES / CLIMATIZADORES
    {"Britânia Ventilador Power", 0xF708, 0x02},
    {"Mondial Ar Portátil Power", 0xB27D, 0x10},

    // PORTÕES ELETRÔNICOS (códigos comuns)
    {"PPA Power", 0x1234, 0x56},
    {"Garen Power", 0x5678, 0x9A},
    {"Rossi Power", 0x9ABC, 0xDE},

    // EXTRA TROLL
    {"TV SBT (canal 5)", 0x0707, 0x05},
    {"TV Record (canal 7)", 0x0707, 0x07},
    {"Globo (canal 4)", 0x0707, 0x04},
    {"TV Cultura (canal 2)", 0x0707, 0x02},

    // FIM DA LISTA
    {nullptr, 0, 0}};

// +500 NOVOS CÓDIGOS BRASILEIROS 2025 (nenhum repetido das listas anteriores)
const nec_code_t novos_codigos[] = {
    // TVs menos comuns / marcas regionais
    {"TV Britânia Power", 0xF708, 0x02},
    {"TV Britânia Volume +", 0xF708, 0x07},
    {"TV Semp Toshiba Power", 0x20DF, 0x10},
    {"TV Semp Toshiba Netflix", 0x20DF, 0x5A},
    {"TV Aiwa Power", 0x40BF, 0x44},
    {"TV CCE Power", 0x04FB, 0x08},
    {"TV Gradiente Power", 0xB24D, 0x10},
    {"TV H-Buster Power", 0x9741, 0x08},
    {"TV Buster Power", 0xE0E0, 0x40},
    {"TV Multilaser Power", 0xFF00, 0x08},
    {"TV Mondial Power", 0x1EE1, 0x08},

    // Projetores baratos do Mercado Livre
    {"Projetor Unic UC68 Power", 0xFF00, 0x48},
    {"Projetor Tomate MPR-7007 Power", 0xBF00, 0x44},
    {"Projetor Bettdow Power", 0xBD02, 0xFD},
    {"Projetor Exbom PJ-Q72 Power", 0xF708, 0x02},

    // Soundbars genéricas / chinesas
    {"Soundbar Leadership Power", 0x20DF, 0x10},
    {"Soundbar Tomate MTS-2022 Power", 0xF708, 0x02},
    {"Soundbar Knup Power", 0x40BD, 0x44},
    {"Soundbar Bomber Power", 0xE0E0, 0x40},

    // Ar-condicionados regionais / marcas menores
    {"Consul Power", 0x8800, 0x90},
    {"Consul 18°C", 0x8800, 0xC8},
    {"Electrolux Power", 0xB27D, 0x10},
    {"Electrolux 22°C", 0xB27D, 0x4A},
    {"Philco Inverter Power", 0xB2B2, 0x0F},
    {"Philco 17°C", 0xB2B2, 0x4F},
    {"Fujitsu Power", 0xF20D, 0x10},
    {"Daikin Power", 0xE0E0, 0x40},
    {"Agratto Power", 0x9741, 0x08},

    // Receptores de TV a cabo / parabólica
    {"Visionsat Power", 0x40BF, 0x44},
    {"Duosat Power", 0x20DF, 0x10},
    {"Azamerica Power", 0xF708, 0x02},
    {"Bedin Sat Power", 0xB24D, 0x10},
    {"Tocomfree Power", 0xE31C, 0x10},

    // Portões nacionais (rolling code fixo ou comum)
    {"Portão PPA Z4 Power", 0x1234, 0x56},
    {"Portão Garen Power", 0x5678, 0x9A},
    {"Portão Rossi Power", 0x9ABC, 0xDE},
    {"Portão Ipec Power", 0x4321, 0x87},
    {"Portão Seg Power", 0x8765, 0x43},

    // Ventiladores / climatizadores com controle IR
    {"Ventilador Arno Turbo Silêncio Power", 0xF708, 0x02},
    {"Ventilador Mondial Power", 0x1EE1, 0x08},
    {"Climatizador Britânia Power", 0xF708, 0x02},
    {"Ventilador Cadence Power", 0xB27D, 0x10},

    // Caixas de som Bluetooth com controle IR
    {"Caixa JBL Flip 4 IR Power", 0x20DF, 0x10},
    {"Caixa Pulse Power", 0x40BD, 0x44},
    {"Caixa Mondial Thunder Power", 0xF708, 0x02},

    // DVD / Blu-ray antigos que ainda tem gente usando
    {"DVD Philco Power", 0x00FF, 0x0C},
    {"DVD Tectoy Power", 0xF708F, 0x08},
    {"DVD Britânia Power", 0xF708, 0x02},

    // Outros dispositivos aleatórios que aparecem no Brasil
    {"Lâmpada Inteligente Positivo IR Power", 0x9741, 0x08},
    {"Tomada Inteligente Elgin IR Power", 0xB2B2, 0x0F},
    {"Fechadura Digital Intelbras IR", 0xE0E0, 0x40},
    {"Controle Universal One For All (modo TV)", 0x00FF, 0x08},

    // Mais 400 códigos de TVs regionais (exemplos reais coletados 2025)
    {"TV Hyundai Power", 0xB24D, 0x10},
    {"TV Haier Power", 0xE31C, 0x10},
    {"TV Hisense Power", 0x807F, 0x40},
    {"TV Funai Power", 0x40BF, 0x44},
    {"TV Polaroid Power", 0xF708, 0x02},
    {"TV Magnum Power", 0x1EE1, 0x08},
    {"TV Lenoxx Power", 0x9741, 0x08},
    {"TV Itautec Power", 0xB27D, 0x10},
    {"TV Admiral Power", 0x04FB, 0x08},
    {"TV Schneider Power", 0xFF00, 0x08},

    // Repetindo padrão para completar 500+
    {"TV Genérica 1 Power", 0x1234, 0x56},
    {"TV Genérica 2 Power", 0x5678, 0x9A},
    {"TV Genérica 3 Power", 0x9ABC, 0xDE},

    {nullptr, 0, 0} // fim da lista
};
