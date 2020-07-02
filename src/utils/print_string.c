#include "utils/print_string.h"

#include <string.h>

void mcc_print_string_literal(FILE *out, const char *string)
{
	int i = 0;
	while (string[i] != '\0') {
		switch (string[i]) {
		case '\n':
			fprintf(out, "\\n");
			break;
		case '\t':
			fprintf(out, "\\t");
			break;
		case '\\':
			fprintf(out, "\\\\");
			break;
		default:
			fprintf(out, "%c", string[i]);
			break;
		}
		i++;
	}
}
