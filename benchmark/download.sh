baseurl='http://pizzachili.dcc.uchile.cl/repcorpus/'
files='
./artificial/fib41.gz
./artificial/rs.13.gz
./artificial/tm29.gz
./pseudo-real/dblp.xml.00001.1.gz
./pseudo-real/dblp.xml.00001.2.gz
./pseudo-real/dblp.xml.0001.1.gz
./pseudo-real/dblp.xml.0001.2.gz
./pseudo-real/dna.001.1.gz
./pseudo-real/english.001.2.gz
./pseudo-real/proteins.001.1.gz
./pseudo-real/sources.001.2.gz
./real/Escherichia_Coli.gz
./real/cere.gz
./real/coreutils.gz
./real/einstein.de.txt.gz
./real/einstein.en.txt.gz
./real/influenza.gz
./real/kernel.gz
./real/para.gz
./real/world_leaders.gz
'

mkdir build

for f in ${files}; do
    echo "${f}"
    dir=$(dirname "${f}")
    name=$(basename "${f}")

    echo "curl -O ${baseurl}/${f}"
    curl -O ${baseurl}/${f}
done

gunzip *
