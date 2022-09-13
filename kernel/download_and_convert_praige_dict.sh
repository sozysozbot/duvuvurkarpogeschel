
# Special thanks to: 
#   https://stackoverflow.com/questions/16414410/delete-empty-lines-using-sed
curl https://raw.githubusercontent.com/jurliyuuri/praige-zerp/master/%E7%87%90%E5%AD%97%E5%85%A5%E5%8A%9B_r/praige_r.dict.yaml -o praige_r.dict.yaml
sed '0,/^\.\.\./d' praige_r.dict.yaml | awk 'NF' | tr " " "_" > praige_r.tsv
echo '#include "praige_r_dict.h"' > praige_r_dict.c
echo 'struct PekzepChar dict[] = {' >> praige_r_dict.c
cat praige_r.tsv | awk '{ gsub(/[^\t]+/,"\x22&\x22"); print "{",$1,",",$2,"}," }' >> praige_r_dict.c
echo '};' >> praige_r_dict.c
echo 'struct PekzepChar *dict_end = dict + (sizeof dict / sizeof *dict);' >> praige_r_dict.c