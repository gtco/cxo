%ifndef SYMBOL_ASM
%define SYMBOL_ASM

%ifdef NEED_UNDERSCORE

%macro EXPORT 1
[GLOBAL _%1]
%define %1 _%1
%endmacro

%macro IMPORT 1
[EXTERN _%1]
%define %1 _%1
%endmacro

%else

%macro EXPORT 1
[GLOBAL %1]
%endmacro

%macro IMPORT 1
[EXTERN %1]
%endmacro

%endif

%endif

