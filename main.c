// #include <argp.h>
#include "punky.h"

// TODO maybe use getopt(_long)?
char *punky_usage = "Usage: punky [OPTION...]\n\
punky - a simple functional language interpreter\n\
\n\
  -d, --debug                enable debugging output\n\
  -i, --input=FILE           read input from FILE (default: stdin)\n\
  -I, --indent=NUM           amount to indent pretty-printed expressions\n\
                             (default: 2; only works when -p is specified)\n\
  -o, --output=FILE          write output to FILE (default: stdout)\n\
  -p, --parse-only           print parsed input expressions without evaluating\n\
                             them\n\
  -P, --pretty-print         pretty-print the output\n\
  -?, --help                 Give this help list\n\
      --usage                Give a short usage message\n\
  -V, --version              Print program version\n\
\n\
Mandatory or optional arguments to long options are also mandatory or optional\n\
for any corresponding short options.\n\
\n\
Report bugs to <cliff.snyder@gmail.com>.\n";

// TODO use autotools for this!
#define PACKAGE_STRING "1.0"
#define PACKAGE_BUGREPORT "cliff.snyder@gmail.com"

void punky_parse_args(punky_t *p, int argc, char *argv[])
{
  int i;
  for(i = 1; i < argc; i++) {
    if((strcmp("-i", argv[i]) == 0) || (strcmp("--input", argv[i]) == 0)) {
      if(i == (argc - 1)) {
	fprintf(stderr, "error: %s requires an argument\n", argv[i]);
	fprintf(stderr, "%s", punky_usage);
	exit(1);
      } else {
	p->in = (strcmp("-", argv[i+1]) == 0) ? stdin : fopen(argv[i+1], "r");
	i++;
      }
    } else if((strcmp("-o", argv[i]) == 0) || (strcmp("--output", argv[i]) == 0)) {
      if(i == (argc - 1)) {
	fprintf(stderr, "error: %s requires an argument\n", argv[i]);
	fprintf(stderr, "%s", punky_usage);
	exit(1);
      } else {
	p->out = (strcmp("-", argv[i+1]) == 0) ? stdout : fopen(argv[i+1], "w");
	i++;
      }
    } else if((strcmp("-d", argv[i]) == 0) || (strcmp("--debug", argv[i]) == 0)) {
      p->debug = 1;
    } else if((strcmp("-p", argv[i]) == 0) || (strcmp("--parse-only", argv[i]) == 0)) {
      p->eval = 0;
    } else if((strcmp("-P", argv[i]) == 0) || (strcmp("--pretty-print", argv[i]) == 0)) {
      p->pretty = 1;
    } else if((strcmp("-I", argv[i]) == 0) || (strcmp("--indent", argv[i]) == 0)) {
      if(i == (argc - 1)) {
	fprintf(stderr, "error: %s requires an argument\n", argv[i]);
	fprintf(stderr, "%s", punky_usage);
	exit(1);
      } else {
	p->indent = atoi(argv[i+1]);
	i++;
      }
    } else if((strcmp("-?", argv[i]) == 0) || (strcmp("--help", argv[i]) == 0)|| (strcmp("--usage", argv[i]) == 0)) {
      printf("%s", punky_usage);
      exit(0);
    } else if((strcmp("-V", argv[i]) == 0) || (strcmp("--version", argv[i]) == 0)) {
      printf("punky %s\n", PACKAGE_STRING);
      exit(0);
    } else {
      fprintf(stderr, "unknown argument: %s\n", argv[i]);
      fprintf(stderr, "%s", punky_usage);
      exit(1);
    }
  }
}

// const char *argp_program_version = PACKAGE_STRING;
// const char *argp_program_bug_address = PACKAGE_BUGREPORT;
// static char doc[] = "punky - a simple functional language interpreter";
// static char args_doc[] = { 0 }; // no mandatory args

/*
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
*/

/*
static error_t parse_opt (int key, char *arg, struct argp_state *state){

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
*/

// static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char *argv[])
{
  punky_t p;
  punky_init(&p);
  // argp_parse (&argp, argc, argv, 0, 0, &p);
  punky_parse_args(&p, argc, argv);

  while(punky_read(&p)) {
    if(p.eval && !IS_ERR(p.e)) {
      expr_t *e = p.e->eval(p.env, p.e);
      // free what we parsed
      _free_expr(p.e);
      p.e = e;
    }
    punky_print(&p);
    // free it once we've printed it
    _free_expr(p.e);
  }
  punky_cleanup(&p);
  exit(0);
}
