#ifndef NAMESPACE_ID /* Configure your possibly-nested namespace-id */
# define NAMESPACE_ID ltl
#endif
#ifdef NAMESPACE_ID
# ifndef NAMESPACE_ID_IS_EMPTY
#  define NAMESPACE_ID_IS_EMPTY_(ID,ONE)ID##ONE
#  define NAMESPACE_ID_IS_EMPTY(NS_ID)NAMESPACE_ID_IS_EMPTY_(NS_ID,0x01)
#  if ! NAMESPACE_ID_IS_EMPTY(NAMESPACE_ID)
namespace NAMESPACE_ID {
#  endif
# elif ! NAMESPACE_ID_IS_EMPTY(NAMESPACE_ID)
}
# undef NAMESPACE_ID_IS_EMPTY
# undef NAMESPACE_ID_IS_EMPTY_
# endif
#endif
