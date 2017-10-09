#ifndef AUTOWIG__SEQUENCE_ANALYSIS
#define AUTOWIG__SEQUENCE_ANALYSIS

#include <boost/python.hpp>
#include <type_traits>
#include <sequence_analysis/semi_markov.h>
#include <sequence_analysis/base.h>
#include <sequence_analysis/renewal.h>
#include <sequence_analysis/hidden_semi_markov.h>
#include <sequence_analysis/variable_order_markov.h>
#include <sequence_analysis/nonhomogeneous_markov.h>
#include <sequence_analysis/sequences.h>
#include <sequence_analysis/sequence_label.h>
#include <sequence_analysis/hidden_variable_order_markov.h>

namespace autowig
{
     template<class T> struct Held {
        typedef T* Type;
        static bool const is_class = false;
    };
}

#endif