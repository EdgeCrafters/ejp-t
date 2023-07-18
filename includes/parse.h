#pragma once

#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

const int bufSize = 512;
const char basicEntry[] = "#include <stdio.h>\n#include \"common.h\"\nint main(int argc, char*argv[])\n{\n";
const char basicClosing[] = "\nprintf(\"%d\",diff(input,output));\nreturn 0;\n}";

const char inputEntry[] = "char input[] = \"";
const char inputClosing[] = "\";";

const char outputEntry[] = "char output[] = \"";
const char outputClosing[] = "\";";

int gen(int argc, char*argv[]);
