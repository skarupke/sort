clear
echo "=================================================================="
echo "==             B E N C H M A R K   S T R I N G S                =="
echo "==                                                              =="
echo "==                 G C C      C O M P I L E R                   =="
echo "=================================================================="
echo "."
echo "C O M P I L I N G . . . . . . . . . . ."
echo "."
g++ ./file_generator.cpp -std=c++17 -march=native -w -fexceptions -O3 -I../../include  -s  -o file_generator

g++ ./benchmark_strings.cpp ./memory_usage.cpp -std=c++17 -march=native -w -fexceptions -O3 -pthread -I../../include -s  -o benchmark_strings

echo "R U N N I N G . . . . . . . . . . ."
echo "( The time needed is around 10 minutes depending on your machine )"
./file_generator input.bin 125000000
echo "."
date
./benchmark_strings
date

rm input.bin
rm file_generator
rm benchmark_strings
echo "."
echo "."
echo "E N D"
echo "."
