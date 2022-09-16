
# Special thanks to: 
#   https://stackoverflow.com/questions/16414410/delete-empty-lines-using-sed
curl https://raw.githubusercontent.com/jurliyuuri/praige-zerp/master/%E7%87%90%E5%AD%97%E5%85%A5%E5%8A%9B_r/lang/praige_r.dict.yaml -o lang/praige_r.dict.yaml
sed '0,/^\.\.\./d' lang/praige_r.dict.yaml | awk 'NF' | tr " " "_" > lang/praige_r.tsv
echo '#include "lang/praige_r_dict.hpp"' > lang/praige_r_dict.cpp
echo 'struct PekzepChar dict[] = {' >> lang/praige_r_dict.cpp
cat lang/praige_r.tsv | awk '{ gsub(/[^\t]+/,"U\x22&\x22"); print "{",$1,",",$2,"}," }' >> lang/praige_r_dict.cpp
echo '};' >> lang/praige_r_dict.cpp
echo 'struct PekzepChar *dict_end = dict + (sizeof dict / sizeof *dict);' >> lang/praige_r_dict.cpp