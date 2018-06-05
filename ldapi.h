
#include <stdbool.h>

#include "uthash.h"

#ifdef __cplusplus
#include <string>

extern "C" {
#endif

typedef enum {
    LDNodeNone = 0,
    LDNodeString,
    LDNodeNumber,
    LDNodeBool,
    LDNodeMap,
    LDNodeArray,
} LDNodeType;

typedef struct LDNode_i {
    union {
        char *key;
        unsigned int idx;
    };
    LDNodeType type;
    union {
        bool b;
        char *s;
        double n;
        struct LDNode_i *m;
        struct LDNode_i *a;
    };
    UT_hash_handle hh;
#ifdef __cplusplus
    struct LDNode_i *lookup(const std::string &key);
    void release(void);
#endif
} LDNode;

typedef struct LDConfig_i {
    bool allAttributesPrivate;
    int backgroundPollingIntervalMillis;
    char *appURI;
    int connectionTimeoutMillis;
    bool disableBackgroundUpdating;
    int eventsCapacity;
    int eventsFlushIntervalMillis;
    char *eventsURI;
    char *mobileKey;
    bool offline;
    int pollingIntervalMillis;
    LDNode *privateAttributeNames;
    bool streaming;
    char *streamURI;
    bool useReport;
} LDConfig;

typedef struct LDUser_i {
    char *key;
    bool anonymous;
    char *secondary;
    char *ip;
    char *firstName;
    char *lastName;
    char *email;
    char *name;
    char *avatar;
    LDNode *custom;
    LDNode *privateAttributeNames;
} LDUser;

struct LDClient_i;

#if !defined(__cplusplus) && !defined(LD_C_API)
typedef struct LDClient_i LDClient;
#endif


void LDSetString(char **, const char *);

LDConfig *LDConfigNew(const char *);
LDUser *LDUserNew(const char *);
bool LDUserSetCustomAttributesJSON(LDUser *user, const char *jstring);

struct LDClient_i *LDClientInit(LDConfig *, LDUser *);
struct LDClient_i *LDClientGet(void);

void LDConfigAddPrivateAttribute(LDConfig *, const char *name);
void LDUserAddPrivateAttribute(LDUser *, const char *name);


void LDUserSetIP(LDUser *user, const char *str);
void LDUserSetFirstName(LDUser *user, const char *str);
void LDUserSetLastName(LDUser *user, const char *str);
void LDUserSetEmail(LDUser *user, const char *str);
void LDUserSetName(LDUser *user, const char *str);
void LDUserSetAvatar(LDUser *user, const char *str);


char *LDClientSaveFlags(struct LDClient_i *client);
void LDClientRestoreFlags(struct LDClient_i *client, const char *data);

void LDClientIdentify(struct LDClient_i *, LDUser *);

void LDClientFlush(struct LDClient_i *client);
bool LDClientIsInitialized(struct LDClient_i *);
bool LDClientIsOffline(struct LDClient_i *);
void LDClientSetOffline(struct LDClient_i *);
void LDClientSetOnline(struct LDClient_i *);
void LDClientClose(struct LDClient_i *);

void LDSetClientStatusCallback(void (callback)(int));

bool LDBoolVariation(struct LDClient_i *, const char *, bool);
int LDIntVariation(struct LDClient_i *, const char *, int);
double LDDoubleVariation(struct LDClient_i *, const char *, double);
char *LDStringVariationAlloc(struct LDClient_i *, const char *, const char *);
char *LDStringVariation(struct LDClient_i *, const char *, const char *, char *, size_t);
LDNode *LDJSONVariation(struct LDClient_i *client, const char *key, LDNode *);
void LDJSONRelease(LDNode *m);

void LDFree(void *);
void *LDAlloc(size_t amt);

/* functions for working with (JSON) nodes (aka hash tables) */
LDNode *LDNodeCreateHash(void);
void LDNodeAddBool(LDNode **hash, const char *key, bool b);
void LDNodeAddNumber(LDNode **hash, const char *key, double n);
void LDNodeAddString(LDNode **hash, const char *key, const char *s);
void LDNodeAddMap(LDNode **hash, const char *key, LDNode *m);
void LDNodeAddArray(LDNode **hash, const char *key, LDNode *a);
LDNode *LDNodeLookup(LDNode *hash, const char *key);
void LDNodeFree(LDNode **hash);
unsigned int LDNodeCount(LDNode *hash);
/* functions for treating nodes as arrays */
LDNode *LDNodeCreateArray(void);
void LDNodeAppendBool(LDNode **array, bool b);
void LDNodeAppendNumber(LDNode **array, double n);
void LDNodeAppendString(LDNode **array, const char *s);
LDNode *LDNodeIndex(LDNode *array, unsigned int idx);

void LDSetLogFunction(int userlevel, void (userlogfn)(const char *));

/*
 * store interface. open files, read/write strings, ...
 */
typedef void *(*LD_store_opener)(void *, const char *, const char *, size_t);
typedef bool (*LD_store_stringwriter)(void *, const char *data);
typedef const char *(*LD_store_stringreader)(void *);
typedef void (*LD_store_closer)(void *);

void
LD_store_setfns(void *context, LD_store_opener, LD_store_stringwriter, LD_store_stringreader, LD_store_closer);

void *LD_store_fileopen(void *, const char *name, const char *mode, size_t len);
bool LD_store_filewrite(void *h, const char *data);
const char *LD_store_fileread(void *h);
void LD_store_fileclose(void *h);

/*
 * listener function for flag changes.
 * arguments:
 * flag name
 * change type - 0 for new or updated, 1 for deleted
 */
typedef void (*LDlistenerfn)(const char *, int);
/*
 * register a new listener.
 */
bool LDClientRegisterFeatureFlagListener(struct LDClient_i *, const char *, LDlistenerfn);
bool LDClientUnregisterFeatureFlagListener(struct LDClient_i *, const char *, LDlistenerfn);

#ifdef __cplusplus
}


class LDClient {
    public:
        static LDClient *Get(void);
        static LDClient *Init(LDConfig *, LDUser *);

        bool isInitialized(void);

        bool boolVariation(const std::string &, bool);
        int intVariation(const std::string &, int);
        double doubleVariation(const std::string &, double);
        std::string stringVariation(const std::string &, const std::string &);
        char *stringVariation(const std::string &, const std::string &, char *, size_t);

        LDNode *JSONVariation(const std::string &, LDNode *);

        void setOffline();
        void setOnline();
        bool isOffline();

        std::string saveFlags();
        void restoreFlags(const std::string &);

        void flush(void);
        void close(void);
    private:
        struct LDClient_i *client;
};


#endif