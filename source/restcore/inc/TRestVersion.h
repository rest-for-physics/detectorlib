#ifndef REST_Version
#define REST_Version
/* Version information automatically generated by installer. *//*
 * These macros can be used in the following way:
 * 
 * #if REST_VERSION_CODE >= REST_VERSION(2,23,4)
 *     #include <newheader.h>
 * #else
 *     #include <oldheader.h>
 * #endif
 *
 */
#define REST_RELEASE "2.2.5"
#define REST_RELEASE_DATE "2018-12-20"
#define REST_RELEASE_TIME "17:09:49 +0100"
#define REST_GIT_COMMIT "571f6d2c"
#define REST_GIT_BRANCH "master"
#define REST_GIT_TAG "v2.2.5"
#define REST_VERSION_CODE 131589
#define REST_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#define REST_SCHEMA_EVOLUTION "ON"
#endif
