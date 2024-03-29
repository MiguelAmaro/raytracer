@echo off

set FLAGS=-nologo -std:c11 -Zi -Od -IF:\Dev\raytracer\src\ -IF:\Dev\raytracer\thirdparty\pcg -IF:\Dev\raytracer\thirdparty\stb

cl %FLAGS% test_pdf.c /link advapi32.lib