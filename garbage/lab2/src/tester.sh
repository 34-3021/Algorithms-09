sudo apt-get update
sudo apt-get install -y gcc-14
sudo apt-get install -y g++-14

g++-14 main2.cpp -o main -Ofast -march=native -std=c++23

# ./main reference.txt query.txt, output will be plot.txt

./main reference.txt query.txt

mv plot.txt plot1.txt

./main reference2.txt query2.txt

mv plot.txt plot2.txt