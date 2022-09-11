#include <hassium.h>

int main(int argc, char *argv[]) {
  hassium_ctx.prog_path = argv[1];

  run_file(hassium_ctx.prog_path);

  done();
}
