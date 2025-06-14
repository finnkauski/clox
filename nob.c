#include <string.h>
#define NOB_IMPLEMENTATION

#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  Nob_Cmd cmd = {0};
  nob_cmd_append(&cmd, "gcc", "-Wall", "-Wextra", "-fPIE", "-g");
  nob_cmd_append(&cmd, "-I" SRC_FOLDER);
  nob_cmd_append(&cmd, "-o", "clox");
  nob_cmd_append(&cmd, SRC_FOLDER "/main.c");
  nob_cmd_append(&cmd, SRC_FOLDER "/lexer.c");
  nob_cmd_append(&cmd, SRC_FOLDER "/stb_ds.c");
  nob_cmd_append(&cmd, SRC_FOLDER "/utils.c");
  if (!nob_cmd_run_sync_and_reset(&cmd))
    return 1;

  return 0;
}
