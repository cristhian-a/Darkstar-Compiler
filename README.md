#

cmake -S . -B build/    # cria os build files

cmake --build build/    # faz o build

./build/darkstar        # executa

./build/darkstar uepa   # executa e passa 'uepa' como argumento

cmake --build build/

./build/darkstar ./hello.ds && (./target/out ; echo $?) # executa o compilador, o programa, e imprime a última chamada no console
