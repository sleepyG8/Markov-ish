#include <stdio.h>

// Tamper with these map size and observe results 5 is best, keep it above 3
#define HASHMAPSIZE 5
#define STORYSIZE 0x1000

typedef struct {
    unsigned char bytes[128];
    int weight;
    void* nextBytes;
} WORDS;

typedef struct {
WORDS entry[20];
} hashmap;

hashmap map[HASHMAPSIZE];

char* nouns[] = {
    "dog",
    "man",
    "woman",
    "child",
    "river",
    "forest",
    "city",
    "machine",
    "tower",
    "stone",
    "light",
    "shadow",
    "memory",
    "story",
    "dream",
    "road",
    "world",
    "field",
    "voice",
    "pattern",
    "system",
    "signal",
    "bridge",
    "mountain",
    "valley",
    "ocean",
    "wave",
    "cloud",
    "storm",
    "fire",
    "wind",
    "path",
    "gate",
    "hall",
    "room",
    "table",
    "book",
    "letter",
    "engine",
    "circuit",
    "frame",
    "shell",
    "core",
    "module",
    "thread",
    "process",
    "sector",
    "archive",
    "memory",
    0
};

WORDS inString[] = {
    
    // Determiners (weight 5 — higher than adjectives so they appear first)
    {"the", 5, 0},
    {"a", 5, 0},
    {"an", 5, 0},
    {"this", 5, 0},
    {"that", 5, 0},
    {"these", 5, 0},
    {"those", 5, 0},
    {"and", 9, 0},

    // Nouns (1)
    {"man", 1, 0},
    {"river", 1, 0},
    {"machine", 1, 0},
    {"forest", 1, 0},
    {"city", 1, 0},
    {"child", 1, 0},
    {"story", 1, 0},
    {"light", 1, 0},
    {"stone", 1, 0},
    {"dream", 1, 0},
    {"world", 1, 0},
    {"road", 1, 0},
    {"signal", 1, 0},
    {"tower", 1, 0},
    {"memory", 1, 0},
    {"shadow", 1, 0},
    {"field", 1, 0},
    {"voice", 1, 0},
    {"pattern", 1, 0},
    {"system", 1, 0},

    // Verbs (2–3)
    {"run", 2, 0},
    {"walk", 2, 0},
    {"speak", 2, 0},
    {"build", 3, 0},
    {"break", 3, 0},
    {"watch", 2, 0},
    {"follow", 3, 0},
    {"carry", 2, 0},
    {"shape", 3, 0},
    {"move", 2, 0},
    {"listen", 3, 0},
    {"open", 2, 0},
    {"close", 2, 0},
    {"search", 3, 0},
    {"create", 3, 0},
    {"hold", 2, 0},
    {"turn", 2, 0},
    {"push", 2, 0},
    {"pull", 2, 0},
    {"reach", 3, 0},

    // Adjectives (4)
    {"bright", 4, 0},
    {"silent", 4, 0},
    {"ancient", 4, 0},
    {"heavy", 4, 0},
    {"soft", 4, 0},
    {"cold", 4, 0},
    {"warm", 4, 0},
    {"strange", 4, 0},
    {"quick", 4, 0},
    {"deep", 4, 0},
    {"wild", 4, 0},
    {"gentle", 4, 0},
    {"broken", 4, 0},
    {"hidden", 4, 0},
    {"quiet", 4, 0},
    {"faint", 4, 0},
    {"vast", 4, 0},
    {"sharp", 4, 0},
    {"distant", 4, 0},

    {0, 0, 0}
};

WORDS* fallBack(int weight) {
    for (int i=0; i < 5; i++) {
        for (int j=0; j < 20; j++) {
            if (map[i].entry[j].weight == weight) {
                return &map[i].entry[j];
            }
        }
    }

    return 0;
}

int num = 0;
int getrand(int maxNum) {
    unsigned long long rand = 00;
    unsigned long long rand2 = 00;
    int finalNum = 0;

    for (int p=0; ; p++) {
    _rdrand64_step(&rand);
    _rdrand64_step(&rand2);
    finalNum = (rand >> 56) << 8 | (rand2 >> 56);
    finalNum = finalNum % 256;
    if ((finalNum) <= maxNum && (finalNum) > 00) break;
    }

    return finalNum;

}

