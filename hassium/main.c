#include <hassium.h>

int main(int argc, char *argv[]) {
  char *code = file_to_str(argv[1]);
  run_string(code);
  free(code);
}
