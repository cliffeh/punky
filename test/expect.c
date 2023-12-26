#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 4096
#define EXPECT_STRING ";expect: "

int
main (int argc, char *argv[])
{
  FILE *fp;
  char buf[BUFSIZE], expect[BUFSIZE], *p;
  int errcount = 0, lineno = 0;

  if (argc != 2)
    {
      fprintf (stderr, "usage: %s FILE\n", argv[0]);
      exit (99); // hard error
    }
  else if (!(fp = fopen (argv[1], "r")))
    {
      fprintf (stderr, "couldn't open file '%s': %s", argv[1],
               strerror (errno));
      exit (99); // hard error
    }

  while (fgets (buf, BUFSIZE, fp))
    {
      lineno++;
      if ((p = strrchr (buf, '\n'))) // strip newline
        *p = 0;

      if ((p = strstr (buf, EXPECT_STRING)))
        {
          strcpy (expect, p + strlen (EXPECT_STRING));

          char *read = fgets (buf, BUFSIZE, stdin);
          if ((p = strrchr (buf, '\n'))) // strip newline
            *p = 0;

          fprintf (stdout, "expect: %s | got: %s\n", expect, buf);

          if (!read || strcmp (buf, expect) != 0)
            errcount++;
        }
    }

  return errcount;
}