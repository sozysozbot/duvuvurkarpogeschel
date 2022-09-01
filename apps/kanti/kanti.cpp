#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../syscall.h"

const char *cersvass[][3] = {
  {"fla_leus", "<chertif>",  "fla leus lurfa chertif"},
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
 // {"iska",    "<>"  ,"iskaes fua kirto karpo karpogeschel"},
 // {"ekcanaxm", "<>", "ekcanal leus litarle"}
};

const char *mels_ksf[][3] = {
  {"ksfnerfe", "<cersva>", "ksfnerfe fenxe cersva"}, 
  {"lkurf", "<kraxaiun>", "kraxaiun'i lkurf"},
  {"lkurf &lex", "", "lovik nestilen cersva'd lususurdieniep'i lkurf"},
  {"xel", "<chertif>", "krantie l'es chertif'i xel" },
  {"melfertal", "", "melfertalasti fon chertif"},
  {"melfertal", "<chesta>", "melfertalasti fon chertif z'en chesta'c"},
  {"karse", "<chertif o chesta>", "fonti'avertz stefart fon chertif o chesta"},
  {"ys", "", "ksf io lex mol lyjot'i ys"}
};

extern "C" void main(int argc, char** argv) {
  const char* query = "";
    if (argc >= 2) {
    query = argv[1];
  }
  
  if (strcmp(query, "") == 0) {
    printf("fenxe \"kanti mels_ksf\" fua lerssergo mels ksf.\n");
    printf("fenxe \"kanti cersvass\" fua lerssergo cersvass.\n");
  } else if (strcmp(query, "cersvass") == 0) {
    for (size_t i = 0; i < sizeof cersvass / sizeof *cersvass; i++) {
      printf("%s %s: %s\n", cersvass[i][0], cersvass[i][1], cersvass[i][2]);
    }
  } else if (strcmp(query, "mels_ksf") == 0) {
    for (size_t i = 0; i < sizeof mels_ksf / sizeof *mels_ksf; i++) {
      printf("%s %s: %s\n", mels_ksf[i][0], mels_ksf[i][1], mels_ksf[i][2]);
    }
  } else {
    bool found = false;
    for (size_t i = 0; i < sizeof mels_ksf / sizeof *mels_ksf; i++) {
      if (strcmp(query, mels_ksf[i][0]) == 0) {
        printf("%s %s: %s\n", mels_ksf[i][0], mels_ksf[i][1], mels_ksf[i][2]);
        found = true;
      }
    }
    for (size_t i = 0; i < sizeof cersvass / sizeof *cersvass; i++) {
      if (strcmp(query, cersvass[i][0]) == 0) {
        printf("%s %s: %s\n", cersvass[i][0], cersvass[i][1], cersvass[i][2]);
        found = true;
      }
    }
    if (!found) {
      printf("mi qune niv cersva z'es %s.\n", query);
    }
  }
  exit(0);
}
