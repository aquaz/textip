#ifndef TEXTIP_UTILS_MPL_H
#define TEXTIP_UTILS_MPL_H

#include <boost/mpl/if.hpp>

template <typename T, bool Const>
using constify = typename boost::mpl::if_c<Const, const T, T>::type;

#endif /* !TEXTIP_UTILS_MPL_H */
