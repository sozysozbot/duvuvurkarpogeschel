
# Special thanks to: 
#   https://stackoverflow.com/questions/16414410/delete-empty-lines-using-sed
curl https://raw.githubusercontent.com/jurliyuuri/praige-zerp/master/%E7%87%90%E5%AD%97%E5%85%A5%E5%8A%9B_r/language/praige_r.dict.yaml -o language/praige_r.dict.yaml
sed '0,/^\.\.\./d' language/praige_r.dict.yaml | awk 'NF' | tr " " "_" > language/praige_r.tsv
echo '#include "language/praige_r_dict.hpp"' > language/praige_r_dict.cpp
echo 'struct PekzepChar dict[] = {' >> language/praige_r_dict.cpp
cat language/praige_r.tsv | awk '{ gsub(/[^\t]+/,"U\x22&\x22"); print "{",$1,",",$2,"}," }' >> language/praige_r_dict.cpp
echo '};' >> language/praige_r_dict.cpp
echo 'struct PekzepChar *dict_end = dict + (sizeof dict / sizeof *dict);' >> language/praige_r_dict.cpp