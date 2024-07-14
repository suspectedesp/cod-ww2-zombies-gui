#pragma once
#ifndef OFFSETS_H
#define OFFSETS_H

//amounts
struct amounts {
    static const int health = 100;
    static const int weapons = 100;
    static const int lethals = 4;
};

//self
struct self {
    static const uintptr_t HealthAddress = 0x0A2D7DC8;
    static const uintptr_t HealthOffset = 0x2DC;
    static const uintptr_t UsernameAddress = 0x0E4650B0;
    static const uintptr_t UsernameOffset = 0x144;
};

//weapons
struct weapons {
    static const uintptr_t _1911 = 0x0A0C7388;
    static const uintptr_t _1911_pistole1 = 0x784;
    static const uintptr_t _1911_pistole2 = 0x780;
    static const uintptr_t m1Garand = 0x0A0C7388;
    static const uintptr_t m1GarandOffset = 0x7C8;
    static const uintptr_t m1928 = 0x0A0C7388;
    static const uintptr_t m1928Offset = 0x7C8;
    static const uintptr_t m30LuftwaffeDrilling = 0x0A0C7388;
    static const uintptr_t m30luftwaffeDrillingOffset = 0x7B0;
    static const uintptr_t machinePistole = 0x0A0C7388;
    static const uintptr_t machinePistole_pistole1 = 0x7B4;
    static const uintptr_t machinePistole_pistole2 = 0x7B0;
    static const uintptr_t svt40 = 0x0A0C7388;
    static const uintptr_t svt40Offset = 0x7B0;
    static const uintptr_t teslaGun = 0x0C804900;
    static const uintptr_t teslaGunOffset = 0x7B0;
    static const uintptr_t type100 = 0x0A0C7388;
    static const uintptr_t type100Offset = 0x7C8;
};

//special
struct special {
    static const int ability_max_amount = 1073741824;
    static const uintptr_t ability_address = 0xA4B1888;
};

//lethal
struct lethal {
    static const uintptr_t lethalWeapon = 0x0A0C7388;
    static const uintptr_t lethalWeaponOffset = 0x768;
};

#endif