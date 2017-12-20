# isrunning.exe

## Description

*isrunning.exe* is a very compact Win32 console application that verifies if a
process is running based on the full path of its executable. Its interface is
desiged to be minimum so it will never print values into the screen, only the
return code will be used to report the results of the execution.

This code was originally written by *NoctuaBR* and released as OSS in 2017.

## Usage

```
isrunning.exe <full path to the executable>
```

Examples:

```
isrunning.exe "c:\windows\system32\notepad.exe"
```

### Return codes:

	0 - Process found
	1 - Invalid argument
	2 - Unable to convert a full path to a device path
	3 - Process not found/not running
	4 - Unable to find the function K32GetProcessImageFileName

## How to compile

### Dependencies

In order to compile this small program, you need to install CLang for Windows
and a Windows SDK.

It is also possible to compile this program using Microsoft Visual Studio

### Compiling

Open a command prompt inside the source directory and run compile.bat.

### Why CLang

This program was first written to Microsoft Visual C++. However, in 2017,
some Anti-Virus started to detect the generated code (in Release mode) as
a variant of a virus (tested using VirusTotal).

The same is not true for CLang so we started to use it instead of MSVC.

## Licensing

This program is licensed under a 3-clause BSD license.
