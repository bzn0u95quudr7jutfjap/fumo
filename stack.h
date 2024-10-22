#ifndef STACK_TEMPLATE
#define STACK_TEMPLATE

#ifndef NULL
#define NULL ((void *)0)
#endif

void *realloc(void *data, unsigned long capacity);

#include "int.h"

#define declare_template_stack_internal(                                       \
    type, stack_type, iter_type, new, free, resize, at, push, pop, ibegin,     \
    iisend, ipeekoff, ipeek, iget, iseekcur, iseekset, iseekend)               \
  typedef struct stack_type stack_type;                                        \
  typedef struct iter_type iter_type;                                          \
  struct stack_type {                                                          \
    u32 size;                                                                  \
    u32 capacity;                                                              \
    type *data;                                                                \
  };                                                                           \
  struct iter_type {                                                           \
    u32 i;                                                                     \
    stack_type *stack;                                                         \
  };                                                                           \
  stack_type new ();                                                           \
  u0 free(stack_type *stack);                                                  \
  u0 resize(stack_type *stack, u32 capacity);                                  \
  type *at(stack_type *, u32 i);                                               \
  u0 push(stack_type *, type data);                                            \
  u0 pop(stack_type *);                                                        \
  iter_type ibegin(stack_type *stream);                                        \
  u8 iisend(iter_type *stream);                                                \
  type *ipeekoff(iter_type *stream, i32 o);                                    \
  type *iseekcur(iter_type *stream, i32 o);                                    \
  type *iseekset(iter_type *stream, i32 o);                                    \
  type *iseekend(iter_type *stream, i32 o);                                    \
  type *ipeek(iter_type *stream);                                              \
  type *iget(iter_type *stream)

#define define_template_stack_internal(                                        \
    type, stack_type, iter_type, new, free, resize, at, push, pop, ibegin,     \
    iisend, ipeekoff, ipeek, iget, iseekcur, iseekset, iseekend)               \
  stack_type new () {                                                          \
    return (stack_type){.size = 0, .capacity = 0, .data = NULL};               \
  }                                                                            \
                                                                               \
  u0 resize(stack_type *stack, u32 capacity) {                                 \
    stack->data = (typeof(stack->data))realloc(                                \
        stack->data, (stack->capacity = capacity) * sizeof(*(stack->data)));   \
  }                                                                            \
                                                                               \
  type *at(stack_type *stack, u32 i) {                                         \
    return (i < stack->size)                                                   \
               ? &(stack->data[i])                                             \
               : ((i >= -(stack->size)) ? &(stack->data[i + stack->size])      \
                                        : NULL);                               \
  }                                                                            \
                                                                               \
  u0 push(stack_type *stack, type data) {                                      \
    if (stack->size >= stack->capacity) {                                      \
      resize(stack, stack->size * 1.5 + 1);                                    \
    }                                                                          \
    stack->data[stack->size++] = data;                                         \
  }                                                                            \
                                                                               \
  u0 pop(stack_type *stack) {                                                  \
    if (stack->size <= stack->capacity / 3) {                                  \
      resize(stack, stack->capacity / 2);                                      \
    }                                                                          \
    stack->size--;                                                             \
  }                                                                            \
  iter_type ibegin(stack_type *stream) {                                       \
    return (iter_type){.i = 0, .stack = stream};                               \
  }                                                                            \
  u8 iisend(iter_type *stream) {                                               \
    return stream != NULL && stream->stack != NULL &&                          \
           stream->i >= stream->stack->size;                                   \
  }                                                                            \
  type *ipeekoff(iter_type *stream, i32 o) {                                   \
    if (iisend(stream)) {                                                      \
      return NULL;                                                             \
    }                                                                          \
    return at(stream->stack, stream->i + o);                                   \
  }                                                                            \
  type *ipeek(iter_type *stream) { return ipeekoff(stream, 0); }               \
  type *iseekcur(iter_type *stream, i32 o) {                                   \
    stream->i += o;                                                            \
    return ipeek(stream);                                                      \
  }                                                                            \
  type *iseekset(iter_type *stream, i32 o) {                                   \
    stream->i = o;                                                             \
    return ipeek(stream);                                                      \
  }                                                                            \
  type *iseekend(iter_type *stream, i32 o) {                                   \
    stream->i += stream->stack->size + o;                                      \
    return ipeek(stream);                                                      \
  }                                                                            \
  type *iget(iter_type *stream) {                                              \
    type *ret = ipeek(stream);                                                 \
    iseekcur(stream, 1);                                                       \
    return ret;                                                                \
  }                                                                            \
  extern type force_semicolon_at_end_##stack_type

#define declare_template_stack_c(type, stack, iterstack)                       \
  declare_template_stack_internal(                                             \
      type, stack, iterstack, new_##stack, free_##stack, resize_##stack,       \
      at_##stack, push_##stack, pop_##stack, ibegin_##iterstack,               \
      iisend_##iterstack, ipeekoff_##iterstack, ipeek_##iterstack,             \
      iget_##iterstack, iseekcur_##iterstack, iseekset_##iterstack,            \
      iseekend_##iterstack)

#define define_template_stack_c(type, stack, iterstack)                        \
  define_template_stack_internal(                                              \
      type, stack, iterstack, new_##stack, free_##stack, resize_##stack,       \
      at_##stack, push_##stack, pop_##stack, ibegin_##iterstack,               \
      iisend_##iterstack, ipeekoff_##iterstack, ipeek_##iterstack,             \
      iget_##iterstack, iseekcur_##iterstack, iseekset_##iterstack,            \
      iseekend_##iterstack)

#define declare_template_stack_cpp(type, stack, iterstack)                     \
  declare_template_stack_internal(                                             \
      type, stack, iterstack, new_##stack, free, resize, at, push, pop,        \
      ibegin, iisend, ipeekoff, ipeek, iget, iseekcur, iseeset, iseekend)

#define define_template_stack_cpp(type, stack, iterstack)                      \
  define_template_stack_internal(                                              \
      type, stack, iterstack, new_##stack, free, resize, at, push, pop,        \
      ibegin, iisend, ipeekoff, ipeek, iget, iseekcur, iseeset, iseekend)

#ifndef __cplusplus

#define define_template_stack define_template_stack_c
#define declare_template_stack declare_template_stack_c

#else

#define define_template_stack define_template_stack_cpp
#define declare_template_stack declare_template_stack_cpp

#endif

#endif // STACK_TEMPLATE
