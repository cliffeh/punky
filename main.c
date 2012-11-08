#include <argp.h>
#include "punky.h"

// TODO use autotools for this!
#define PACKAGE_STRING "1.0"
#define PACKAGE_BUGREPORT "cliff.snyder@gmail.com"

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;
static char doc[] = "punky - a simple functional language interpreter";
static char args_doc[] = { 0 }; // no mandatory args

static struct argp_option options[] = {
  { "input",         'i', "FILE",   0, 
    "read input from FILE (default: stdin)" },
  { "output",        'o', "FILE",   0, 
    "write output to FILE (default: stdout)" },
  { "parse-only",    'p', 0,        0,
    "print parsed input expressions without evaluating them" },
  { "pretty-print",  'P', 0,        0,
    "pretty-print the output" },
  { "indent",        'I', "NUM",    0,
    "amount to indent pretty-printed expressions (default: 2; only works when -p is specified)" },
  { "debug",         'd', 0,        0,
    "enable debugging output" },
  { 0 }
};

static error_t parse_opt (int key, char *arg, struct argp_state *state){
  /* get the input argument from argp_parse, which we
     know is a pointer to our arguments structure */
  punky_t *args = state->input;

  switch(key){
  case 'i': args->in = (strcmp("-", arg) == 0) ? stdin : fopen(arg, "r"); break;
  case 'o': args->out = (strcmp("-", arg) == 0) ? stdout : fopen(arg, "w"); break;
  case 'd': args->debug = 1; break;
  case 'p': args->eval = 0; break;
  case 'P': args->pretty = 1; break;
  case 'I': args->indent = atoi(arg); break;
  default: return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char *argv[])
{
  punky_t p;
  init(&p);
  argp_parse (&argp, argc, argv, 0, 0, &p);

  expr_t *e;
  while(read(&p)) {
    if(p.eval) eval(&p);
    if(p.e) print(&p);
  }
  cleanup(&p);
  exit(0);
}
