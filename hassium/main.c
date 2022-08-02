#include <hassium.h>

int main(int argc, char *argv[]) {
  hassium_ctx.prog_path = argv[1];
  char *code = file_to_str(argv[1]);
  run_string(code);
  free(code);
}