int hash(char* key) {

    unsigned long long hash = 0;
    for (int i=0; i < strlen(key); i++) {
        hash ^= num * key[i] | hash << (8 * i); 
    }

    return hash % HASHMAPSIZE;
}

int GlyphHashmapStore(WORDS* key) {

    if (strlen(key->bytes) < 2) return 3;

    int res = hash(key->bytes);

    for (int i=0; i < 20; i++) {
    if (map[res].entry[i].weight == 0) {
    strncpy(map[res].entry[i].bytes, key->bytes, 15);
    map[res].entry[i].weight = key->weight;
    //map[res].entry[i].nextBytes = map + strlen(key) + 2;
    break;
    }
    }

    return 0;
}

WORDS* GlyphHashmapGet(char* key, int weight, int inNum) {

    int res = hash(key);

    int weightCount = 0;
    for (int i=0; i < 20; i++) {

        if (strcmp(map[res].entry[i].bytes, "null") == 0) break;

        if (weight == map[res].entry[i].weight) {
            weightCount++;

            if (weightCount == inNum) {
            return &map[res].entry[i];
            }

        }

    }

    return 0;
}

int initMarkov() {

    int i = 0;
    while(inString[i].weight != 00) {
        GlyphHashmapStore(&inString[i]);
        i++;
    }

    return 0;
}

WORDS* markov(unsigned char* in, int event, int weight) {

    int weightCount = 0;
    int res = hash(in);

    for (int i=0; i < 20; i++) {
        if (map[res].entry[i].weight == 0) break;
        if (map[res].entry[i].weight == weight) {
            weightCount++;
        }
    }

    int tempNum = 1;
    if (weightCount != 0) {
    // this gets rand entry inside map
    tempNum = getrand(weightCount);
    }

    WORDS* out = GlyphHashmapGet(in, weight, tempNum);
    if (out == 0) {
        out = fallBack(weight);
    }

    return out;
}

unsigned char finalString[STORYSIZE];
int lastType = 0;
int tracker = 0;
int makeString(char* topic, int iteration, int type, int size, int firstOne) {

    int continueSentence = getrand(4);

    if (lastType == 4) {
        num = 5;
    } else if (lastType == 5) {
        num = 2;
    } else if (lastType == 2) {
        num = 4;
    } else {
        num = 1;
    }

    WORDS* first = markov(topic, 0, num);
    if (first == 0) return 0;

    int isPeriod = 0;
    if (type == 4 && lastType == 1) {
        first->bytes[strlen(first->bytes)] = '.';
        isPeriod = 1;
    } else {

        if (continueSentence == 1 && lastType == 4) {
            strcpy(first->bytes, "and ");
        } else {
            first->bytes[strlen(first->bytes)] = 0x20;
        }
    }

    first->bytes[strlen(first->bytes) + 1] = '\0';

    int j;
    for (j=0; j < strlen(first->bytes) + 1; j++) {
        if (tracker + j + strlen(first->bytes) >= sizeof(finalString)) break;
        if (first->bytes[j] == 00 || first->bytes[j] == 0x20) {
            finalString[tracker + j] = 0x20;
            break;
        }
        finalString[tracker + j] = first->bytes[j];
        // printf("%02X ", finalString[tracker + j]);
    }

    tracker += j + 1;

    if (isPeriod == 1 && tracker >= sizeof(finalString) - (sizeof(finalString) / 10)) {
        return 0;
    }

    if (iteration == size) return 0;

    lastType = type;
    iteration++;
    makeString(first->bytes, iteration, num, size, 0);

    return 0;
}

int main() {

    for (int i=0; i < sizeof(finalString); i++) {
        finalString[i] = 00;
    }

    for (int i=0; i < 5; i++) {
        for (int j=0; j < 20; j++) {
            map[i].entry[j].weight = 0;
            strcpy(map[i].entry[j].bytes, "null");
            map[i].entry[j].nextBytes = 0;
        }
    }

    // init
    num = getrand(100);
    initMarkov();

    int randNoun = getrand(25);
    makeString(nouns[randNoun], 0, 5, sizeof(finalString), 1);

    printf("%s\n", finalString);

}
