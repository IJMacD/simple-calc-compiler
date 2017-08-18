#include <stdio.h>

#include "lexer.c"
#include "parser.c"
#include "generator.c"
#include "linker.c"

int main(int argc, char **argv){
  char input_buffer[255] = { 0 };
  int retain_output = 0;
  int print_output = 0;
  int verbose = 0;

  if(argc > 1) {
    int i;
    int offset = 0;
    for(i = 1; i < argc; i++) {
      if(argv[i][0] == '-') {
        if(argv[i][1] == 'r') {
          retain_output = 1;
        }
        else if(argv[i][1] == 'p') {
          print_output = 1;
        }
        else if(argv[i][1] == 'v') {
          verbose = 1;
        }
        else {
          printf("Unknown option %s\n", argv[i]);
          exit(-1);
        }
      }
      else {
        strcpy(&input_buffer[offset], argv[i]);
        offset += strlen(argv[i]);
        input_buffer[offset] = ' ';
        offset++;
      }
    }

    input_buffer[offset] = '\0';
  }

  if(!strlen(input_buffer)) {
    strcpy(input_buffer, "add 5 subtract 4 2");
  }

  token_list *tokens = lexer(input_buffer);

  if (verbose) {
    printf("%d tokens found\n", tokens->length);

    int i;
    for(i = 0; i < tokens->length; i++) {
      printf("%s\n", tokens->list[i].value);
    }
  }

  ast_node *root_node = parser(tokens);

  if (verbose) {
    debug_node(root_node->param1);
  }

  int linker_flags = 0;

  char *program = generator(root_node, &linker_flags);

  char *output = linker(program, linker_flags);

  free_tokens(tokens);
  free_node(root_node);
  free(program);

  if(print_output) {
    printf("%s\n", output);
  }

  FILE *f = fopen("output.c", "w");
  fprintf(f, "%s", output);
  fclose(f);

  free(output);

#ifdef linux
  system("clang output.c -o output && ./output && rm output");
#endif

  if (!retain_output) {
    remove("output.c");
  }
}

/*    TRAVERSER   */

/*    TRANSFORMER   */

/*    CODE GENERATOR    */

/*    COMPILER    */
