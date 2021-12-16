#ifndef PGE_TEMPLATE_ENABLE_IF

#define PGE_TEMPLATE_ENABLE_IF(PREDICATE) template <typename Y = T, typename = typename std::enable_if<PREDICATE>::type>

#endif // PGE_TEMPLATE_ENABLE_IF
