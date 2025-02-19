# lilac::meta

Welcome to C++ metaprogramming hell!
If you're looking for the implementation of this library, they are located in `{root}/include/sapphire/core/meta`.

### Abstract

Calling conventions suck. Especially undocumented ones, that are only generated for tightly optimized code.
Unfortunately, this is a reality we must deal with when attempting to interface with binaries with functions
that were only meant to be called internally. Luckily, C++ offers some very basic metaprogramming that we
can abuse in order to invoke these functions without having to manually write inline assembly every function call.

### Design

Meta is mostly split into two user interfaces: `Function` and `Hook`. While these are not fully fleshed out yet, their
wrapper abilities are complete. They are generic interfaces to functions with non-conventional calling conventions,
which can be customized by subclassing the CallConv class and manipulating parameter packs (and tuples) to the
implementer's liking. These can then be used relatively normally by users, without tedious manual code.

### Implementation

Currently, there are three implemented calling conventions in Meta. These calling conventions are:
- Optcall. A calling convention generated by the Microsoft Visual C++ compiler for tightly optimized
internal functions in 32-bit Intel x86 executables. 
- Thiscall. The generic calling convention for member functions, wrapped so that they can be declared
independent of any classes.
- Membercall. A cross between optcall and thiscall, used for tightly optimized member functions.

### Optcall

Optcall follows a format similar to the standard 64-bit calling convention used on Microsoft Windows:
| Parameter type | 1 | 2 | 3 | 4 | 5+ |
| --- | --- | --- | --- | --- | --- |
| Floating point | xmm0 | xmm1 | xmm2 | xmm3 | stack |

The first and second integer / pointer type arguments go in ecx and edx, regardless of its position.
This calling convention pushes structures to the end of the parameter list before placing arguments in registers.

Caller cleans the stack.

This is not replicable normally, but using some manipulation of parameter packs, we can actually transform
a call to an optcall function to a `vectorcall` function, with the exception of stack cleaning. This must
be implemented with inline assembly, but it is more minimal than manual passing of parameters in assembly.

While this readme will not go into detail about how specifically this is implemented (as C++ metaprogramming
is absolutely horrible to look at and work with), effectively, the class that implements optcall uses a 
sequence of indices (generated by a filter) in order to grab variables at certain offsets of the parameter
pack given, restructuring the call to place it correctly into a `vectorcall` wrapper. Another "function"
uses the sequence of indices in order to calculate the stack fix generated from the `vectorcall` wrapper, and undoes
it with a single line of assembly.

### Membercall

Membercall is a cross between `thiscall` and optcall:
| Parameter type | 1 | 2 | 3 | 4 | 5+ |
| --- | --- | --- | --- | --- | --- |
| Floating point | Not possible | xmm1 | xmm2 | xmm3 | stack |
| Integers, pointers | ecx | stack | stack | stack | stack |

Callee cleans the stack, like in `thiscall`.

This is implemented similarly to optcall, but with less complexity, as no inline assembly needs to be generated
in order to take care of the conflicting stack cleanup between optcall and `vectorcall`.

### Thiscall
This is a well-documented calling convention, the default for member functions:
| Parameter type | 1 | 2+ |
| --- | --- | --- |
| Floating point | Not possible | stack |
| Integers, pointers | ecx | stack |

Callee cleans the stack.

This is only implemented in order to allow out-of-line declarations, as typically, `thiscall` is only permitted
on member functions.