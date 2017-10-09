#ifndef SEQUENCE_ANALYSIS_BASE_H
#define SEQUENCE_ANALYSIS_BASE_H

#if defined WIN32 || defined _WIN32 || defined __CYGWIN__
  #ifdef LIBSEQUENCE_ANALYSIS
    #ifdef __GNUC__
      #define SEQUENCE_ANALYSIS_API __attribute__ ((dllexport))
    #else
      #define SEQUENCE_ANALYSIS_API __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define SEQUENCE_ANALYSIS_API __attribute__ ((dllimport))
    #else
      #define SEQUENCE_ANALYSIS_API __declspec(dllimport)
    #endif
  #endif
#else
  #if __GNUC__ >= 4
    #define SEQUENCE_ANALYSIS_API __attribute__ ((visibility ("default")))
  #else
    #define SEQUENCE_ANALYSIS_API
  #endif
#endif

#endif
