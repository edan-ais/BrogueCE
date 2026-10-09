// Include the real implementation to exercise its file-local entrancement helper.
// The runner excludes Movement.o to avoid duplicate symbols.
#include "../src/brogue/Movement.c"
#include "../src/platform/platform.h"
int main(void) {
    currentConsole = nullConsole;
    initializeGameVariant();
    initializeRogue(396);
    rogue.playbackMode = true;
    rogue.automationActive = true;
    for (int x = 0; x < DCOLS; x++) {
        for (int y = 0; y < DROWS; y++) {
            memset(&pmap[x][y], 0, sizeof(pcell));
            pmap[x][y].layers[DUNGEON] = FLOOR;
        }
    }
    creature *m = generateMonster(MK_RAT, false, false);
    // Keep the fixture deterministic and separate from turn processing.
    m->loc = (pos){20, 10};
    m->status[STATUS_ENTRANCED] = 10;
    m->status[STATUS_STUCK] = 3;
    pmap[20][10].flags |= HAS_MONSTER;
    pmap[20][10].layers[SURFACE] = SPIDERWEB;
    moveEntrancedMonsters(LEFT);
    printf("After entranced move: stuck=%d (expected 2), x=%d (expected 20)\n", m->status[STATUS_STUCK], m->loc.x);
    if (m->status[STATUS_STUCK] != 2 || m->loc.x != 20) return 1;
    moveEntrancedMonsters(LEFT);
    if (m->status[STATUS_STUCK] != 1 || m->loc.x != 20) return 2;
    moveEntrancedMonsters(LEFT);
    if (m->status[STATUS_STUCK] != 0 || m->loc.x != 21
        || pmap[20][10].layers[SURFACE] != NOTHING) return 3;
    m->status[STATUS_STUCK] = 3;
    m->status[STATUS_PARALYZED] = 2;
    pmap[21][10].layers[SURFACE] = SPIDERWEB;
    moveEntrancedMonsters(LEFT);
    if (m->status[STATUS_STUCK] != 3 || m->loc.x != 21) return 4;
    m->status[STATUS_PARALYZED] = 0;
    m->bookkeepingFlags |= MB_CAPTIVE;
    moveEntrancedMonsters(LEFT);
    if (m->status[STATUS_STUCK] != 3 || m->loc.x != 21) return 5;
    m->bookkeepingFlags &= ~MB_CAPTIVE;
    m->status[STATUS_ENTRANCED] = 0;
    moveEntrancedMonsters(LEFT);
    if (m->status[STATUS_STUCK] != 3 || m->loc.x != 21) return 6;
    m->status[STATUS_ENTRANCED] = 10;
    creature *defender = generateMonster(MK_RAT, false, false);
    defender->loc = (pos){22, 10};
    defender->creatureState = MONSTER_ALLY;
    defender->info.defense = 0;
    defender->currentHP = defender->info.maxHP = 1000;
    pmap[22][10].flags |= HAS_MONSTER;
    m->info.accuracy = 1000;
    m->info.damage = (randomRange){2, 2, 1};
    m->ticksUntilTurn = 0;
    moveEntrancedMonsters(LEFT);
    if (defender->currentHP >= 1000 || m->loc.x != 21
        || m->status[STATUS_STUCK] != 3 || m->ticksUntilTurn != m->attackSpeed) return 7;
    puts("PASS: repeated web escape, web removal, paralysis, captivity, unentranced exclusions and attack while stuck");
    return 0;
}
