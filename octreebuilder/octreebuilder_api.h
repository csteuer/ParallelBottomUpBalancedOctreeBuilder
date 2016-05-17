#pragma once

#if defined _WIN32 || defined __CYGWIN__
    #ifdef __GNUC__
      #define API_EXPORT_DECLARATION __attribute__ ((dllexport))
      #define API_IMPORT_DECLARATION __attribute__ ((dllimport))
    #else
      #define API_EXPORT_DECLARATION __declspec(dllexport)
      #define API_IMPORT_DECLARATION __declspec(dllimport)
    #endif
#else
  #if __GNUC__ >= 4
    #define API_EXPORT_DECLARATION __attribute__ ((visibility ("default")))
  #else
    #define API_EXPORT_DECLARATION
  #endif
  #define API_IMPORT_DECLARATION
#endif

#ifdef OCTREEBUILDER_EXPORTS
    #define OCTREEBUILDER_API API_EXPORT_DECLARATION
#else
    #define OCTREEBUILDER_API API_IMPORT_DECLARATION
#endif
