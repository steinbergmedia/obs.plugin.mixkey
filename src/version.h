#define MAJOR_VERSION_STR "1"
#define MAJOR_VERSION_INT 1

#define SUB_VERSION_STR "0"
#define SUB_VERSION_INT 0

#define RELEASE_NUMBER_STR "0"
#define RELEASE_NUMBER_INT 0

#include "build_tag.h"

#define BUILD_NUMBER_INT TAG_BUILDNR
#define BUILD_NUMBER_STR TAG_BUILDNR_STR

#define PLUGIN_VERSION MAJOR_VERSION_STR "." SUB_VERSION_STR "." RELEASE_NUMBER_STR "." BUILD_NUMBER_STR
#define PLUGIN_NAME "MixKey"

#define stringFileDescription	"MixKey OBS Plugin"
#define stringEffectName		stringFileDescription
#define stringOriginalName		"MixKey.dll"
#define stringProduct			"MixKey OBS Plugin"
