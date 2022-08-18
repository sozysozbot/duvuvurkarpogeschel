#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../syscall.h"

const char *ans[][3]= {
  {"fla_leus", "<chertif>"  "fla leus lurfa chertif"},
  {"fiurs",    "", "fiursusti zu josxe co'st fudiurmo"},
  {"feucocar", "", "feucocarven velfez"},
  {"therdaes", "<ler> <el>", "chertif'i therdaes"},
  {"tarmzi",   "", "皇論之書'i lurfa"},
  {"chaiss",   "", "chaiss'i skurla"},
  {"cerxe",    "", "cerxe'i sisnest"},
  {"xel_aklu", "<chertif>", "aklurpti'a'i lurfa"},
  {"kotielss", "", "kotiel'i skurla"},
  {"kanti", "<cersva>", "cersva'it liusel'i co'c kanti"},
  {"mili", "<sinestususn>", "liestusnej io mili"},
  {"mejfert", "<chertif> <mej>",  "murtmirme'd mejestan mol fal chertif? la lex'i kinfit"},
  {"lurfa_kr", "<chertif>",  "krantie'i lurfa"},
  {"banmis", "",   "banmis's lurfa"},
  {"selsurle", "", "co'st fudiurmo leus ciliso'i cenees"},
  {"iska",    "<>"  ,"iskaes fua kirto karpo karpogeschel"},
  {"ekcanaxm", "<>", "ekcanal leus litarle"}
};

extern "C" void main(int argc, char** argv) {
  const char* query = "als";
  if (argc >= 2) {
    query = argv[1];
  }
  
  if (strcmp(query, "als") == 0) {
    for (size_t i = 0; i < sizeof ans / sizeof *ans; i++) {
      printf("%s %s: %s\n", ans[i][0], ans[i][1], ans[i][2]);
    }
  } else {
    bool found = false;
    for (size_t i = 0; i < sizeof ans / sizeof *ans; i++) {
      if (strcmp(query, ans[i][0]) == 0) {
        printf("%s %s: %s\n", ans[i][0], ans[i][1], ans[i][2]);
        found = true;
      }
    }
    if (!found) {
      printf("mi qune niv cersva z'es %s.\n", query);
    }
  }
  exit(0);
}
