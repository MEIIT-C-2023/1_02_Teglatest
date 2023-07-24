#include <fcntl.h>
#define main main1
#include "main.c"

#undef main

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Automated.h>

FILE *f;
char text[200] = "";

void create_test_values(char * _input)
{
  f = fopen("in", "w");
  fputs(_input, f);
  fclose(f);
}

void setup_test(char * _input)
{
  // put test values into "in" file.
  create_test_values(_input);

  // use file input to mock stdin
  f = freopen("in", "r", stdin);
  if (f == NULL)
  {
    printf("failed to open file\n");
  }

  // redirect stdout to "file", and save stdout
  int out = open("output", O_WRONLY | O_CREAT | O_TRUNC, 0777);
  int saved = dup(1);

  close(1);
  dup(out);
  close(out);

  main1();

  fflush(stdout);
  // restore stdout and close the files
  dup2(saved, 1);
  close(saved);
  fclose(f);
}

void append(char *s, char c)
{
  int len = strlen(s);
  s[len] = c;
  s[len + 1] = '\0';
}

char *get_result()
{
  // open file
  FILE *textfile;
  textfile = fopen("output", "r");

  // check if there has been messages on the terminal.
  if (textfile == NULL)
  {
    printf("\nNo messages printed to the terminal!\n");
  }

  rewind(textfile);

  // read file char by char until EOF
  char ch;
  while (1)
  {
    ch = fgetc(textfile);
    if (ch == EOF)
      break;
    append(text, ch);
  }
  fclose(textfile);

  return text;
}

void volume_test()
{
  CU_ASSERT_TRUE(strstr(text, "200") != NULL);
}

void surface_test()
{
  CU_ASSERT_TRUE(strstr(text, "220") != NULL);
}

int init_suite()
{
  setup_test("10, 5, 4");
  get_result();

  return 0;
}

int cleanup_suite()
{
  // delete temporary files
  if (remove("in") == 0 && remove("output") == 0)
    return 0;
}

//formatted for markdown. Will be sent feedback as a comment to the student's commit.
void printFailedTests()
{
  CU_pFailureRecord fr = CU_get_failure_list();
  if (fr != NULL)
  {
    printf("A kod at alabbi teszteken megbukott:\n * ");
    printf(fr->pTest->pName);
    while (fr->pNext)
    {
      fr = fr->pNext;
      printf("\n * ");
      printf(fr->pTest->pName);
    }
    //hibak:
    printf("\n> Jellemzo hibak lehetnek: \n>* Hibas beolvasasi formatum \n>* Hibas/hianyzo eredmeny\n>* Nem megfelelo formazas (pl. tizedesjegyek szama)");
  }
  else
  {
    printf("Minden teszt sikeres!");
  }
}

int main()
{
  CU_initialize_registry();
  CU_pSuite suite = CU_add_suite("Teglatest test", init_suite, cleanup_suite);
  CU_add_test(suite, "Felszin teszt", surface_test);
  CU_add_test(suite, "Terfogat teszt", volume_test);
  CU_basic_set_mode(CU_BRM_VERBOSE);

  printf("\n<!--");
  CU_basic_run_tests();
  printf("\n-->");

  printFailedTests();
  CU_cleanup_registry();

  return 0;
}
