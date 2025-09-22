/**

- fread(3) / fwrite(3):

  - I/O type : Unformatted (binary) I/O
  - Unit     : Objects of given size (often bytes, but can be larger if size > 1)
  - Behavior : Direct copy between memory and file with no conversion or interpretation
  - Usage    : Structs, arrays, images, serialized data, any binary format

- fscanf(3) / fprintf(3):

  - I/O type : Formatted (text) I/O
  - Unit     : Characters, parsed/printed according to format string
  - Behavior : Converts between internal representation and textual form
  - Usage    : Human-readable files, logs, configuration, CSV, text protocols

 */

#include <assert.h>
#include <stdio.h>
void test_character_stream()
{
  const char * const file = "tmp/data/stdio/charstream";
  FILE *fp = fopen(file, "w+");
  fprintf(fp, "%d", 16384);

  rewind(fp);

  int a;
  fscanf(fp, "%d", &a);
  assert(a == 16384);

  fclose(fp);
}

void test_byte_stream()
{
  const char * const file = "tmp/data/stdio/bytestream";
  FILE *fp = fopen(file, "w+");
  int a = 16384;
  fwrite(&a, sizeof(int), 1, fp);

  rewind(fp);

  int b;
  fread(&b, sizeof(int), 1, fp);
  assert(b == 16384);

  fclose(fp);
}

int main(int argc, char *argv[]) {
  test_character_stream();
  test_byte_stream();
  return 0;
}

/*

> hexdump tmp/data/stdio/charstream
0000000 3631 3833 0034
0000005
> hexdump -c tmp/data/stdio/charstream
0000000   1   6   3   8   4
0000005
> cat tmp/data/stdio/charstream
16384%
> hexdump tmp/data/stdio/bytestream
0000000 4000 0000
0000004
> hexdump -c tmp/data/stdio/bytestream
0000000  \0   @  \0  \0
0000004
> cat tmp/data/stdio/bytestream
@%                                    # @: ascii (0x40)

 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
